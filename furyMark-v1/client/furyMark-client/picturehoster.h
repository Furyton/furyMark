#ifndef PICTUREHOSTER_H
#define PICTUREHOSTER_H

#include <QObject>
#include <QFileDialog>
#include <QHttpMultiPart>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkReply>


class PictureHoster:public QObject
{
    Q_OBJECT
public:
    PictureHoster();
private:
    QNetworkAccessManager *networkAccessManager;

public slots:
    void sendPost();
    void sendPost(const QString& filePath);
    void deal(QNetworkReply*);
signals:
    void urlSignal(const QString&);
//    QString getUrl();
};

#endif // PICTUREHOSTER_H
