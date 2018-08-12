#ifndef PANADAPTER_H
#define PANADAPTER_H


#include <QCloseEvent>
#include <QShowEvent>
#include <QSettings>
#include <QDialog>
#include <QTcpSocket>
#include <hamlib/rig.h>
#include <hamlib/riglist.h>

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

signals:
    void newLnbLo(double freq_mhz);

private:
    Ui::Panadapter *ui;
    QTcpSocket *TCPSocket;
    double m_rigFreq;
};

#endif // PANADAPTER_H
