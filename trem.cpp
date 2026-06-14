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
      regiaoAtual(RC_NENHUMA),
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
        return QRect(80, 194, 128, 44);
    case RC4:
        return QRect(288, 279, 100, 44);
    case RC5:
        return QRect(388, 279, 100, 44);
    case RC6:
        return QRect(560, 194, 96, 44);
    case RC7:
        return QRect(240, 323, 56, 76);
    case RC8:
        return QRect(488, 323, 40, 37);
    case RC9:
        return QRect(488, 360, 40, 38);
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

int Trem::identificarRegiao(int x, int y) const
{
    for (int regiao = RC1; regiao <= RC9; ++regiao) {
        if (retanguloTrem(x, y).intersects(areaRegiao(regiao))) {
            return regiao;
        }
    }

    return RC_NENHUMA;
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

    const int regiaoInicial = identificarRegiao(posicaoAtual.x(), posicaoAtual.y());
    if (regiaoInicial != RC_NENHUMA) {
        qDebug() << "Trem" << ID << "esperando RC" << regiaoInicial
                 << nomeRegiao(regiaoInicial)
                 << "coordenadas" << coordenadasTexto(posicaoAtual.x(), posicaoAtual.y())
                 << "hora" << QDateTime::currentDateTime().toString(Qt::ISODate);

        rc[regiaoInicial - 1].lock();
        regiaoAtual = regiaoInicial;

        qDebug() << "Trem" << ID << "entrou RC" << regiaoAtual
                 << nomeRegiao(regiaoAtual)
                 << "coordenadas" << coordenadasTexto(posicaoAtual.x(), posicaoAtual.y())
                 << "hora" << QDateTime::currentDateTime().toString(Qt::ISODate);
    }

    while (!isInterruptionRequested()) {
        if (velocidade <= 0) {
            msleep(static_cast<unsigned long>(atrasoDaIteracao()));
            continue;
        }

        const QPoint proximaPosicao = preverProximaPosicao();
        const int regiaoProxima = identificarRegiao(proximaPosicao.x(), proximaPosicao.y());

        if (regiaoAtual == regiaoProxima) {
            avancarUmPasso();
        } else if (regiaoAtual == RC_NENHUMA && regiaoProxima != RC_NENHUMA) {
            qDebug() << "Trem" << ID << "esperando RC" << regiaoProxima
                     << nomeRegiao(regiaoProxima)
                     << "coordenadas" << coordenadasTexto(posicaoAtual.x(), posicaoAtual.y())
                     << "hora" << QDateTime::currentDateTime().toString(Qt::ISODate);

            rc[regiaoProxima - 1].lock();
            regiaoAtual = regiaoProxima;

            qDebug() << "Trem" << ID << "entrou RC" << regiaoAtual
                     << nomeRegiao(regiaoAtual)
                     << "coordenadas" << coordenadasTexto(posicaoAtual.x(), posicaoAtual.y())
                     << "hora" << QDateTime::currentDateTime().toString(Qt::ISODate);

            avancarUmPasso();
        } else if (regiaoAtual != RC_NENHUMA && regiaoProxima != RC_NENHUMA && regiaoAtual != regiaoProxima) {
            qDebug() << "Trem" << ID << "esperando RC" << regiaoProxima
                     << nomeRegiao(regiaoProxima)
                     << "coordenadas" << coordenadasTexto(posicaoAtual.x(), posicaoAtual.y())
                     << "hora" << QDateTime::currentDateTime().toString(Qt::ISODate);

            rc[regiaoProxima - 1].lock();

            qDebug() << "Trem" << ID << "entrou RC" << regiaoProxima
                     << nomeRegiao(regiaoProxima)
                     << "coordenadas" << coordenadasTexto(proximaPosicao.x(), proximaPosicao.y())
                     << "hora" << QDateTime::currentDateTime().toString(Qt::ISODate);

            avancarUmPasso();

            const int regiaoAnterior = regiaoAtual;
            rc[regiaoAnterior - 1].unlock();
            qDebug() << "Trem" << ID << "saiu RC" << regiaoAnterior
                     << nomeRegiao(regiaoAnterior)
                     << "coordenadas" << coordenadasTexto(posicaoAtual.x(), posicaoAtual.y())
                     << "hora" << QDateTime::currentDateTime().toString(Qt::ISODate);
            regiaoAtual = regiaoProxima;
        } else if (regiaoAtual != RC_NENHUMA && regiaoProxima == RC_NENHUMA) {
            avancarUmPasso();

            const int regiaoAnterior = regiaoAtual;
            rc[regiaoAnterior - 1].unlock();
            qDebug() << "Trem" << ID << "saiu RC" << regiaoAnterior
                     << nomeRegiao(regiaoAnterior)
                     << "coordenadas" << coordenadasTexto(posicaoAtual.x(), posicaoAtual.y())
                     << "hora" << QDateTime::currentDateTime().toString(Qt::ISODate);
            regiaoAtual = RC_NENHUMA;
        } else {
            avancarUmPasso();
        }

        emit updateGUI(ID, posicaoAtual.x(), posicaoAtual.y());
        msleep(static_cast<unsigned long>(atrasoDaIteracao()));
    }

    if (regiaoAtual != RC_NENHUMA) {
        rc[regiaoAtual - 1].unlock();
        qDebug() << "Trem" << ID << "saiu RC" << regiaoAtual
                 << nomeRegiao(regiaoAtual)
                 << "coordenadas" << coordenadasTexto(posicaoAtual.x(), posicaoAtual.y())
                 << "hora" << QDateTime::currentDateTime().toString(Qt::ISODate);
        regiaoAtual = RC_NENHUMA;
    }
}
