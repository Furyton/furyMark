#include <QCoreApplication>

#include <QDebug>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

//    w.show();

    try {
            sql::Driver *driver;
            sql::Connection *con;
            sql::Statement *stmt;
            sql::ResultSet *res;
            sql::PreparedStatement *pstmt;

            /* Create a connection */
            driver = get_driver_instance();
            con = driver->connect("127.0.0.1:3306", "root", "password");
            /* Connect to the MySQL test database */
            con->setSchema("test");

            stmt = con->createStatement();
            stmt->execute("DROP TABLE IF EXISTS test");
            stmt->execute("CREATE TABLE test(id INT)");
            delete stmt;

            /* '?' is the supported placeholder syntax */
            pstmt = con->prepareStatement("INSERT INTO test(id) VALUES (?)");
            for (int i = 1; i <= 10; i++) {
                pstmt->setInt(1, i);
                pstmt->executeUpdate();
            }
            delete pstmt;

            /* Select in ascending order */
            pstmt = con->prepareStatement("SELECT id FROM test ORDER BY id ASC");
            res = pstmt->executeQuery();

            /* Fetch in reverse = descending order! */
            res->afterLast();
            while (res->previous())
                qDebug() << "\t... MySQL counts: " << res->getInt("id") << endl;
            delete res;

            delete pstmt;
            delete con;

        }
        catch (sql::SQLException &e) {
            qDebug() << "# ERR: SQLException in " << __FILE__;
            qDebug() << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
            qDebug() << "# ERR: " << e.what();
            qDebug() << " (MySQL error code: " << e.getErrorCode();
//            qDebug() << e.getSQLState();
        }

    return a.exec();
}
