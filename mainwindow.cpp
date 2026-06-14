#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>

namespace {
const int kTremLargura = 24;
const int kTremAltura = 24;
const int kMargemRota = 8;
const int kPercursoLargura = 240;
const int kAlturaBase = 170;
const int kAlturaTopoCentro = kAlturaBase + (kAlturaBase / 2);
const int kAlturaInferiorCentro = kAlturaBase / 2;
const int kGradeX = 40;
const int kGradeY = 60;
const int kEspessuraTrilho = 8;
const int kGradeLargura = kPercursoLargura * 3;
const int kGradeAltura = kAlturaBase * 2;

QFrame *criarSegmento(QWidget *parent, const QRect &geometry, const QString &cor)
{
    QFrame *segmento = new QFrame(parent);
    segmento->setGeometry(geometry);
    segmento->setStyleSheet(
        QString("QFrame { background-color: %1; border: 0px; }")
            .arg(cor)
    );
    return segmento;
}

QPoint centroEsquerda(const QRect &celula)
{
    return QPoint(celula.x() + kMargemRota, celula.y() + (celula.height() - kTremAltura) / 2);
}

QPoint centroTopo(const QRect &celula)
{
    return QPoint(celula.x() + (celula.width() - kTremLargura) / 2, celula.y() + kMargemRota);
}

QPoint centroBase(const QRect &celula)
{
    return QPoint(celula.x() + (celula.width() - kTremLargura) / 2,
                  celula.y() + celula.height() - kTremAltura - kMargemRota);
}

QPoint cantoSuperiorEsquerdo(const QRect &celula)
{
    return QPoint(celula.x() + kMargemRota, celula.y() + kMargemRota);
}

QPoint cantoSuperiorDireito(const QRect &celula)
{
    return QPoint(celula.x() + celula.width() - kTremLargura - kMargemRota,
                  celula.y() + kMargemRota);
}

QPoint cantoInferiorDireito(const QRect &celula)
{
    return QPoint(celula.x() + celula.width() - kTremLargura - kMargemRota,
                  celula.y() + celula.height() - kTremAltura - kMargemRota);
}

QPoint cantoInferiorEsquerdo(const QRect &celula)
{
    return QPoint(celula.x() + kMargemRota,
                  celula.y() + celula.height() - kTremAltura - kMargemRota);
}
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    areaMalha(0),
    painelVelocidade(0)
{
    ui->setupUi(this);

    setWindowTitle("Trabalho dos Trens");
    setMinimumSize(1260, 560);
    resize(1260, 560);

    criarInterface();
    criarTrens();
    iniciarTrens();
}

MainWindow::~MainWindow()
{
    for (int id = 1; id <= 6; ++id) {
        if (trens[id]) {
            trens[id]->requestInterruption();
        }
    }

    for (int id = 1; id <= 6; ++id) {
        if (trens[id] && trens[id]->isRunning()) {
            trens[id]->wait();
        }
    }

    delete ui;
}

MainWindow::TremConfig MainWindow::configuracaoPorId(int id) const
{
    TremConfig config;
    config.id = id;

    switch (id) {
    case 1:
        config.nome = "Verde";
        config.cor = "#76a83b";
        config.celula = QRect(kGradeX, kGradeY, kPercursoLargura, kAlturaBase);
        break;
    case 2:
        config.nome = "Vermelho";
        config.cor = "#c94f4f";
        config.celula = QRect(kGradeX + kPercursoLargura, kGradeY, kPercursoLargura, kAlturaTopoCentro);
        break;
    case 3:
        config.nome = "Azul";
        config.cor = "#4a79b8";
        config.celula = QRect(kGradeX + (2 * kPercursoLargura), kGradeY, kPercursoLargura, kAlturaBase);
        break;
    case 4:
        config.nome = "Laranja";
        config.cor = "#df8631";
        config.celula = QRect(kGradeX, kGradeY + kAlturaBase, kPercursoLargura, kAlturaBase);
        break;
    case 5:
        config.nome = "Ciano";
        config.cor = "#4aaec6";
        config.celula = QRect(kGradeX + kPercursoLargura, kGradeY + kAlturaTopoCentro, kPercursoLargura, kAlturaInferiorCentro);
        break;
    case 6:
        config.nome = "Roxo";
        config.cor = "#725291";
        config.celula = QRect(kGradeX + (2 * kPercursoLargura), kGradeY + kAlturaBase, kPercursoLargura, kAlturaBase);
        break;
    default:
        config.nome = "Desconhecido";
        config.cor = "#94a3b8";
        config.celula = QRect(kGradeX, kGradeY, kPercursoLargura, kAlturaBase);
        break;
    }

    config.trajeto = gerarTrajetoHorario(id, config.celula);
    config.posicaoInicial = config.trajeto.isEmpty() ? QPoint() : config.trajeto.first();
    return config;
}

QVector<QPoint> MainWindow::gerarTrajetoHorario(int id, const QRect &celula) const
{
    QVector<QPoint> trajeto;

    switch (id) {
    case 1:
    case 4:
        trajeto << centroEsquerda(celula)
                << cantoSuperiorEsquerdo(celula)
                << cantoSuperiorDireito(celula)
                << cantoInferiorDireito(celula)
                << cantoInferiorEsquerdo(celula);
        break;
    case 2:
    case 3:
        trajeto << centroTopo(celula)
                << cantoSuperiorDireito(celula)
                << cantoInferiorDireito(celula)
                << cantoInferiorEsquerdo(celula)
                << cantoSuperiorEsquerdo(celula);
        break;
    case 5:
    case 6:
        trajeto << centroBase(celula)
                << cantoInferiorDireito(celula)
                << cantoSuperiorDireito(celula)
                << cantoSuperiorEsquerdo(celula)
                << cantoInferiorEsquerdo(celula);
        break;
    default:
        trajeto << cantoSuperiorEsquerdo(celula)
                << cantoSuperiorDireito(celula)
                << cantoInferiorDireito(celula)
                << cantoInferiorEsquerdo(celula);
        break;
    }

    return trajeto;
}

void MainWindow::criarInterface()
{
    ui->centralWidget->setStyleSheet("background-color: #ecf1f7;");

    labelsTrens.resize(7);
    slidersVelocidade.resize(7);
    valoresVelocidade.resize(7);
    trens.resize(7);

    areaMalha = new QFrame(ui->centralWidget);
    areaMalha->setObjectName("areaMalha");
    areaMalha->setGeometry(20, 20, 880, 500);
    areaMalha->setStyleSheet(
        "QFrame#areaMalha {"
        " background-color: #ffffff;"
        " border: 1px solid #cbd5e1;"
        " border-radius: 16px;"
        "}"
    );

    QLabel *tituloMalha = new QLabel("Malha ferroviaria", areaMalha);
    tituloMalha->setGeometry(24, 16, 260, 24);
    tituloMalha->setStyleSheet("color: #0f172a; font-size: 16px; font-weight: bold;");

    QLabel *subtituloMalha = new QLabel("Estrutura interligada com regioes compartilhadas para a etapa de sincronizacao.", areaMalha);
    subtituloMalha->setGeometry(24, 38, 620, 18);
    subtituloMalha->setStyleSheet("color: #475569; font-size: 10px;");

    desenharMalhaConectada();

    for (int id = 1; id <= 6; ++id) {
        TremConfig config = configuracaoPorId(id);

        QLabel *rotuloPercurso = new QLabel(QString("T%1").arg(id), areaMalha);
        rotuloPercurso->setAlignment(Qt::AlignCenter);
        rotuloPercurso->setStyleSheet("color: #0f172a; background: transparent; font-size: 28px; font-weight: bold;");

        switch (id) {
        case 1:
            rotuloPercurso->setGeometry(config.celula.x(), config.celula.y() + 14, config.celula.width(), 44);
            break;
        case 2:
            rotuloPercurso->setGeometry(config.celula.x(), config.celula.y() + 18, config.celula.width(), 44);
            break;
        case 3:
            rotuloPercurso->setGeometry(config.celula.x(), config.celula.y() + 14, config.celula.width(), 44);
            break;
        case 4:
            rotuloPercurso->setGeometry(config.celula.x(), config.celula.y() + 14, config.celula.width(), 44);
            break;
        case 5:
            rotuloPercurso->setGeometry(config.celula.x() + 6, config.celula.y() + 4, config.celula.width() - 12, 38);
            rotuloPercurso->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            break;
        case 6:
            rotuloPercurso->setGeometry(config.celula.x(), config.celula.y() + 14, config.celula.width(), 44);
            break;
        default:
            rotuloPercurso->setGeometry(config.celula.x(), config.celula.y(), config.celula.width(), 44);
            break;
        }
    }

    for (int id = 1; id <= 6; ++id) {
        TremConfig config = configuracaoPorId(id);

        QLabel *trem = new QLabel(areaMalha);
        trem->setObjectName(QString("label_trem%1").arg(id));
        trem->setFixedSize(kTremLargura, kTremAltura);
        trem->setText(QString());
        trem->setStyleSheet(
            QString("QLabel { background-color: %1; border: 1px solid rgba(15, 23, 42, 0.25); border-radius: 4px; }")
                .arg(config.cor)
        );
        trem->move(config.posicaoInicial);
        trem->raise();
        labelsTrens[id] = trem;
    }

    painelVelocidade = new QFrame(ui->centralWidget);
    painelVelocidade->setObjectName("painelVelocidade");
    painelVelocidade->setGeometry(920, 20, 320, 500);
    painelVelocidade->setStyleSheet(
        "QFrame#painelVelocidade {"
        " background-color: #0f172a;"
        " border: 1px solid #334155;"
        " border-radius: 16px;"
        "}"
    );

    QVBoxLayout *layoutPainel = new QVBoxLayout(painelVelocidade);
    layoutPainel->setContentsMargins(16, 16, 16, 16);
    layoutPainel->setSpacing(10);

    QLabel *tituloPainel = new QLabel("Velocidade", painelVelocidade);
    tituloPainel->setStyleSheet("color: #f8fafc; font-size: 16px; font-weight: bold;");
    layoutPainel->addWidget(tituloPainel);

    QLabel *descricaoPainel = new QLabel("0 = parado | 200 = muito rapido", painelVelocidade);
    descricaoPainel->setStyleSheet("color: #94a3b8; font-size: 10px;");
    layoutPainel->addWidget(descricaoPainel);

    for (int id = 1; id <= 6; ++id) {
        TremConfig config = configuracaoPorId(id);

        QFrame *card = new QFrame(painelVelocidade);
        card->setStyleSheet(
            "QFrame {"
            " background-color: rgba(255, 255, 255, 0.04);"
            " border: 1px solid #334155;"
            " border-radius: 10px;"
            "}"
        );

        QVBoxLayout *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(12, 10, 12, 10);
        cardLayout->setSpacing(6);

        QLabel *nomeTrem = new QLabel(QString("Trem %1 - %2").arg(id).arg(config.nome), card);
        nomeTrem->setStyleSheet(QString("color: %1; font-weight: bold;").arg(config.cor));
        cardLayout->addWidget(nomeTrem);

        QHBoxLayout *linhaSlider = new QHBoxLayout();
        linhaSlider->setContentsMargins(0, 0, 0, 0);
        linhaSlider->setSpacing(8);

        QSlider *slider = new QSlider(Qt::Horizontal, card);
        slider->setObjectName(QString("slider_trem%1").arg(id));
        slider->setRange(0, 200);
        slider->setValue(100);
        slider->setStyleSheet(
            QString(
                "QSlider::groove:horizontal { height: 6px; background: #334155; border-radius: 3px; }"
                "QSlider::sub-page:horizontal { background: %1; border-radius: 3px; }"
                "QSlider::handle:horizontal { background: %2; width: 16px; margin: -6px 0; border-radius: 8px; }"
            ).arg(config.cor, config.cor)
        );

        QLabel *valor = new QLabel("100", card);
        valor->setFixedWidth(34);
        valor->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        valor->setStyleSheet("color: #e2e8f0; font-weight: bold;");

        linhaSlider->addWidget(slider, 1);
        linhaSlider->addWidget(valor);
        cardLayout->addLayout(linhaSlider);

        layoutPainel->addWidget(card);

        slidersVelocidade[id] = slider;
        valoresVelocidade[id] = valor;
    }

    QLabel *notaPainel = new QLabel("Os 6 trens iniciam automaticamente ao abrir a aplicacao.", painelVelocidade);
    notaPainel->setWordWrap(true);
    notaPainel->setStyleSheet("color: #cbd5e1; font-size: 10px;");
    layoutPainel->addStretch(1);
    layoutPainel->addWidget(notaPainel);
}

void MainWindow::desenharMalhaConectada()
{
    QFrame *baseMalha = new QFrame(areaMalha);
    baseMalha->setGeometry(kGradeX - 18, kGradeY - 18, kGradeLargura + 36, kGradeAltura + 36);
    baseMalha->setStyleSheet(
        "QFrame {"
        " background-color: #f8fbff;"
        " border: 1px solid #d5e3f3;"
        " border-radius: 12px;"
        "}"
    );
    baseMalha->lower();

    const QString corTrilho = "#5c88c4";
    const QString corJuncoes = "#4e78b3";

    criarSegmento(areaMalha, QRect(kGradeX, kGradeY, kGradeLargura, kEspessuraTrilho), corTrilho);
    criarSegmento(areaMalha, QRect(kGradeX, kGradeY + kGradeAltura - kEspessuraTrilho, kGradeLargura, kEspessuraTrilho), corTrilho);
    criarSegmento(areaMalha, QRect(kGradeX, kGradeY, kEspessuraTrilho, kGradeAltura), corTrilho);
    criarSegmento(areaMalha, QRect(kGradeX + kGradeLargura - kEspessuraTrilho, kGradeY, kEspessuraTrilho, kGradeAltura), corTrilho);

    const int xDivisoria1 = kGradeX + kPercursoLargura - (kEspessuraTrilho / 2);
    const int xDivisoria2 = kGradeX + (2 * kPercursoLargura) - (kEspessuraTrilho / 2);
    const int yDivisoria1 = kGradeY + kAlturaBase - (kEspessuraTrilho / 2);
    const int yDivisoria2 = kGradeY + kAlturaTopoCentro - (kEspessuraTrilho / 2);

    criarSegmento(areaMalha, QRect(xDivisoria1, kGradeY, kEspessuraTrilho, kGradeAltura), corTrilho);
    criarSegmento(areaMalha, QRect(xDivisoria2, kGradeY, kEspessuraTrilho, kGradeAltura), corTrilho);
    criarSegmento(areaMalha, QRect(kGradeX, yDivisoria1, kPercursoLargura, kEspessuraTrilho), corTrilho);
    criarSegmento(areaMalha, QRect(kGradeX + 2 * kPercursoLargura, yDivisoria1, kPercursoLargura, kEspessuraTrilho), corTrilho);
    criarSegmento(areaMalha, QRect(kGradeX + kPercursoLargura, yDivisoria2, kPercursoLargura, kEspessuraTrilho), corTrilho);

    // Pontos fisicos compartilhados da malha. Nesta etapa eles existem apenas na geometria,
    // prontos para receber mutexes/semaforos na proxima fase do trabalho.
    criarSegmento(areaMalha, QRect(xDivisoria1 - 3, yDivisoria1 - 3, 14, 14), corJuncoes);
    criarSegmento(areaMalha, QRect(xDivisoria2 - 3, yDivisoria1 - 3, 14, 14), corJuncoes);
    criarSegmento(areaMalha, QRect(xDivisoria1 - 3, yDivisoria2 - 3, 14, 14), corJuncoes);
    criarSegmento(areaMalha, QRect(xDivisoria2 - 3, yDivisoria2 - 3, 14, 14), corJuncoes);
}

void MainWindow::criarTrens()
{
    for (int id = 1; id <= 6; ++id) {
        TremConfig config = configuracaoPorId(id);

        Trem *trem = new Trem(id, config.posicaoInicial, config.trajeto, this);
        trens[id] = trem;

        connect(trem, SIGNAL(updateGUI(int,int,int)), this, SLOT(updateInterface(int,int,int)));
        connect(slidersVelocidade[id], SIGNAL(valueChanged(int)), valoresVelocidade[id], SLOT(setNum(int)));
        connect(slidersVelocidade[id], SIGNAL(valueChanged(int)), trem, SLOT(setVelocidade(int)));

        trem->setVelocidade(slidersVelocidade[id]->value());
    }
}

void MainWindow::iniciarTrens()
{
    for (int id = 1; id <= 6; ++id) {
        if (trens[id]) {
            trens[id]->start();
        }
    }
}

void MainWindow::updateInterface(int id, int x, int y)
{
    if (id < 1 || id >= labelsTrens.size()) {
        return;
    }

    if (!labelsTrens[id]) {
        return;
    }

    labelsTrens[id]->move(x, y);
}
