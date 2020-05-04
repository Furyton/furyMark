#include <QCoreApplication>

#include <QDebug>

#include <generaldata.h>
#include <sqlhandler.h>

sql::Driver* sqlHandler::driver = nullptr;
sql::Connection* sqlHandler::con = nullptr;
sql::Statement* sqlHandler::stmt = nullptr;
sql::ResultSet* sqlHandler::res = nullptr;
bool sqlHandler::hasInited = false;


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int count, timeStamp;

    sqlHandler::init();
    sqlHandler::getTableInfo(count, timeStamp);

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
