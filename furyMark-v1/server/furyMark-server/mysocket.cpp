#include "mysocket.h"

MySocket::MySocket()
{
    server =  new QTcpServer(this);
}
MySocket::~MySocket()
{

}
void MySocket::init()
{
    int port = 8806;
    if(!server->listen(QHostAddress::Any, port)) {
        qDebug() << "server listen FAILED";
        return;
    } else {
        qDebug() << "server listen SUCCESS!!!";
    }
    connect(server, SIGNAL(newConnection()), this, SLOT(ServerNewConnection()));
}
void MySocket::ServerNewConnection()
{
    socket = server->nextPendingConnection();
    if(!socket) {
        qDebug() << "get connection from client ERROR";
        return;
    } else {
        qDebug() << "get connection SUCCESS!!";
        connect(socket, SIGNAL(readyRead()), this, SLOT(ServerReadData()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(ServerDisconnection()));
    }
}
void MySocket::ServerReadData()
{
    QByteArray buffer;
//    buffer.push_back(socket->read(1024));
    buffer = socket->readAll();

    if(buffer.length() > 0) {
        QString testString(buffer);
        qDebug() << testString;

        ServerSendData("Hi, I received!");
    } else {
        qDebug() << "receive data ERROR";
    }
    return;
}
void MySocket::ServerSendData(const QString& data)
{
    if(data.isEmpty()) {
        qDebug() << "empty data ERROR";
        return;
    }
    if(socket->isValid()) {
        int sendResponse = socket->write(data.toUtf8());
        if(sendResponse == -1) {
            qDebug() << "socket writng FAILED";
        } else {
            qDebug() << "socket writing SUCCESS!!!";
        }
    } else {
        qDebug() << "socket invalid";
    }
}
void MySocket::ServerDisconnection()
{
    qDebug() << "server disconnected";
}
