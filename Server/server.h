#pragma once

#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QTime>

class MyServer : public QTcpServer
{
    Q_OBJECT
public:
    MyServer();
    QTcpSocket *socket;

private:
    // создает сокеты для каждого нового подключения
    QVector<QTcpSocket*> Sockets;
    // полученные данные
    QByteArray Data;
    // функция для передачи данных клиенту
    void SendToClirnt(QString str /*сообщение, которое надо отправить*/);
    quint16 nextBlockSize;

public slots:
    // обработчик новых подключений
    void incomingConnection(qintptr socketDescriptor);
    // обработчик получееных от клиента сообщений
    void slotReadyRead();
};

#endif // SERVER_H
