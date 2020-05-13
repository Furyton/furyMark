#include "picturehoster.h"
#include <QGuiApplication>
#include <QMessageBox>

PictureHoster::PictureHoster()
{
    networkAccessManager = new QNetworkAccessManager(this);

    connect(networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(deal(QNetworkReply*)));

}

void PictureHoster::sendPost()
{
//    qDebug() << "Call sendPost() function in class PictureHoster";
//    return;

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);

    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));

    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant("form-data; name=\"smfile\"; filename=\"image.png\""));

    QString strFilePath = QFileDialog::getOpenFileName(nullptr, QString::fromLocal8Bit("Select a picture"), "./",
                                                     tr("Image files(*.bmp *.jpg *.pbm *.pgm *.png *.ppm *.xbm *.xpm *.jpeg);;All files (*.*)"));

    QFile *file = new QFile(strFilePath, this);
    if(!file->open(QIODevice::ReadOnly)){
        QMessageBox::warning(nullptr, tr("ERROR"), tr("Can not open this picture  :("));
        return;
    }else{
        imagePart.setBodyDevice(file);
        multiPart->append(imagePart);
    }

    QNetworkRequest request;
    request.setUrl(QUrl("https://sm.ms/api/v2/upload"));
    if(networkAccessManager->post(request,multiPart)->error() != QNetworkReply::NoError){
        QGuiApplication::restoreOverrideCursor();
        return;
    }
}
void PictureHoster::sendPost(const QString& filePath)
{
//    qDebug() << "Call sendPost() function in class PictureHoster";
//    return;

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);

    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));

    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant("form-data; name=\"smfile\"; filename=\"image.png\""));

    QFile *file = new QFile(filePath, this);
    if(!file->open(QIODevice::ReadOnly)){
        QMessageBox::warning(nullptr, tr("ERROR"), tr("Can not open this picture  :("));
        return;
    }else{
        imagePart.setBodyDevice(file);
        multiPart->append(imagePart);
    }

    QNetworkRequest request;
    request.setUrl(QUrl("https://sm.ms/api/v2/upload"));
    if(networkAccessManager->post(request,multiPart)->error() != QNetworkReply::NoError){
        QGuiApplication::restoreOverrideCursor();
        return;
    }
}
void PictureHoster::deal(QNetworkReply* reply)
{

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    qDebug() << "statusCode:" << statusCode;

    QGuiApplication::restoreOverrideCursor();

    if(reply->error() == QNetworkReply::NoError)
    {
       QByteArray allData = reply->readAll();

       qDebug() << QString(allData);

       QJsonParseError json_error;
       QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &json_error));
       if(json_error.error != QJsonParseError::NoError)
       {
           QMessageBox::warning(nullptr, tr("ERROR"), tr("Can not analysize this json data  :("));
           return;
       }
        QJsonObject rootObj = jsonDoc.object();

        if(rootObj.contains("data"))
        {
           QJsonObject subObj = rootObj.value("data").toObject();
           qDebug() << subObj["filename"].toString();
           qDebug() << subObj["url"].toString();
           emit urlSignal("![" + subObj["filename"].toString() + "](" + subObj["url"].toString() + ")");
        } else if(rootObj.contains("images")) {
//            qDebug() << "========================================";
            qDebug() << rootObj["images"].toString();
            emit urlSignal("![existImage](" + rootObj["images"].toString() + ")");
        }
    }
    else
    {
        QMessageBox::warning(nullptr, tr("ERROR"), tr("Can not analysize this json data  :("));
    }

    reply->deleteLater();
}
