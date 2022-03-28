#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMap>
#include <QByteArray>
#include <QDataStream>
#include "baseobjthread.h"
#include "receivedata.h"

class TCPServer : public BaseObjThread
{
    Q_OBJECT
public:
    explicit TCPServer(const QHostAddress &ipHost, const quint16 &port, QObject *parent = nullptr);
    QHostAddress getHostAddress()const;
    quint16 getHostPort()const;

public slots:
    void onStartObject() override;
    void onStopObject() override;
    void onNewConnection();
    void onReadData();
    void onClientDisconnected();

signals:
    void receiveData(const int &idClient, const QByteArray &data);
    void clientConnected(const int &idClient);
    void clientDisconnect(const int &idClient);

private:
    quint16 _port;
    QHostAddress _host;
    QTcpServer * _tcpServer;
    QMap<int, QTcpSocket *> _clientsMap;
    QMap<int, ReceiveData> _clientsData;

};

#endif // TCPSERVER_H
