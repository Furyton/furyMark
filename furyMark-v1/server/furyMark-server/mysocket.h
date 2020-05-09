#ifndef MYSOCKET_H
#define MYSOCKET_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <generaldata.h>
#include <sqllitehandler.h>

class MySocket:public QObject
{
    Q_OBJECT
public:
    MySocket();
    ~MySocket();
    void init();
private slots:
    void ServerReadData();
    void ServerNewConnection();
    void ServerDisconnection();
    void ServerSendData(const generalData&);

private:
    QTcpServer *server;
    QTcpSocket *socket;

};

#endif // MYSOCKET_H
