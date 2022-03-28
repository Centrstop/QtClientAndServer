#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QPixmap>
#include <QImage>
#include <QThread>
#include <QCloseEvent>
#include <QBuffer>
#include <QMessageBox>
#include "tcpclient.h"

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
    void onConnectedToServer();
    void onDisconnectedFromServer();

signals:
    void stopAllObject();
    void sendData(const QByteArray &data);

private slots:
    void on_button_connect_clicked();

    void on_button_choiceImage_clicked();

    void on_button_sendImage_clicked();

private:
    void startObject(BaseObjThread *object, QThread *thread);
    void showMessage(const QString &message);
    void uiConnectSetEnable(const bool &enable);
    void uiTransmitSetEnable(const bool &enable);
    Ui::MainWindow *ui;
    TCPClient * _tcpClient;
    QThread *_tcpThread;
    QImage _image;
    QString _filenameImage;
};
#endif // MAINWINDOW_H
