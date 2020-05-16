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
//        QString testString(buffer);
//        qDebug() << testString;

//        ServerSendData("Hi, I received!");
        generalData rec;
        QDataStream stream(&buffer, QIODevice::ReadWrite);
        stream >> rec;

        qDebug() << "\noperate type: " << rec.type <<endl;

        if(rec.type == 0) {
            ServerSendData(sqlLiteHandler::upload(rec));
        } else if(rec.type == 1) {
            ServerSendData(sqlLiteHandler::getList());
        } else if(rec.type == 2) {
            ServerSendData(sqlLiteHandler::download_text(rec));
        } else if(rec.type == 3) {
            ServerSendData(sqlLiteHandler::download_pdf(rec));
        } else if(rec.type == 4) {
            ServerSendData(sqlLiteHandler::deleteFile(rec));
        } else {
            qDebug() << "unknown data type ERROR";
            generalData ret;
            ret.type = 9;
            ServerSendData(ret);
        }

    } else {
        qDebug() << "receive data ERROR";
    }
    return;
}
void MySocket::ServerSendData(const generalData& data)
{
//    if(data.isEmpty()) {
//        qDebug() << "empty data ERROR";
//        return;
//    }
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::ReadWrite);
    stream << data;

    if(buffer.isEmpty()) {
        qDebug() << "empty data ERROR";
        return;
    }

    if(socket->isValid()) {
        int sendResponse = socket->write(buffer);
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
