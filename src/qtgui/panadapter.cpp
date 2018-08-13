#include "panadapter.h"
#include "ui_panadapter.h"
#include <QDebug>

Panadapter::Panadapter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Panadapter)
{
    ui->setupUi(this);
    TCPSocket = new QTcpSocket(this);
    panadapter_refresh_timer = new QTimer(this);
    connect(TCPSocket, SIGNAL(connected()),this, SLOT(connected()));
    connect(TCPSocket, SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(TCPSocket, SIGNAL(readyRead()),this, SLOT(readyToRead()));
    connect(panadapter_refresh_timer, SIGNAL(timeout()), this, SLOT(updateRigFrequency()));
    m_rigFreq = 0;
    m_isConnected = false;
}

Panadapter::~Panadapter()
{
    delete ui;
    panadapter_refresh_timer->stop();
    delete panadapter_refresh_timer;
}

/*! \brief Catch window close events.
 *
 * This method is called when the user closes the audio options dialog
 * window using the window close icon. We catch the event and hide the
 * dialog but keep it around for later use.
 */
void Panadapter::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

/*! \brief Catch window show events. */
void Panadapter::showEvent(QShowEvent * event)
{
    Q_UNUSED(event);
}

void Panadapter::on_pushButton_PanConnect_clicked()
{
    TCPSocket->connectToHost(ui->lineEdit_PanRigctldAddress->text(),ui->lineEdit_PanRigctldPort->text().toInt());
    if(!TCPSocket->waitForConnected(5000))
    {
        ui->plainTextEdit_RigComMonitor->appendPlainText(TCPSocket->errorString());
    }
}

void Panadapter::on_pushButton_PanDisconnect_clicked()
{
    TCPSocket->close();
}

void Panadapter::connected()
{
    m_isConnected = true;
    if(ui->checkBox_EnableComMonitor->isChecked())
    {
        ui->plainTextEdit_RigComMonitor->appendPlainText("Connected");
    }
    ui->pushButton_PanConnect->setDisabled(true);
    ui->pushButton_PanDisconnect->setEnabled(true);
    panadapter_refresh_timer->start(400);
}
void Panadapter::disconnected()
{
    m_isConnected = false;
    if(ui->checkBox_EnableComMonitor->isChecked())
    {
        ui->plainTextEdit_RigComMonitor->appendPlainText("Disonnected");
        ui->plainTextEdit_RigComMonitor->appendPlainText(TCPSocket->errorString());
    }
    ui->pushButton_PanDisconnect->setDisabled(true);
    ui->pushButton_PanConnect->setEnabled(true);
    panadapter_refresh_timer->stop();
}
void Panadapter::readyToRead()
{
    QString incommingMessage;
    incommingMessage = TCPSocket->readAll();
    if(ui->checkBox_EnableComMonitor->isChecked())
    {
        ui->plainTextEdit_RigComMonitor->appendPlainText(incommingMessage);
    }
    m_rigFreq = incommingMessage.toDouble()/1000000;
}

void Panadapter::on_lineEdit_RigCommand_returnPressed()
{
    ui->lineEdit_RigCommand->setFocus();
    TCPSocket->write(ui->lineEdit_RigCommand->text().toUtf8());
    if(ui->checkBox_EnableComMonitor->isChecked())
    {
        ui->plainTextEdit_RigComMonitor->appendPlainText(ui->lineEdit_RigCommand->text());
    }
    ui->lineEdit_RigCommand->clear();
}

void Panadapter::updateRigFrequency()
{
    if(m_isConnected)
    {
        if(ui->checkBox_CtrlGqrx->isChecked())
        {
            TCPSocket->write("f\r\n");
            ui->plainTextEdit_RigComMonitor->appendPlainText("f");
            emit newFrequency((qint64)(m_rigFreq * 1000000));
            emit newLoFrequency(m_rigFreq - ui->lineEdit_PanIfFrequency_3->text().toDouble());
        }
        else if(ui->checkBox_CtrlRig->isChecked())
        {
            TCPSocket->write("F\r\n");
            ui->plainTextEdit_RigComMonitor->appendPlainText("F");
            emit newFrequency((qint64)(m_rigFreq * 1000000));
            emit newLoFrequency(m_rigFreq - ui->lineEdit_PanIfFrequency_3->text().toDouble());
        }

    }
}

void Panadapter::on_radioButton_UseRigctld_clicked()
{
    ui->groupBox_Rigctld->setEnabled(true);
    ui->groupBox_SerialConnection->setDisabled(true);
    ui->radioButton_UseRigctld->setChecked(true);
    ui->radioButton_UseComPort->setChecked(false);
}

void Panadapter::on_radioButton_UseComPort_clicked()
{
    ui->groupBox_Rigctld->setDisabled(true);
    ui->groupBox_SerialConnection->setEnabled(true);
    ui->radioButton_UseRigctld->setChecked(false);
    ui->radioButton_UseComPort->setChecked(true);
}

void Panadapter::on_checkBox_EnableComMonitor_clicked()
{
    ui->groupBox_ComMonitor->setEnabled(ui->checkBox_EnableComMonitor->isChecked());
}
