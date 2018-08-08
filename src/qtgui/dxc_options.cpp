#include "dxc_options.h"
#include "ui_dxc_options.h"
#include <QDebug>
#include <QTcpSocket>
#include <QString>
#include <QStringList>
#include "dxc_spots.h"

DXC_Options::DXC_Options(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DXC_Options)
{
    ui->setupUi(this);
    TCPSocket = new QTcpSocket(this);
    connect(TCPSocket, SIGNAL(connected()),this, SLOT(connected()));
    connect(TCPSocket, SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(TCPSocket, SIGNAL(readyRead()),this, SLOT(readyToRead()));
}

DXC_Options::~DXC_Options()
{
    delete ui;
}

/*! \brief Catch window close events.
 *
 * This method is called when the user closes the audio options dialog
 * window using the window close icon. We catch the event and hide the
 * dialog but keep it around for later use.
 */
void DXC_Options::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

/*! \brief Catch window show events. */
void DXC_Options::showEvent(QShowEvent * event)
{
    Q_UNUSED(event);
}

void DXC_Options::on_pushButton_DXCConnect_clicked()
{
    TCPSocket->connectToHost("dxc.nc7j.com",7300);
    if(!TCPSocket->waitForConnected(5000))
    {
        qDebug() << "Error: " << TCPSocket->errorString();
    }
    qDebug() << "Button Clicked";
}

void DXC_Options::on_pushButton_DXCDisconnect_clicked()
{
    TCPSocket->close();
}

void DXC_Options::connected()
{
qDebug() << "connected";
}
void DXC_Options::disconnected()
{
qDebug() << "disconnected";
}
void DXC_Options::readyToRead()
{
    DXCSpotInfo info;
    QStringList Spot;
    QString incommingMessage;
    incommingMessage = TCPSocket->readAll();
    qDebug() << incommingMessage.simplified();
    if(incommingMessage.contains("Please enter your callsign:"))
    {
        TCPSocket->write("DH2WQ\r\n");
        qDebug() << "send call";
    }
    else if(incommingMessage.contains("DX de"))
    {
        Spot = incommingMessage.split(" ", QString::SkipEmptyParts);
        info.name = Spot[4].trimmed();
        info.frequency = Spot[3].toDouble() * 1000;
        DXCSpots::Get().add(info);
    }



}
