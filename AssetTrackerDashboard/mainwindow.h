#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMqttClient>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QSslConfiguration>
#include <QSslSocket>
#include <QNetworkProxy>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onConnectButtonClicked();
    void onMQTTConnected();
    void onMessageReceived(const QByteArray &message, const QMqttTopicName &topic);
    void on_btnReset_clicked();

private:
    Ui::MainWindow *ui;
    QMqttClient *m_client;
    QSslSocket *m_secureSocket;

    QLineSeries *m_seriesX;
    QLineSeries *m_seriesY;
    QLineSeries *m_seriesZ;
    QChart *m_chart;
    QValueAxis *m_axisX;
    QValueAxis *m_axisY;
    int m_timeCounter;
};
#endif // MAINWINDOW_H
