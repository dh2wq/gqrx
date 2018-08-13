#ifndef PANADAPTER_H
#define PANADAPTER_H


#include <QCloseEvent>
#include <QShowEvent>
#include <QSettings>
#include <QDialog>
#include <QTcpSocket>
#include <QTimer>

namespace Ui {
class Panadapter;
}

class Panadapter : public QDialog
{
    Q_OBJECT

public:
    explicit Panadapter(QWidget *parent = 0);
    ~Panadapter();

    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent * event);
    void saveSettings(QSettings *settings);
    void readSettings(QSettings *settings);

private slots:

    void on_pushButton_PanConnect_clicked();
    void on_pushButton_PanDisconnect_clicked();
    void connected();
    void disconnected();
    void readyToRead();
    void updateRigFrequency();

    void on_lineEdit_RigCommand_returnPressed();

    void on_radioButton_UseRigctld_clicked();

    void on_radioButton_UseComPort_clicked();

    void on_checkBox_EnableComMonitor_clicked();

signals:
    void newLoFrequency(double freq_mhz);
    void newFrequency(qint64 freq_mhz);

private:
    Ui::Panadapter *ui;
    QTcpSocket *TCPSocket;
    double m_rigFreq;
    bool m_isConnected;
    QTimer   *panadapter_refresh_timer;
};

#endif // PANADAPTER_H
