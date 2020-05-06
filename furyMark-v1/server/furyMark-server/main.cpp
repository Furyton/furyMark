#include <QCoreApplication>

#include <QDebug>

#include <generaldata.h>
#include <sqllitehandler.h>

#include <sqlhandler.h>

sql::Driver* sqlHandler::driver = nullptr;
sql::Connection* sqlHandler::con = nullptr;
sql::Statement* sqlHandler::stmt = nullptr;
sql::ResultSet* sqlHandler::res = nullptr;
sql::PreparedStatement* sqlHandler::pstmt = nullptr;
bool sqlHandler::hasInited = false;
QSqlDatabase sqlLiteHandler::database;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    sqlLiteHandler::init();

    generalData tmp;
    tmp.timeStamp = 2;
    sqlLiteHandler::deleteFile(tmp);




//    generalData tmp = sqlLiteHandler::getList();

//    qDebug() << tmp.listLength;
//    queue<int>tmpStampQueue = tmp.fileStampList;
//    queue<QString>tmpNameQueue = tmp.fileNameList;

//    for(int i = 0; i < tmp.listLength; i++) {
//        qDebug() << tmpStampQueue.front() << " : " << tmpNameQueue.front();
//        tmpStampQueue.pop(); tmpNameQueue.pop();
//    }

//    queue<int>tmpStampQueue1 = tmp.fileStampList;
//    queue<QString>tmpNameQueue1 = tmp.fileNameList;

//    for(int i = 0; i < tmp.listLength; i++) {
//        qDebug() << tmpStampQueue1.front() << " : " << tmpNameQueue1.front();
//        tmpStampQueue1.pop(); tmpNameQueue1.pop();
//    }
//    sqlLiteHandler::changeFile(2,"file3","# title");

//    generalData tmp;
//    tmp.type = 2;
//    tmp.timeStamp = 2;

//    generalData ret = sqlLiteHandler::download_pdf(tmp);

//    QFile file(QCoreApplication::applicationDirPath() + "/temp.pdf");
//    file.open(QIODevice::WriteOnly);
//    file.write(ret.attachPDF);
//    file.close();

//    qDebug() << QString("return:\rtimeStamp:%1\rfilename:%2\rcontent:%3").arg(ret.timeStamp).arg(ret.fileName).arg(ret.content);

//    int fileCount, stamp;
//    sqlLiteHandler::getTableInfo(fileCount, stamp);
//    sqlLiteHandler::setTableInfo(1,1);
//    sqlLiteHandler::getTableInfo(fileCount, stamp);

//    sqlLiteHandler::changeFile(1, "file1", "content1");
//    sqlLiteHandler::addFile(2,"file2","content2");
//    sqlLiteHandler::changeFile(2,"file3","content3");

//    qDebug() << sqlLiteHandler::fileExist(2);
//    int count, timeStamp;

//    sqlHandler::init();
//    sqlHandler::getTableInfo(count, timeStamp);

//    w.show();

//    try {
//            sql::Driver *driver;
//            sql::Connection *con;
//            sql::Statement *stmt;
//            sql::ResultSet *res;
//            sql::PreparedStatement *pstmt;

//            /* Create a connection */
//            driver = get_driver_instance();
//            con = driver->connect("127.0.0.1:3306", "root", "password");
//            /* Connect to the MySQL test database */
//            con->setSchema("test");

//            stmt = con->createStatement();
//            stmt->execute("DROP TABLE IF EXISTS test");
//            stmt->execute("CREATE TABLE test(id INT)");
//            delete stmt;

//            /* '?' is the supported placeholder syntax */
//            pstmt = con->prepareStatement("INSERT INTO test(id) VALUES (?)");
//            for (int i = 1; i <= 10; i++) {
//                pstmt->setInt(1, i);
//                pstmt->executeUpdate();
//            }
//            delete pstmt;

//            /* Select in ascending order */
//            pstmt = con->prepareStatement("SELECT id FROM test ORDER BY id ASC");
//            res = pstmt->executeQuery();

//            /* Fetch in reverse = descending order! */
//            res->afterLast();
//            while (res->previous())
//                qDebug() << "\t... MySQL counts: " << res->getInt("id") << endl;
//            delete res;

//            delete pstmt;
//            delete con;

//        }
//        catch (sql::SQLException &e) {
//            qDebug() << "# ERR: SQLException in " << __FILE__;
//            qDebug() << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
//            qDebug() << "# ERR: " << e.what();
//            qDebug() << " (MySQL error code: " << e.getErrorCode();
//            qDebug() << e.getSQLState();
//        }
/*
    generalData data;
    data.type = 0;
    data.timeStamp = 999;
    data.fileName = QString("title");
    data.content = QString("content");

    QByteArray temp;
    QDataStream ds(&temp, QIODevice::ReadWrite);

    ds<<data;

    qDebug() << temp;
    QDataStream ds2(&temp, QIODevice::ReadWrite);

    generalData data2;
    while(!ds2.atEnd()) {
        ds2>>data2;
    }

    qDebug() << data2.type;
    qDebug() << data2.timeStamp;
    qDebug() << data2.fileName;
    qDebug() << data2.content;
*/



    return a.exec();
}
