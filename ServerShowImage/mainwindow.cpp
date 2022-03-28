#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _tcpThread = nullptr;
    _tcpServer = nullptr;
    findAvailableIP(QNetworkInterface::allAddresses());
    for(const auto &item : _availableIP){
        ui->comboBox_ip->addItem(item.toString());
    }

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
        ui->button_startServer->setEnabled(true);
    }
    showMessage(what);
}

void MainWindow::onReceiveData(const int &idClient, const QByteArray &data)
{
    QByteArray imageData = QByteArray::fromBase64(data);
    QImage image;
    image.loadFromData(imageData);
    int idTab = clientTab[idClient];
    ImageView * show_image = dynamic_cast<ImageView*>(ui->tab_client->widget(idTab));
    show_image->setPixmap(QPixmap::fromImage(image));
}

void MainWindow::onNewClient(const int &idClient)
{
    clientTab[idClient] = createTab(idClient);
}

void MainWindow::onDisconnectClient(const int &idClient)
{
    int tabID = clientTab[idClient];
    ui->tab_client->removeTab(tabID);
    clientTab.remove(idClient);
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

int MainWindow::createTab(const int &id)
{
    ImageView * imageView = new ImageView();
    int idTab = ui->tab_client->addTab(imageView, QString("Client #%1").arg(id));
    return idTab;
}

void MainWindow::findAvailableIP(const QList<QHostAddress> &listIP)
{
    for(const auto &item : listIP){
        if(item.toIPv4Address()!=0){
            _availableIP.append(item);
        }
    }
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


void MainWindow::on_button_startServer_clicked()
{
    QHostAddress host = _availableIP[ui->comboBox_ip->currentIndex()];
    quint16 port = ui->spinBox_port->value();
    _tcpServer = new TCPServer(host, port);
    _tcpThread = new QThread();
    connect(_tcpServer, &TCPServer::receiveData, this, &MainWindow::onReceiveData, Qt::QueuedConnection);
    connect(_tcpServer, &TCPServer::clientConnected, this, &MainWindow::onNewClient, Qt::QueuedConnection);
    connect(_tcpServer, &TCPServer::clientDisconnect, this, &MainWindow::onDisconnectClient, Qt::QueuedConnection);
    startObject(_tcpServer, _tcpThread);
    ui->button_startServer->setEnabled(false);
}
