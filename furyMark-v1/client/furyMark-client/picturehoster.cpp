#include "picturehoster.h"
#include <QGuiApplication>

PictureHoster::PictureHoster()
{
    networkAccessManager = new QNetworkAccessManager(this);

    //网络载体的响应接收信号，与响应接收槽绑定
    connect(networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(deal(QNetworkReply*)));

}

void PictureHoster::sendPost()
{
//    qDebug() << "Call sendPost() function in class PictureHoster";
//    return;

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));
  //如果上传的是jpg图片，就修改为image/jpg
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant("form-data; name=\"smfile\"; filename=\"image.png\""));
   //这里的smfile是参数名，不可以修改，因为SM.MS的API接口要求参数名必须为smfile
   //下面这行代码的作用是打开选择窗口，选择一个图片并返回路径
  QString strFilePath = QFileDialog::getOpenFileName(nullptr, QString::fromLocal8Bit("Select a picture"), "./",
                                                     tr("Image files(*.bmp *.jpg *.pbm *.pgm *.png *.ppm *.xbm *.xpm *.jpeg);;All files (*.*)"));

    QFile *file = new QFile(strFilePath);
    if(!file->open(QIODevice::ReadOnly))
            qDebug()<<"=================================";
    else{
         imagePart.setBodyDevice(file);
         multiPart->append(imagePart);
    }

    QNetworkRequest request;
    request.setUrl(QUrl("https://sm.ms/api/v2/upload"));
    networkAccessManager->post(request,multiPart);
}
void PictureHoster::deal(QNetworkReply* reply)
{
    QGuiApplication::restoreOverrideCursor();
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    qDebug() << "statusCode:" << statusCode;

    if(reply->error() == QNetworkReply::NoError)
    {
        //官方文档显示返回的reply是json格式，所以我们采用json格式读取
       QByteArray allData = reply->readAll();

       qDebug() << QString(allData);

       QJsonParseError json_error;
       QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &json_error));
       if(json_error.error != QJsonParseError::NoError)
       {
           qDebug() << "json error!";
           return;
       }
        QJsonObject rootObj = jsonDoc.object();
      //我们只需要上传后的url，所以值提取了url，如果你需要其他内容，请提取方法类似
        if(rootObj.contains("data"))
        {
           QJsonObject subObj = rootObj.value("data").toObject();
           qDebug() << subObj["filename"].toString();
           qDebug() << subObj["url"].toString();
           emit urlSignal("![" + subObj["filename"].toString() + "](" + subObj["url"].toString() + ")");
        } else if(rootObj.contains("images")) {
            qDebug() << "========================================";
            qDebug() << rootObj["images"].toString();
            emit urlSignal("![existImage](" + rootObj["images"].toString() + ")");
        }
    }
    else
    {
            qDebug() << "=========";
    }

    reply->deleteLater();
}
