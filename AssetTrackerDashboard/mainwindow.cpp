#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QDebug" //allows us to print in the output console
#include <QSslConfiguration> //required for HiveMQ security
#include <QJsonDocument>
#include <QJsonObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //Building the MQTT engine
    m_client = new QMqttClient(this);
    m_client->setProtocolVersion(QMqttClient::MQTT_3_1_1);
    m_client->setUsername("vamsi");
    m_client->setPassword("Pointbreak27");
    //HiveMq uses ssl so we tell qt to use it
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    // Sender       = ui->btnConnect
    // Signal       = &QPushButton::clicked
    // Receiver     = this (the MainWindow itself)
    // Slot         = &MainWindow::onConnectButtonClicked
    connect(ui->btnConnect, &QPushButton::clicked,this,&MainWindow::onConnectButtonClicked);
    //wiring the mqtt engine to built in connect button to signal the custom plot
    connect(m_client,&QMqttClient::connected,this,&MainWindow::onMQTTConnected);
    //wirre for the custom plot
    connect(m_client, &QMqttClient::messageReceived, this, &MainWindow::onMessageReceived);
    connect(m_client, &QMqttClient::errorChanged, this, [](QMqttClient::ClientError error){
        qDebug() << "MQTT Error Occurred:" << error;
    });
    connect(ui->btnReset,&QPushButton::clicked,this,&MainWindow::on_btnReset_clicked);

    //GRAPHS
    m_timeCounter = 0;
    //Create the lines
    m_seriesX = new QLineSeries(); m_seriesX->setName("X-Axis");
    m_seriesY = new QLineSeries(); m_seriesY->setName("Y-Axis");
    m_seriesZ = new QLineSeries(); m_seriesZ->setName("Z-Axis");
    //Create the chart and add the lines
    m_chart = new QChart();
    m_chart->addSeries(m_seriesX);
    m_chart->addSeries(m_seriesY);
    m_chart->addSeries(m_seriesZ);
    m_chart->setTitle("Live Impact Telemetry");
    m_chart->setTheme(QChart::ChartThemeDark);
    //Create the X Axis (Time)
    m_axisX = new QValueAxis();
    m_axisX->setRange(0, 20); // Show 100 ticks of history
    m_axisX->setTitleText("Time");
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_seriesX->attachAxis(m_axisX);
    m_seriesY->attachAxis(m_axisX);
    m_seriesZ->attachAxis(m_axisX);
    //Create the Y Axis (G-Force)
    m_axisY = new QValueAxis();
    m_axisY->setRange(-5, 5); // G-forces between -5g and 5g
    m_axisY->setTitleText("G-Force");
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_seriesX->attachAxis(m_axisY);
    m_seriesY->attachAxis(m_axisY);
    m_seriesZ->attachAxis(m_axisY);
    //Put the chart into the UI layout box!
    QChartView *chartView = new QChartView(m_chart);
    chartView->setRenderHint(QPainter::Antialiasing); // Smooths the lines
    ui->graphLayout->addWidget(chartView);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onConnectButtonClicked() {
    if(m_client->state() == QMqttClient::Disconnected) {
        ui->btnConnect->setText("Connecting");
        m_client->setHostname("40db01641ba149dc91182576560ea139.s1.eu.hivemq.cloud");
        m_client->setPort(8883);
        //command that reaches out to internet so we are using the envrypted ver
        m_client->connectToHostEncrypted(QSslConfiguration::defaultConfiguration());
    } else {
    //if already connected it disconnects us from there upon clicking the button
        m_client->disconnectFromHost();
        ui->btnConnect->setText("Connect");
        //Reset the graph
        // m_seriesX->clear();
        // m_seriesY->clear();
        // m_seriesZ->clear();
        // m_timeCounter = 0; // Reset time back to the start
        // m_axisX->setRange(0, 20); // putting the window back to the beginning
    }
}

void MainWindow::onMQTTConnected() {
    qDebug() << "Success: Connected to Hive MQ";
    //asking HiveMQ to send us anything thats published to this topic
    auto subscription = m_client->subscribe(QMqttTopicFilter("asset_tracker/#"));
    if(!subscription) {
        qDebug() << "Error could not subscribe to the topic";
    } else {
        qDebug() << "Successfully connected to the topic";
    }
    ui->btnConnect->setText("Disconnect");
}

void MainWindow::onMessageReceived(const QByteArray &message, const QMqttTopicName &topic) {
    qDebug() << "Message Received, Topic:" << topic.name() << "Payload:" << message;
    //connect the raw text into JSON Object
    QJsonDocument jsonDoc = QJsonDocument::fromJson(message);
    if(!jsonDoc.isNull() && jsonDoc.isObject()) {
        QJsonObject jsonObj = jsonDoc.object();
        if(jsonObj.contains("x") && jsonObj.contains("y") && jsonObj.contains("z")) {
            double x = jsonObj["x"].toDouble();
            double y = jsonObj["y"].toDouble();
            double z = jsonObj["z"].toDouble();

            ui->labelX->setText(QString("X: %1").arg(x));
            ui->labelY->setText(QString("Y: %1").arg(y));
            ui->labelZ->setText(QString("Z: %1").arg(z));
            // --- UPDATE THE GRAPH ---
            m_seriesX->append(m_timeCounter, x);
            m_seriesY->append(m_timeCounter, y);
            m_seriesZ->append(m_timeCounter, z);
            m_chart->update();

            m_timeCounter++;

            // If the line hits the edge of the screen, scroll the X-axis forward
            if (m_timeCounter > 20) {
                m_axisX->setRange(m_timeCounter - 20, m_timeCounter);
            }
        }
    }
}

void MainWindow::on_btnReset_clicked() {
    m_seriesX->clear();
    m_seriesY->clear();
    m_seriesZ->clear();
}
