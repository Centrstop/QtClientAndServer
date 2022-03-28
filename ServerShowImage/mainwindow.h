#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QScrollArea>
#include <QLabel>
#include <QThread>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QMessageBox>
#include "tcpserver.h"
#include "imageview.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);
public slots:
    void onErrorObject(const int &errorCode, const QString &what);
    void onReceiveData(const int &idClient, const QByteArray &data);
    void onNewClient(const int &idClient);
    void onDisconnectClient(const int &idClient);
signals:
    void stopAllObject();

private slots:
    void on_button_startServer_clicked();

private:
    void startObject(BaseObjThread *object, QThread *thread);
    int createTab(const int &id);
    void findAvailableIP(const QList<QHostAddress> &listIP);
    void showMessage(const QString &message);
    Ui::MainWindow *ui;
    QMap<int, int> clientTab;
    TCPServer * _tcpServer;
    QThread *_tcpThread;
    QList<QHostAddress> _availableIP;

};
#endif // MAINWINDOW_H
