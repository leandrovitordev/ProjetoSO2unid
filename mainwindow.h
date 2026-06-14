#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>
#include <QRect>
#include <QString>
#include <QVector>
#include "trem.h"

class QFrame;
class QLabel;
class QSlider;
class QWidget;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void updateInterface(int id, int x, int y);

private:
    struct TremConfig {
        int id;
        QString nome;
        QString cor;
        QRect celula;
        QPoint posicaoInicial;
        QVector<QPoint> trajeto;
    };

    void criarInterface();
    void desenharMalhaConectada();
    void criarTrens();
    void iniciarTrens();
    TremConfig configuracaoPorId(int id) const;
    QVector<QPoint> gerarTrajetoHorario(int id, const QRect &celula) const;

    Ui::MainWindow *ui;
    QWidget *areaMalha;
    QWidget *painelVelocidade;
    QVector<Trem*> trens;
    QVector<QLabel*> labelsTrens;
    QVector<QSlider*> slidersVelocidade;
    QVector<QLabel*> valoresVelocidade;
};

#endif // MAINWINDOW_H
