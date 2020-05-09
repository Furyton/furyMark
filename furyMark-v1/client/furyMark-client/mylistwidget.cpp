#include "mylistwidget.h"
#include "ui_mylistwidget.h"
#include <QDebug>
#include <QtWidgets>
#include <QListWidget>
#include <QPushButton>

#include <generaldata.h>

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
        generalData rec;
        QDataStream stream(&buffer, QIODevice::ReadWrite);
        stream >> rec;

        qDebug() << rec.type;

        if(rec.type == 5) {
            onList(rec);
        } else if(rec.type == 6) {
            onOpen(rec);
        } else if(rec.type == 7) {
            onPdf(rec);
        } else if(rec.type == 8){
            QMessageBox::information(this, "server", "Operation success");
        }else if(rec.type == 9) {
            QMessageBox::information(this, "server", "unknown ERROR from server :(");
        }

    } else {
        QMessageBox::information(this, "server", "receive data error.");
    }
}
void MyListWidget::getListRequest()
{
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    generalData data;

    data.type = 1;

    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::ReadWrite);
    stream << data;

//    QString sendData = "Hello world from getList function";
    int sendRespond = socket->write(buffer);
    if(sendRespond == -1) {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::information(this, "server", "sending data error");
    }
}
void MyListWidget::onList(const generalData& data)
{
    listContent.clear();
    ui->listWidget->clear();

    qDebug() << data.listLength;
    qDebug() << data.fileStampList;

    for(int i = 0; i < data.listLength; i++) {
        ui->listWidget->addItems(data.fileNameList);
        listContent = data.fileStampList;
//        listContent[0];
    }
}
void MyListWidget::on_deleteButton_clicked()
{
    if(ui->listWidget->currentRow() == -1) {
        QMessageBox::information(this, "Operation Error", "You haven't select any file");
        return;
    }

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    generalData data;

    data.type = 4;
    data.timeStamp = listContent[ui->listWidget->currentRow()];
    ui->listWidget->setItemHidden(ui->listWidget->item(ui->listWidget->currentRow()), true);

//    ui->listWidget->items

    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::ReadWrite);
    stream << data;

//    QString sendData = "Hello world from delete function";
//    int sendRespond = socket->write(sendData.toUtf8());

    int sendRespond = socket->write(buffer);
    if(sendRespond == -1) {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::information(this, "server", "sending data error");
    }

    qDebug() << ui->listWidget->currentRow();
}

void MyListWidget::on_openButton_clicked()
{
    if(ui->listWidget->currentRow() == -1) {
        QMessageBox::information(this, "Operation Error", "You haven't select any file");
        return;
    }

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    generalData data;

    data.type = 2;
    data.timeStamp = listContent[ui->listWidget->currentRow()];

    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::ReadWrite);
    stream << data;

    int sendRespond = socket->write(buffer);

    if(sendRespond == -1) {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::information(this, "server", "sending data error");
    }

    qDebug() << ui->listWidget->currentRow();
//    emit openSignal(1, "test.md", "# test ");
}

void MyListWidget::onOpen(const generalData& data){
    emit openSignal(data.timeStamp, data.fileName, data.content);
    this->close();
}

void MyListWidget::on_pdfButton_clicked()
{
    if(ui->listWidget->currentRow() == -1) {
        QMessageBox::information(this, "Operation Error", "You haven't select any file");
        return;
    }

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    generalData data;

    data.type = 3;
    data.timeStamp = listContent[ui->listWidget->currentRow()];

    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::ReadWrite);
    stream << data;

    int sendRespond = socket->write(buffer);

    if(sendRespond == -1) {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::information(this, "server", "sending data error");
    }
    qDebug() << ui->listWidget->currentRow();
}
void MyListWidget::onPdf(const generalData& data)
{
    QString dir = QFileDialog::getExistingDirectory(
                   this, "choose the Directory to save PDF file.",
                   "/");
//    do{
//        dir = QFileDialog::getExistingDirectory(
//                           this, "choose the Directory to save PDF file.",
//                            "/");
//        if(dir.isEmpty()) {
//            QMessageBox::warning(this, "Operation", "Please select a dir");
//        }
//    } while(dir.isEmpty());
//    QString name = data.fileName;
//    name.remove(name.length() - 2, 2);
//    name.append("pdf");

    dir.append("/" + data.fileName);
    qDebug() << dir;

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    QString errorMessage;
    QSaveFile file(dir);
    if(file.open(QFile::WriteOnly)) {
        file.write(data.attachPDF);
        if(!file.commit()) {
            errorMessage = tr("Cannot write file %1: \n %2.")
                           .arg(QDir::toNativeSeparators(dir), file.errorString());
        }
    } else {
        errorMessage = tr("Cannot open file %1 :\n %2.")
                       .arg(QDir::toNativeSeparators(dir), file.errorString());
    }

    QGuiApplication::restoreOverrideCursor();

    if(!errorMessage.isEmpty()) {
        QMessageBox::warning(this, tr("FuryMark"), errorMessage);
    }
    QGuiApplication::restoreOverrideCursor();

//    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

//    QFile file(dir);
//    QString errorMessage;

//    QFile file(QCoreApplication::applicationDirPath() + "/temp.pdf");
//    file.open(QIODevice::WriteOnly);
//    file.write(data.attachPDF);
//    file.close();

//    QGuiApplication::restoreOverrideCursor();
}

void MyListWidget::closeEvent(QCloseEvent *event)
{
    QGuiApplication::restoreOverrideCursor();
    socket->disconnectFromHost();
    event->accept();
}
