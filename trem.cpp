#include "trem.h"

#include <QDateTime>
#include <QDebug>

QMutex Trem::rc[9];

namespace {
constexpr int kPassoMovimento = 2;
constexpr int kTremLargura = 24;
constexpr int kTremAltura = 24;

QString coordenadasTexto(int x, int y)
{
    return QString("(%1,%2)").arg(x).arg(y);
}

QRect retanguloTrem(int x, int y)
{
    return QRect(x, y, kTremLargura, kTremAltura);
}
}

Trem::Trem(int id,
           const QPoint &posicaoInicial,
           const QVector<QPoint> &trajeto,
           QObject *parent)
    : QThread(parent),
      posicaoAtual(posicaoInicial),
      trajeto(trajeto),
      indiceProximoPonto(0),
      ID(id),
      velocidade(100)
{
    if (this->trajeto.size() > 1) {
        indiceProximoPonto = 1;
    }
}

void Trem::setVelocidade(int valor)
{
    if (valor < 0) {
        valor = 0;
    } else if (valor > 200) {
        valor = 200;
    }

    velocidade = valor;
}

int Trem::atrasoDaIteracao() const
{
    int atraso = 201 - velocidade;
    if (atraso < 1) {
        atraso = 1;
    } else if (atraso > 201) {
        atraso = 201;
    }

    return atraso;
}

QPoint Trem::preverProximaPosicao() const
{
    if (trajeto.size() < 2) {
        return posicaoAtual;
    }

    int indice = indiceProximoPonto;
    if (indice < 0 || indice >= trajeto.size()) {
        indice = 0;
    }

    QPoint destino = trajeto.at(indice);
    QPoint proxima = posicaoAtual;

    if (proxima == destino) {
        indice = (indice + 1) % trajeto.size();
        destino = trajeto.at(indice);
    }

    if (proxima.x() < destino.x()) {
        proxima.setX(proxima.x() + kPassoMovimento);
        if (proxima.x() > destino.x()) {
            proxima.setX(destino.x());
        }
    } else if (proxima.x() > destino.x()) {
        proxima.setX(proxima.x() - kPassoMovimento);
        if (proxima.x() < destino.x()) {
            proxima.setX(destino.x());
        }
    } else if (proxima.y() < destino.y()) {
        proxima.setY(proxima.y() + kPassoMovimento);
        if (proxima.y() > destino.y()) {
            proxima.setY(destino.y());
        }
    } else if (proxima.y() > destino.y()) {
        proxima.setY(proxima.y() - kPassoMovimento);
        if (proxima.y() < destino.y()) {
            proxima.setY(destino.y());
        }
    }

    return proxima;
}

QRect Trem::areaRegiao(int regiao)
{
    switch (regiao) {
    case RC1:
        return QRect(240, 92, 56, 80);
    case RC2:
        return QRect(480, 92, 56, 80);
    case RC3:
        return QRect(80, 196, 128, 44);
    case RC4:
        return QRect(288, 281, 100, 44);
    case RC5:
        return QRect(388, 281, 100, 44);
    case RC6:
        return QRect(560, 196, 96, 44);
    case RC7:
        return QRect(240, 323, 56, 76);
    case RC8:
        return QRect(490, 323, 40, 37);
    case RC9:
        return QRect(490, 360, 40, 38);
    default:
        return QRect();
    }
}

QString Trem::nomeRegiao(int regiao)
{
    switch (regiao) {
    case RC1:
        return QStringLiteral("RC1");
    case RC2:
        return QStringLiteral("RC2");
    case RC3:
        return QStringLiteral("RC3");
    case RC4:
        return QStringLiteral("RC4");
    case RC5:
        return QStringLiteral("RC5");
    case RC6:
        return QStringLiteral("RC6");
    case RC7:
        return QStringLiteral("RC7");
    case RC8:
        return QStringLiteral("RC8");
    case RC9:
        return QStringLiteral("RC9");
    default:
        return QStringLiteral("RC_NENHUMA");
    }
}

QVector<int> Trem::identificarRegioes(int x, int y) const
{
    QVector<int> regioes;
    for (int regiao = RC1; regiao <= RC9; ++regiao) {
        if (retanguloTrem(x, y).intersects(areaRegiao(regiao))) {
            regioes.append(regiao);
        }
    }

    return regioes;
}

bool Trem::avancarUmPasso()
{
    if (trajeto.size() < 2) {
        return false;
    }

    if (indiceProximoPonto < 0 || indiceProximoPonto >= trajeto.size()) {
        indiceProximoPonto = 0;
    }

    QPoint destino = trajeto.at(indiceProximoPonto);

    if (posicaoAtual == destino) {
        indiceProximoPonto = (indiceProximoPonto + 1) % trajeto.size();
        destino = trajeto.at(indiceProximoPonto);
    }

    bool concluiuTrecho = false;

    if (posicaoAtual.x() < destino.x()) {
        posicaoAtual.setX(posicaoAtual.x() + kPassoMovimento);
        if (posicaoAtual.x() > destino.x()) {
            posicaoAtual.setX(destino.x());
        }
    } else if (posicaoAtual.x() > destino.x()) {
        posicaoAtual.setX(posicaoAtual.x() - kPassoMovimento);
        if (posicaoAtual.x() < destino.x()) {
            posicaoAtual.setX(destino.x());
        }
    } else if (posicaoAtual.y() < destino.y()) {
        posicaoAtual.setY(posicaoAtual.y() + kPassoMovimento);
        if (posicaoAtual.y() > destino.y()) {
            posicaoAtual.setY(destino.y());
        }
    } else if (posicaoAtual.y() > destino.y()) {
        posicaoAtual.setY(posicaoAtual.y() - kPassoMovimento);
        if (posicaoAtual.y() < destino.y()) {
            posicaoAtual.setY(destino.y());
        }
    }

    if (posicaoAtual == destino) {
        concluiuTrecho = true;
        indiceProximoPonto = (indiceProximoPonto + 1) % trajeto.size();
    }

    return concluiuTrecho;
}

void Trem::run()
{
    emit updateGUI(ID, posicaoAtual.x(), posicaoAtual.y());

    auto registrarEntrada = [&](int regiao, const QPoint &coordenadas) {
        qDebug() << "Trem" << ID << "entrou RC" << regiao
                 << nomeRegiao(regiao)
                 << "coordenadas" << coordenadasTexto(coordenadas.x(), coordenadas.y())
                 << "hora" << QDateTime::currentDateTime().toString(Qt::ISODate);
    };

    auto registrarSaida = [&](int regiao, const QPoint &coordenadas) {
        qDebug() << "Trem" << ID << "saiu RC" << regiao
                 << nomeRegiao(regiao)
                 << "coordenadas" << coordenadasTexto(coordenadas.x(), coordenadas.y())
                 << "hora" << QDateTime::currentDateTime().toString(Qt::ISODate);
    };

    auto registrarEspera = [&](int regiao, const QPoint &coordenadas) {
        qDebug() << "Trem" << ID << "esperando RC" << regiao
                 << nomeRegiao(regiao)
                 << "coordenadas" << coordenadasTexto(coordenadas.x(), coordenadas.y())
                 << "hora" << QDateTime::currentDateTime().toString(Qt::ISODate);
    };

    auto diferencaRegioes = [](const QVector<int> &origem, const QVector<int> &alvo) {
        QVector<int> resultado;
        for (int regiao : origem) {
            if (!alvo.contains(regiao)) {
                resultado.append(regiao);
            }
        }
        return resultado;
    };

    auto tentarBloquearRegioes = [&](const QVector<int> &regioes,
                                     const QPoint &coordenadas,
                                     QVector<int> *bloqueadas) -> bool {
        bloqueadas->clear();

        for (int regiao : regioes) {
            if (!rc[regiao - 1].tryLock()) {
                registrarEspera(regiao, coordenadas);
                return false;
            }

            bloqueadas->append(regiao);
            registrarEntrada(regiao, coordenadas);
        }

        return true;
    };

    auto liberarRegioes = [&](const QVector<int> &regioes, const QPoint &coordenadas) {
        for (int indice = regioes.size() - 1; indice >= 0; --indice) {
            const int regiao = regioes.at(indice);
            rc[regiao - 1].unlock();
            registrarSaida(regiao, coordenadas);
        }
    };

    QVector<int> regioesBloqueadas;

    while (!isInterruptionRequested()) {
        const QVector<int> regioesAtuais = identificarRegioes(posicaoAtual.x(), posicaoAtual.y());

        // A posicao atual deve possuir suas regioes antes de qualquer movimento.
        if (regioesBloqueadas.isEmpty() && !regioesAtuais.isEmpty()) {
            QVector<int> adquiridas;
            if (!tentarBloquearRegioes(regioesAtuais, posicaoAtual, &adquiridas)) {
                liberarRegioes(adquiridas, posicaoAtual);
                msleep(static_cast<unsigned long>(atrasoDaIteracao()));
                continue;
            }

            regioesBloqueadas = adquiridas;
        }

        if (velocidade <= 0) {
            msleep(static_cast<unsigned long>(atrasoDaIteracao()));
            continue;
        }

        const QPoint proximaPosicao = preverProximaPosicao();
        const QVector<int> regioesProximas = identificarRegioes(proximaPosicao.x(), proximaPosicao.y());

        // Apenas a proxima fronteira imediata pode ser adquirida nesta iteracao.
        QVector<int> regioesNovas = diferencaRegioes(regioesProximas, regioesBloqueadas);
        QVector<int> regioesASoltar = diferencaRegioes(regioesBloqueadas, regioesProximas);

        QVector<int> regioesAdquiridasNestaIteracao;
        bool podeAvancar = true;

        for (int regiao : regioesNovas) {
            if (!rc[regiao - 1].tryLock()) {
                registrarEspera(regiao, posicaoAtual);
                podeAvancar = false;
                break;
            }

            regioesAdquiridasNestaIteracao.append(regiao);
            registrarEntrada(regiao, posicaoAtual);
        }

        if (!podeAvancar) {
            liberarRegioes(regioesAdquiridasNestaIteracao, posicaoAtual);
            msleep(static_cast<unsigned long>(atrasoDaIteracao()));
            continue;
        }

        avancarUmPasso();
        emit updateGUI(ID, posicaoAtual.x(), posicaoAtual.y());

        // Libera a regiao antiga somente depois que o trem saiu dela.
        liberarRegioes(regioesASoltar, posicaoAtual);

        regioesBloqueadas = regioesProximas;
        msleep(static_cast<unsigned long>(atrasoDaIteracao()));
    }

    // Garante liberacao limpa ao encerrar a thread.
    liberarRegioes(regioesBloqueadas, posicaoAtual);
}
