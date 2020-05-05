#include "sqllitehandler.h"

/*
 * init()
 *
 * connect to local database, named filedata.db with connection called "my_sqlite"
 *
 * this file is in /debug dir
 *
 * if the file doesn't exist then create this database, and create tables named: file, info
 *
 * return if there has an error
*/

bool sqlLiteHandler::init()
{
//    QSqlDatabase database;

    if(QSqlDatabase::contains("my_sqlite")) {
        database = QSqlDatabase::database("my_sqlite");

        qDebug() << "already connected";
    } else {
        database = QSqlDatabase::addDatabase("QSQLITE","my_sqlite");
        database.setDatabaseName("filedata.db");
    }

    if(!database.open()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), database.lastError().text());
        return false;
    }

    if(!database.tables().contains("file")) {
        qDebug() << "creating tables";
        createTable();
    }
    return true;
}

/*
 * createTable()
 *
 * create tables: file, info
*/
void sqlLiteHandler::createTable()
{
    QSqlQuery sql_query(database);
    QString create_sql = "create table file (id int primary key, filename varchar(255), content BLOB)";
    sql_query.prepare(create_sql);

    if(!sql_query.exec()) {
        qDebug() << "Error: fail to create table." << sql_query.lastError();
    } else {
        qDebug() << "table file created.";
    }
    create_sql = "create table info (curFileCount int primary key, timeStamp int)";
    sql_query.prepare(create_sql);

    if(!sql_query.exec()) {
        qDebug() << "Error: fail to create table." << sql_query.lastError();
    } else {
        qDebug() << "table info created.";
    }
}

/*
 * getTableInfo()
 *
 * get curFileCount and timeStamp from info
*/

bool sqlLiteHandler::getTableInfo(int &curFileCount, int &timeStamp)
{
    QSqlQuery sql_query(database);
    curFileCount = -1;
    timeStamp = -1;

    QString select_all_sql = "select * from info";
    sql_query.prepare(select_all_sql);
    if(!sql_query.exec()) {
        qDebug() << sql_query.lastError();
        return false;
    } else {
        while(sql_query.next()) {
            curFileCount = sql_query.value(0).toInt();
            timeStamp = sql_query.value(1).toInt();

            qDebug() << QString("curFileCount: %1, timeStamp: %2").arg(curFileCount).arg(timeStamp);
        }
        return true;
    }
}

/*
 * setTableInfo()
 *
 * set curFileCount and timeStamp in info
*/

bool sqlLiteHandler::setTableInfo(int curFileCount, int timeStamp)
{
    QString clear_infoTable = "delete from info";
    QSqlQuery sql_query(database);

    sql_query.prepare(clear_infoTable);

    if(!sql_query.exec()) {
        qDebug() << sql_query.lastError();
        return false;
    } else {
        qDebug() << "info table cleared";
    }

    QString insert = QString("INSERT INTO info VALUES(%1, %2)").arg(curFileCount).arg(timeStamp);

    sql_query.prepare(insert);

    if(!sql_query.exec()) {
        qDebug() << sql_query.lastError();
        return false;
    } else {
        qDebug() << QString("inserted curFileCount:%1, timeStamp:%2").arg(curFileCount).arg(timeStamp);
    }
    return true;

}

/*
 * changeFile()
 *
 * change the file's name and content whose id is 'stamp'
 *
*/

bool sqlLiteHandler::changeFile(int stamp, const QString &fileName, const QString &content)
{
    if(!fileExist(stamp)) return false;

    QByteArray barry;
    barry = content.toUtf8();

    QString update_sql = "update file set filename = :filename, content = :content where id = :id";
    QSqlQuery sql_query(database);
    sql_query.prepare(update_sql);
    sql_query.bindValue(":filename", fileName);
    sql_query.bindValue(":content", barry);
    sql_query.bindValue(":id", stamp);

    if(!sql_query.exec()) {
        qDebug() << sql_query.lastError();
        return false;
    } else {
        qDebug() << QString("update file id:%1, filename:%2").arg(stamp).arg(fileName);
        return true;
    }
}

/*
 * addFile()
 *
 * insert a new file with id 'stamp'
*/

bool sqlLiteHandler::addFile(int stamp, const QString &fileName, const QString &content)
{
    if(fileExist(stamp)) return false;

    QByteArray barry;
    barry = content.toUtf8();

    QString insert = "INSERT INTO file VALUES(?, ?, ?)";
    QSqlQuery sql_query(database);
    sql_query.prepare(insert);

    sql_query.addBindValue(stamp);
    sql_query.addBindValue(fileName);
    sql_query.addBindValue(barry);

    if(!sql_query.exec()) {
        qDebug() << sql_query.lastError();
        return false;
    } else {
        qDebug() << QString("added a new file, id:%1, filename:%2").arg(stamp).arg(fileName);
    }

    return true;
}
bool sqlLiteHandler::fileExist(int stamp)
{
    QString select_sql = "select * from file where id=:id";
    QSqlQuery sql_query(database);
    sql_query.prepare(select_sql);

    sql_query.bindValue(":id",stamp);

    if(!sql_query.exec()) {
        qDebug() << sql_query.lastError();
        return false;
    } else {
        while(sql_query.next()) {
            if(stamp == sql_query.value(0).toInt()) return true;
        }
        return false;
    }
}
QString sqlLiteHandler::getContent(int stamp)
{
    if(!fileExist(stamp)) return QString();

    QString select_sql = "select * from file where id=:id";
    QSqlQuery sql_query(database);
    sql_query.prepare(select_sql);

    sql_query.bindValue(":id", stamp);

    if(!sql_query.exec()) {
        qDebug() << sql_query.lastError();
        return QString();
    } else {
        while(sql_query.next()) {
            if(stamp == sql_query.value(0).toInt()) {
                QByteArray barry = sql_query.value(2).toByteArray();
                return QString(barry);
            }
        }
    }

    return QString();
}
//=========================================================
generalData sqlLiteHandler::upload(const generalData &info)
{
    generalData ret;

    if(fileExist(info.timeStamp)) {
        if(changeFile(info.timeStamp, info.fileName, info.content)) {
            ret.type = 8;
        } else {
            ret.type = 9;
        }
    } else {
        int curCount, stamp;

        if(getTableInfo(curCount, stamp)) {
            curCount++;
            stamp++;

            setTableInfo(curCount, stamp);

            if(addFile(stamp, info.fileName, info.content)) {
                ret.type = 8;
            } else {
                ret.type = 9;
            }
        } else {
            ret.type = 9;
        }
    }

    return ret;
}
generalData sqlLiteHandler::download_pdf(const generalData &info)
{
    generalData ret;

    ret = download_text(info);
    if(ret.type != 9) {
        ret.type = 7;

        QFile file(QCoreApplication::applicationDirPath() + "/temporary.md");
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        file.write(ret.content.toUtf8());
        file.close();

        qDebug() << QCoreApplication::applicationDirPath() + "/temporary.md";

        QProcess::execute("markdown-pdf " + QCoreApplication::applicationDirPath() + "/temporary.md");

        QFile pdf(QCoreApplication::applicationDirPath() + "/temporary.pdf");
        pdf.open(QIODevice::ReadOnly);
        ret.attachPDF = pdf.readAll();
        pdf.close();

        pdf.remove();
        file.remove();
    }

    return ret;
}
generalData sqlLiteHandler::download_text(const generalData &info)
{
    generalData ret;

    ret.timeStamp = info.timeStamp;

//    if()
    if(!fileExist(info.timeStamp)) {
        ret.type = 9;
    } else {
        QString select_sql = "select * from file where id=:id";
        QSqlQuery sql_query(database);
        sql_query.prepare(select_sql);

        sql_query.bindValue(":id", info.timeStamp);

        if(!sql_query.exec()) {
            qDebug() << sql_query.lastError();
            ret.type = 9;
        } else {
            ret.type = 9;
            while(sql_query.next()) {
                if(info.timeStamp == sql_query.value(0).toInt()) {
                    ret.type = 6;
                    QByteArray barry = sql_query.value(2).toByteArray();
                    ret.content = QString(barry);
                    ret.fileName = sql_query.value(1).toString();
                }
            }
        }
    }

    return ret;
}
generalData sqlLiteHandler::getList(const generalData &info)
{
    generalData ret;



    return ret;
}
generalData sqlLiteHandler::deleteFile(const generalData &info)
{
    generalData ret;

    return ret;
}
