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

#include <dataPackage/generaldata.h>

class sqlHandler
{
    public:
        sqlHandler(const sql::SQLString &host, const sql::SQLString &user_name, const sql::SQLString &password);
        sqlHandler();
        ~sqlHandler();

        /*
            报文格式:
                客户端：
                    3bit: 000->upload, 001->getList, 010->download_text, 011->download_pdf, 100->delete

                    upload: 32bit->timeStamp, 32bit->filenameLength, 64bit->contentLength, filename, content
                    getList: null
                    download_text: 32bit->timeStamp
                    download_pdf: 32bit->timeStamp
                    delete: 32bit->timeStamp
                服务器：
                    3bit: 000->list, 001->text, 010->pdf, 011->upload/delete success, 100->upload/delete failed

                    list: 32bit->length, length * 32 timeStamp, name

        */

        generalData upload(const generalData &info);
        generalData download_pdf(const generalData &info);
        generalData download_text(const generalData &info);
        generalData getList(const generalData &info);
        generalData deleteFile(const generalData &info);
    private:
        sql::Driver *driver;
        sql::Connection *con;
        sql::Statement *stmt;
        sql::ResultSet *res;
        sql::PreparedStatement *pstmt;
};

#endif // SQLHANDLER_H
