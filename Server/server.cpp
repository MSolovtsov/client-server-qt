#include "server.h"

MyServer::MyServer() {
    if(this->listen(
            QHostAddress::Any /*сервер принимает сигналы с любого адреса*/,
            2323 /*прослушивает порт 2323*/
            )){
        qDebug() << "start";

    } else {
        qDebug() << "error";
    }

    this->nextBlockSize = 0;
}

// обработчик новых подключений
void MyServer::incomingConnection(qintptr socketDescriptor){
    this->socket = new QTcpSocket; // задаем новый сокет
    socket->setSocketDescriptor(socketDescriptor); // устанавливаем в него дискриптер из аргументов функции
    /* Дискривтер - простое неотрицательное число, которое индивицирует поток ввода-вывода */

    // объединяем функцию с соответсвенным стотом
    connect(socket, &QTcpSocket::readyRead, this, &MyServer::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    Sockets.push_back(socket);
    qDebug() << "Client connected" << socketDescriptor;
}

// обработчик получееных от клиента сообщений
void MyServer::slotReadyRead(){
    // смотрим тот сокет, на который пришел запрос
    this->socket = (QTcpSocket*)sender();
    // класс для работы с потоковым вводом-выводом информации
    QDataStream in(this->socket); // через in работаем с данными в сокете
    // Указываем версию для исключения ошибки
    in.setVersion(QDataStream::Qt_6_2);

    // проверка состояния
    if(in.status() == QDataStream::Ok){
        qDebug() << "read...";
        for(;;){
            if (this->nextBlockSize == 0) {
                qDebug() << "nextBlockSize = 0";
                if (this->socket->bytesAvailable() < 2){
                    qDebug() << "Data < 2, breake";
                    break;
                }
                in >> this->nextBlockSize;
                qDebug() << "nextBlockSize = " << this->nextBlockSize;
            }
            if (this->socket->bytesAvailable() < this->nextBlockSize){
                qDebug() << "Data not full, break";
                break;
            }

            QString str;
            QTime time;
            in  >> time >> str;
            this->nextBlockSize = 0;
            qDebug() << str;
            // отправляем клиенту
            SendToClirnt(str);
            break;
        }



    } else {
        qDebug() << "DataStrem error";
    }
}

// функция для передачи данных клиенту
void MyServer::SendToClirnt(QString str /*сообщение, которое надо отправить*/){
    this->Data.clear();
    // данные только на вывод  в массив байтов
    QDataStream out(&this->Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    // ввод данных, чтобы не потерять
    out << quint16(0) << QTime::currentTime() << str;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));
    //
    //out << str;
    //socket->write(this->Data);
    for (int i = 0; i < Sockets.size(); i++){
        Sockets[i]->write(Data);
    }

    // Данные отправлены!
}
