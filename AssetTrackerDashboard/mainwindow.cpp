#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>
#include <QRandomGenerator>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_secureSocket = nullptr;
    m_client = new QMqttClient(this);

    // Basic Credentials
    m_client->setProtocolVersion(QMqttClient::MQTT_3_1_1);
    m_client->setUsername("vamsi");
    m_client->setPassword("Pointbreak27");

    connect(ui->btnConnect, &QPushButton::clicked, this, &MainWindow::onConnectButtonClicked);
    connect(ui->btnReset, &QPushButton::clicked, this, &MainWindow::on_btnReset_clicked);
    connect(m_client, &QMqttClient::connected, this, &MainWindow::onMQTTConnected);
    connect(m_client, &QMqttClient::messageReceived, this, &MainWindow::onMessageReceived);

    // Graph Setup
    m_timeCounter = 0;
    m_seriesX = new QLineSeries(); m_seriesX->setName("X");
    m_seriesY = new QLineSeries(); m_seriesY->setName("Y");
    m_seriesZ = new QLineSeries(); m_seriesZ->setName("Z");

    m_chart = new QChart();
    m_chart->addSeries(m_seriesX); m_chart->addSeries(m_seriesY); m_chart->addSeries(m_seriesZ);
    m_chart->setTheme(QChart::ChartThemeDark);

    m_axisX = new QValueAxis(); m_axisX->setRange(0, 20);
    m_axisY = new QValueAxis(); m_axisY->setRange(-5, 5);

    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_seriesX->attachAxis(m_axisX); m_seriesY->attachAxis(m_axisX); m_seriesZ->attachAxis(m_axisX);
    m_seriesX->attachAxis(m_axisY); m_seriesY->attachAxis(m_axisY); m_seriesZ->attachAxis(m_axisY);

    QChartView *chartView = new QChartView(m_chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->graphLayout->addWidget(chartView);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::onConnectButtonClicked()
{
    if(m_client->state() == QMqttClient::Disconnected) {
        ui->btnConnect->setText("Connecting...");
        ui->btnConnect->setEnabled(false);

        QString host = "40db01641ba149dc91182576560ea139.s1.eu.hivemq.cloud";
        quint16 port = 8883;

        m_client->setHostname(host);
        m_client->setPort(port);
        // Force a unique ClientID so it doesn't clash with your Pi
        m_client->setClientId("DesktopDash_" + QString::number(QRandomGenerator::global()->bounded(10000)));

        if (m_secureSocket) {
            m_secureSocket->abort();
            m_secureSocket->deleteLater();
        }

        m_secureSocket = new QSslSocket(this);
        m_secureSocket->setProxy(QNetworkProxy::NoProxy);

        // 1. SSL Configuration (TLS 1.2 is mandatory for HiveMQ Cloud)
        QSslConfiguration config = QSslConfiguration::defaultConfiguration();
        config.setProtocol(QSsl::TlsV1_2);
        config.setPeerVerifyMode(QSslSocket::VerifyNone);

        m_secureSocket->setSslConfiguration(config);
        m_secureSocket->setPeerVerifyName(host); // SNI

        // 2. CONNECTION FLOW: TCP -> START ENCRYPTION -> MQTT LOGIN
        connect(m_secureSocket, &QSslSocket::connected, this, [=]() {
            qDebug() << ">>> TCP Connected. Starting SSL Handshake...";
            m_secureSocket->startClientEncryption();
        });

        connect(m_secureSocket, &QSslSocket::encrypted, this, [=]() {
            qDebug() << ">>> SUCCESS: Tunnel Encrypted! Sending MQTT Login...";
            m_client->setTransport(m_secureSocket, QMqttClient::IODevice);
            m_client->connectToHost();
        });

        connect(m_secureSocket, &QSslSocket::errorOccurred, this, [=]() {
            qDebug() << "Socket Error:" << m_secureSocket->errorString();
            ui->btnConnect->setText("Connect");
            ui->btnConnect->setEnabled(true);
        });

        qDebug() << ">>> Opening TCP Socket...";
        m_secureSocket->connectToHost(host, port);

    } else {
        m_client->disconnectFromHost();
        ui->btnConnect->setText("Connect");
    }
}

void MainWindow::onMQTTConnected() {
    qDebug() << "MQTT Login Successful!";
    ui->btnConnect->setText("Disconnect");
    ui->btnConnect->setEnabled(true);
    m_client->subscribe(QMqttTopicFilter("asset_tracker/#"));
}

void MainWindow::onMessageReceived(const QByteArray &message, const QMqttTopicName &topic) {
    QJsonDocument doc = QJsonDocument::fromJson(message);
    if(!doc.isObject()) return;
    QJsonObject obj = doc.object();

    double x = obj["x"].toDouble();
    double y = obj["y"].toDouble();
    double z = obj["z"].toDouble();

    m_seriesX->append(m_timeCounter, x);
    m_seriesY->append(m_timeCounter, y);
    m_seriesZ->append(m_timeCounter, z);
    m_timeCounter++;

    if(m_timeCounter > 20) m_axisX->setRange(m_timeCounter - 20, m_timeCounter);
}

void MainWindow::on_btnReset_clicked() {
    m_seriesX->clear(); m_seriesY->clear(); m_seriesZ->clear();
    m_timeCounter = 0; m_axisX->setRange(0, 20);
}
