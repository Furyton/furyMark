#ifndef GENERALDATA_H
#define GENERALDATA_H

#include <QObject>
#include <QDataStream>
#include <QtCore>
#include <queue>
using namespace std;

class generalData
{
//    Q_OBJECT
public:
    explicit generalData(QObject *parent = nullptr);

public:
    short type;
    QByteArray data;
    int timeStamp;
    QString fileName;
    QString content;
    queue<QString> fileNameList;
    queue<int> fileStampList;
    QByteArray attachPDF;
public:
    friend QDataStream &operator<<(QDataStream &, const generalData &);
    friend QDataStream &operator>>(QDataStream &, generalData &);

};

#endif // GENERALDATA_H
