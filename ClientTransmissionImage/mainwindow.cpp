#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _tcpClient = nullptr;
    _tcpThread = nullptr;
    uiTransmitSetEnable(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    emit stopAllObject();
    if(_tcpThread != nullptr){
        _tcpThread->quit();
        if(!_tcpThread->wait(1000)){
            qDebug()<<"TCP thread not finished";
        }
    }
    event->accept();
}

void MainWindow::onErrorObject(const int &errorCode, const QString &what)
{
    if(errorCode == 0xFFFF){
        emit stopAllObject();
        uiConnectSetEnable(true);
    }
    showMessage(what);
}

void MainWindow::onConnectedToServer()
{
    uiTransmitSetEnable(true);
}

void MainWindow::onDisconnectedFromServer()
{
    showMessage("Disconnecting from server");
    uiTransmitSetEnable(false);
    uiConnectSetEnable(true);
}


void MainWindow::on_button_connect_clicked()
{
    QString strIP = QString::number(ui->ip_1->value(),10) + "." +
                    QString::number(ui->ip_2->value(),10) + "." +
                    QString::number(ui->ip_3->value(),10) + "." +
                    QString::number(ui->ip_4->value(),10);
    QHostAddress ipAddress(strIP);
    quint16 port = ui->port->value();
    _tcpClient = new TCPClient(ipAddress, port);
    _tcpThread = new QThread();
    connect(this, &MainWindow::sendData, _tcpClient, &TCPClient::onSendData, Qt::QueuedConnection);
    connect(_tcpClient, &TCPClient::connected, this, &MainWindow::onConnectedToServer, Qt::QueuedConnection);
    connect(_tcpClient, &TCPClient::disconnected, this, &MainWindow::onDisconnectedFromServer, Qt::QueuedConnection);
    startObject(_tcpClient, _tcpThread);
    uiConnectSetEnable(false);
}

void MainWindow::on_button_choiceImage_clicked()
{
    _filenameImage = QFileDialog::getOpenFileName(0,"Open Image",_filenameImage, "*.png *.jpg");
    ui->value_dirImage->setText(_filenameImage);
    if(!_filenameImage.isEmpty()){
        _image = QImage(_filenameImage);
        ui->image->setPixmap(QPixmap::fromImage(_image));
    }
}

void MainWindow::on_button_sendImage_clicked()
{
    if(_filenameImage.isEmpty()){
        showMessage("Choose an image");
        return;
    }
    QByteArray imageData;
    QBuffer buffer(&imageData);
    _image.save(&buffer,"png");
    imageData = imageData.toBase64();
    emit sendData(imageData);
}

void MainWindow::showMessage(const QString &message)
{
    QMessageBox msgBox;
    msgBox.setWindowFlags(msgBox.windowFlags()|Qt::WindowStaysOnTopHint);
    QString format("<span style='text-align: center'><p style='font-size: %1pt; color: black'>%2</p></span><p>");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle("Сообщение");
    msgBox.setText(format.arg("12",(message)));
    msgBox.exec();
}

void MainWindow::uiConnectSetEnable(const bool &enable)
{
    ui->button_connect->setEnabled(enable);
    ui->ip_1->setEnabled(enable);
    ui->ip_2->setEnabled(enable);
    ui->ip_3->setEnabled(enable);
    ui->ip_4->setEnabled(enable);
    ui->port->setEnabled(enable);
}

void MainWindow::uiTransmitSetEnable(const bool &enable)
{
    ui->button_sendImage->setEnabled(enable);
}

void MainWindow::startObject(BaseObjThread *object, QThread *thread)
{
    object->moveToThread(thread);
    connect(thread,&QThread::started,object,&BaseObjThread::onStartObject,Qt::QueuedConnection);
    connect(this,&MainWindow::stopAllObject,object,&BaseObjThread::onStopObject,Qt::QueuedConnection);
    connect(object,&BaseObjThread::finished,thread,&QThread::quit,Qt::QueuedConnection);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater, Qt::QueuedConnection);
    connect(object,&BaseObjThread::finished,object,&BaseObjThread::deleteLater,Qt::QueuedConnection);
    connect(object,&BaseObjThread::ErrorObject,this,&MainWindow::onErrorObject,Qt::QueuedConnection);
    thread->start();
}
