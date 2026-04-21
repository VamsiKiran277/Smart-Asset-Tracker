#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMqttClient> //MQTT library
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
//added this section
private slots:
    //custom function that will run when connect button is clicked
    void onConnectButtonClicked();
    //when mqtt is connected
    void onMQTTConnected();
    void onMessageReceived(const QByteArray &message, const QMqttTopicName &topic); //for the messsage and the topic
    void on_btnReset_clicked();


private:
    Ui::MainWindow *ui;
    //mqtt engine pointer
    QMqttClient *m_client;
    //graph variables
    QLineSeries *m_seriesX;
    QLineSeries *m_seriesY;
    QLineSeries *m_seriesZ;
    QChart *m_chart;
    QValueAxis *m_axisX;
    QValueAxis *m_axisY;
    int m_timeCounter; // To push the graph forward
};
#endif // MAINWINDOW_H
