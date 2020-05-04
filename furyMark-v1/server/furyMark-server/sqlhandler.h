#ifndef SQLHANDLER_H
#define SQLHANDLER_H

#include <QDebug>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>

#include <generaldata.h>

class sqlHandler
{
    public:
        static sql::Driver *driver;
        static sql::Connection *con;
        static sql::Statement *stmt;
        static sql::ResultSet *res;
        static sql::PreparedStatement *pstmt;
        static bool hasInited;

        sqlHandler(){};
        ~sqlHandler(){};

        /*
            报文格式:
                客户端：
                    3bit: 0->upload, 1->getList, 2->download_text, 3->download_pdf, 4->delete

                    upload: 32bit->timeStamp, 32bit->filenameLength, 64bit->contentLength, filename, content
                    getList: null
                    download_text: 32bit->timeStamp
                    download_pdf: 32bit->timeStamp
                    delete: 32bit->timeStamp
                服务器：
                    3bit: 5->list, 6->text, 7->pdf, 8->upload/delete success, 10->upload/delete failed

                    list: 32bit->length, length * 32 timeStamp, name

        */

        static generalData upload(const generalData &info);
        static generalData download_pdf(const generalData &info);
        static generalData download_text(const generalData &info);
        static generalData getList(const generalData &info);
        static generalData deleteFile(const generalData &info);

        static void init();
   public:

        static void getTableInfo(int &, int &);
        static void setTableInfo(int, int);
        static void changeFile(int stamp, const QString&, const QString&);
        static void addFile(int stamp, const QString&, const QString&);

};

#endif // SQLHANDLER_H
