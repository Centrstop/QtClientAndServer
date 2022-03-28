#include "tcpserver.h"

TCPServer::TCPServer(const QHostAddress &ipHost,const quint16 &port, QObject *parent) : BaseObjThread(parent)
{
    _host = ipHost;
    _port = port;
}

QHostAddress TCPServer::getHostAddress() const
{
    return _host;
}

quint16 TCPServer::getHostPort() const
{
    return _port;
}

void TCPServer::onStartObject()
{
    _tcpServer = new QTcpServer();
    connect(_tcpServer, &QTcpServer::newConnection, this, &TCPServer::onNewConnection);
    if(!_tcpServer->listen(_host, _port)){
        emit ErrorObject(0xFFFF,"Cannot start listen on host");
        emit finished();
        return;
    }
}

void TCPServer::onStopObject()
{
    for(const auto &key: _clientsMap.keys()){
        _clientsMap[key]->close();
        _clientsMap.remove(key);
    }
    _tcpServer->close();
    _tcpServer->deleteLater();
    emit finished();
}

void TCPServer::onNewConnection()
{
    QTcpSocket * clientSocket = _tcpServer->nextPendingConnection();
    int idSocket = clientSocket->socketDescriptor();
    _clientsMap[idSocket] = clientSocket;
    emit clientConnected(idSocket);
    connect(_clientsMap[idSocket], &QTcpSocket::readyRead, this, &TCPServer::onReadData);
    connect(_clientsMap[idSocket], &QTcpSocket::disconnected, this, &TCPServer::onClientDisconnected);
}

void TCPServer::onReadData()
{
    QTcpSocket * clientSocket = (QTcpSocket *)sender();
    int idSocket = clientSocket->socketDescriptor();
    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_5_9);
    if(_clientsData[idSocket].size == 0){
        if(clientSocket->bytesAvailable() < sizeof (int)){
            return;
        }
        in>>_clientsData[idSocket].size;

    }
    if(clientSocket->bytesAvailable() < _clientsData[idSocket].size){
        return;
    }
    in>>_clientsData[idSocket].data;
    emit receiveData(idSocket, _clientsData[idSocket].data);
    _clientsData.remove(idSocket);
}

void TCPServer::onClientDisconnected()
{
    QTcpSocket * clientSocket = (QTcpSocket *)sender();
    int idSocket = clientSocket->socketDescriptor();
    emit clientDisconnect(idSocket);
    if(_clientsMap.contains(idSocket)){
        _clientsMap.remove(idSocket);
    }
    if(_clientsData.contains(idSocket)){
        _clientsData.remove(idSocket);
    }
}
