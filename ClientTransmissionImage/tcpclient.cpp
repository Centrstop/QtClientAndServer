#include "tcpclient.h"

TCPClient::TCPClient(const QHostAddress &ip, const quint16 &port, QObject *parent) : BaseObjThread(parent)
{
    _ipAddress = ip;
    _port = port;
}

void TCPClient::onStartObject()
{
    _tcpSocket = new QTcpSocket();
    connect(_tcpSocket, &QTcpSocket::connected, this, &TCPClient::onConnected);
    connect(_tcpSocket, &QTcpSocket::disconnected, this, &TCPClient::onDisconnected);
    connect(_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onError(QAbstractSocket::SocketError)));
    _tcpSocket->connectToHost(_ipAddress, _port);

}

void TCPClient::onStopObject()
{
    _tcpSocket->close();
    _tcpSocket->deleteLater();
    emit finished();
}

void TCPClient::onConnected()
{
    emit connected();
}

void TCPClient::onDisconnected()
{
    emit disconnected();
    emit finished();
}

void TCPClient::onError(QAbstractSocket::SocketError err)
{
    QString strError =
            "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                         "The host was not found." :
                         err == QAbstractSocket::RemoteHostClosedError ?
                         "The remote host is closed." :
                         err == QAbstractSocket::ConnectionRefusedError ?
                         "The connection was refused." :
                         QString(_tcpSocket->errorString())
                        );
    emit ErrorObject(0xFFFF, strError);
}

void TCPClient::onSendData(const QByteArray &data)
{
    QByteArray transmitData;
    QDataStream out(&transmitData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_9);
    out<<int(data.size())<<data;
    _tcpSocket->write(transmitData);
}
