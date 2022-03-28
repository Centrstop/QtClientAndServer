#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDataStream>
#include <baseobjthread.h>

class TCPClient : public BaseObjThread
{
    Q_OBJECT
public:
    explicit TCPClient(const QHostAddress &ip, const quint16 &port, QObject *parent = nullptr);

signals:
    void connected();
    void disconnected();

public slots:
    void onStartObject() override;
    void onStopObject() override;
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError err);
    void onSendData(const QByteArray &data);

private:
    QTcpSocket * _tcpSocket;
    QHostAddress _ipAddress;
    quint16 _port;

};

#endif // TCPCLIENT_H
