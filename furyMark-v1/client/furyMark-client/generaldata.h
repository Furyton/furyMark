#ifndef GENERALDATA_H
#define GENERALDATA_H

#include <QObject>
#include <QDataStream>
#include <QtCore>
//#include <queue>
#include <QList>
using namespace std;

class generalData
{
//    Q_OBJECT
public:
    generalData(){};
    ~generalData(){};

public:
    short type;
    int timeStamp;
    QString fileName;
    QString content;
    int listLength;
    QList<QString> fileNameList;
    QList<int> fileStampList;
    QByteArray attachPDF;

    void initProperty(int type);
    void initProperty(int type, int timeStamp);
    void initProperty(int type, int timeStamp, const QString &fileName, const QString &content);

    void initProperty(int type, int listLenght, const QList<int> &fileStampList, const QList<QString> &fileNameList);
    void initProperty(int type, const QString &fileName, const QByteArray &pdf);
public:
    friend QDataStream &operator <<(QDataStream &, generalData const &);
    friend QDataStream &operator >>(QDataStream &, generalData &);

};

#endif // GENERALDATA_H
