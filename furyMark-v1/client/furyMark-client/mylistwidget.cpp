#include "mylistwidget.h"
#include "ui_mylistwidget.h"
#include <QDebug>
#include <QtWidgets>
#include <QListWidget>
#include <QPushButton>

MyListWidget::MyListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyListWidget)
{
    ui->setupUi(this);
    setWindowTitle(tr("Operate your cloud files"));
}

MyListWidget::~MyListWidget()
{
    delete ui;
}

void MyListWidget::init()
{
    qDebug() << "initialising.";
    socket = new QTcpSocket(this);

    QString ip = "127.0.0.1";
    int port = 8806;
    socket->connectToHost(ip, port);

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    if(!socket->waitForConnected(10000)) {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::information(this, "Server", "connecting to server failed.");
        this->close();
        return;
    }
    QGuiApplication::restoreOverrideCursor();

    getListRequest();

    connect(socket, SIGNAL(readyRead()), this, SLOT(respondDealer()));
}

void MyListWidget::respondDealer()
{
    QGuiApplication::restoreOverrideCursor();

    QByteArray buffer;
    buffer = socket->readAll();
    if(buffer.length() > 0) {
        qDebug() << QString(buffer);
    } else {
        QMessageBox::information(this, "server", "receive data error.");
    }
}
void MyListWidget::getListRequest()
{
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    QString sendData = "Hello world from getList function";
    int sendRespond = socket->write(sendData.toUtf8());
    if(sendRespond == -1) {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::information(this, "server", "sending data error");
    }
}
void MyListWidget::on_deleteButton_clicked()
{
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    QString sendData = "Hello world from delete function";
    int sendRespond = socket->write(sendData.toUtf8());
    if(sendRespond == -1) {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::information(this, "server", "sending data error");
    }

    qDebug() << ui->listWidget->currentRow();
}

void MyListWidget::on_openButton_clicked()
{
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    QString sendData = "Hello world from open function";
    int sendRespond = socket->write(sendData.toUtf8());
    if(sendRespond == -1) {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::information(this, "server", "sending data error");
    }

    qDebug() << ui->listWidget->currentRow();
//    emit openSignal(1, "test.md", "# test ");
}

void MyListWidget::on_pdfButton_clicked()
{
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    QString sendData = "Hello world from pdf function";
    int sendRespond = socket->write(sendData.toUtf8());
    if(sendRespond == -1) {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::information(this, "server", "sending data error");
    }

    qDebug() << ui->listWidget->currentRow();
}


void MyListWidget::closeEvent(QCloseEvent *event)
{
    QGuiApplication::restoreOverrideCursor();
    socket->disconnectFromHost();
    event->accept();
}
