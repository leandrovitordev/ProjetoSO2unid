#ifndef TREM_H
#define TREM_H

#include <QPoint>
#include <QRect>
#include <QMutex>
#include <QThread>
#include <QString>
#include <QVector>

/*
 * Classe Trem herda QThread.
 * A thread executa run() continuamente enquanto nao for interrompida.
 * A velocidade e controlada pelo slider da interface e convertida em tempo de espera.
 * O acesso as regioes criticas e feito localmente por mutexes.
 */
class Trem : public QThread{
 Q_OBJECT
public:
    enum RegiaoCritica {
        RC_NENHUMA = 0,
        RC1 = 1,
        RC2,
        RC3,
        RC4,
        RC5,
        RC6,
        RC7,
        RC8,
        RC9
    };

    Trem(int id,
         const QPoint &posicaoInicial,
         const QVector<QPoint> &trajeto,
         QObject *parent = 0);
    void run();

public slots:
    void setVelocidade(int valor);

signals:
    void updateGUI(int,int,int);

private:
    bool avancarUmPasso();
    int atrasoDaIteracao() const;
    QPoint preverProximaPosicao() const;
    int identificarRegiao(int x, int y) const;
    static QRect areaRegiao(int regiao);
    static QString nomeRegiao(int regiao);

    static QMutex rc[9];

    int regiaoAtual;
    QPoint posicaoAtual;
    QVector<QPoint> trajeto;
    int indiceProximoPonto;
    int ID;
    int velocidade;
};

#endif // TREM_H
