#include "panadapter.h"
#include "ui_panadapter.h"
#include <hamlib/riglist.h>
#include <hamlib/rig.h>
#include <hamlib/rigclass.h>
#include <QDebug>

Panadapter::Panadapter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Panadapter)
{
    ui->setupUi(this);
    TCPSocket = new QTcpSocket(this);
    connect(TCPSocket, SIGNAL(connected()),this, SLOT(connected()));
    connect(TCPSocket, SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(TCPSocket, SIGNAL(readyRead()),this, SLOT(readyToRead()));
    m_rigFreq = 0;
}

Panadapter::~Panadapter()
{
    delete ui;
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
    ui->plainTextEdit_RigComMonitor->appendPlainText("Connected");
    ui->pushButton_PanConnect->setDisabled(true);
    ui->pushButton_PanDisconnect->setEnabled(true);
}
void Panadapter::disconnected()
{
    ui->plainTextEdit_RigComMonitor->appendPlainText("Disconnected");
    ui->plainTextEdit_RigComMonitor->appendPlainText(TCPSocket->errorString());
    ui->pushButton_PanDisconnect->setDisabled(true);
    ui->pushButton_PanConnect->setEnabled(true);
}
void Panadapter::readyToRead()
{
    QString incommingMessage;
    incommingMessage = TCPSocket->readAll();
    ui->plainTextEdit_RigComMonitor->appendPlainText(incommingMessage);
    m_rigFreq = incommingMessage.toDouble()/1000000;
}

void Panadapter::on_lineEdit_RigCommand_returnPressed()
{
    ui->lineEdit_RigCommand->setFocus();
    TCPSocket->write(ui->lineEdit_RigCommand->text().toUtf8());
    ui->plainTextEdit_RigComMonitor->appendPlainText(ui->lineEdit_RigCommand->text());
    ui->lineEdit_RigCommand->clear();
}

void Panadapter::updateRigFrequency()
{
    if(TCPSocket->isOpen())
    {
        qDebug() << "f: m_rigFreq";
        TCPSocket->write("f\r\n");
        ui->plainTextEdit_RigComMonitor->appendPlainText("f");
        emit newLnbLo(m_rigFreq - ui->lineEdit_PanIfFrequency_3->text().toDouble());
    }
}
