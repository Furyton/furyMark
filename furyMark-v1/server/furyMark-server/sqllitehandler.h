#ifndef SQLLITEHANDLER_H
#define SQLLITEHANDLER_H

#include <QDebug>
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <generaldata.h>

class sqlLiteHandler
{
public:
    sqlLiteHandler(){};

    static QSqlDatabase database;

public:
    static generalData upload(const generalData &info);
    static generalData download_pdf(const generalData &info);
    static generalData download_text(const generalData &info);
    static generalData getList();
    static generalData deleteFile(const generalData &info);

    static bool init();
    static void createTable();

public:
    static bool getTableInfo(int &, int &);
    static bool setTableInfo(int, int);
    static bool changeFile(int stamp, const QString&, const QString&);
    static bool addFile(int stamp, const QString&, const QString&);
    static bool fileExist(int stamp);
    static QString getContent(int stamp);
};

#endif // SQLLITEHANDLER_H
