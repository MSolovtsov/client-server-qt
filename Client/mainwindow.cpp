#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    this->nextBlockSize = 0;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnConnect_clicked()
{
    this->socket->connectToHost("127.0.0.1", 2323);
}


void MainWindow::on_btnSend_clicked()
{
    SendToServer(ui->lineEdit->text());
}

void MainWindow::SendToServer(QString str)
{
    this->Data.clear();
    // данные только на вывод  в массив байтов
    QDataStream out(&this->Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    // Формируем сообщение
    out << quint16(0) << QTime::currentTime() << str;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    //
    socket->write(this->Data);
    ui->lineEdit->clear();
}

void MainWindow::slotReadyRead()
{
    // класс для работы с потоковым вводом-выводом информации
    QDataStream in(this->socket); // через in работаем с данными в сокете
    // Указываем версию для исключения ошибки
    in.setVersion(QDataStream::Qt_6_2);

    // проверка состояния
    if(in.status() == QDataStream::Ok){
       /* QString str;
        in >> str;
        qDebug() << str;
        // отправляем клиенту
        ui->textBrowser->append(str);*/

        for(;;){
           if (this->nextBlockSize == 0) {
                if (this->socket->bytesAvailable() < 2){
                   break;
               }
                in >> this->nextBlockSize;
            }
           if (this->socket->bytesAvailable() < this->nextBlockSize){
                break;
            }

            QString str;
            QTime time;
            in >> time >> str;
            this->nextBlockSize = 0;
            ui->textBrowser->append(time.toString()+ " " + str);
       }

    } else {
        ui->textBrowser->append("Error connection");
    }
}


void MainWindow::on_lineEdit_returnPressed()
{
    SendToServer(ui->lineEdit->text());
}

