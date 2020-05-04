#include "sqlhandler.h"

void sqlHandler::init()
{
    if(sqlHandler::hasInited) {
        return;
    }

    driver = get_driver_instance();

    con = driver->connect("tcp://127.0.0.1:3306", "root", "password");

    con->setSchema("filedata");

    hasInited = true;

    cout << "MySQL initialized...."<<endl;

    return;
}
generalData sqlHandler::upload(const generalData &info)
{
    qDebug() << "uploading...";
    int timeStamp = info.timeStamp;
    QString name = info.fileName;
    QString content = info.content;

    generalData ret;

    if(timeStamp == -1) {

    }

    return ret;
}
generalData sqlHandler::getList(const generalData &info)
{
    generalData ret;

    return ret;
}
generalData sqlHandler::deleteFile(const generalData &info)
{
    generalData ret;

    return ret;
}
generalData sqlHandler::download_pdf(const generalData &info)
{
    generalData ret;

    return ret;
}
generalData sqlHandler::download_text(const generalData &info)
{
    generalData ret;

    return ret;
}
void sqlHandler::getTableInfo(int &fileCount, int &timeStamp)
{
    fileCount = 0; timeStamp = 0;

    string sql_cmd = "select * from tableInfo";

    init();

    stmt = con->createStatement();
    pstmt = con->prepareStatement("INSERT INTO test VALUES (?)");

//    pstmt->setBlob()

    try {
        res = stmt->executeQuery(sql_cmd);
        while(res->next()) {
            fileCount = res->getInt("fileCount");
            timeStamp = res->getInt("timeStamp");
        }

        delete res;
        delete stmt;

    } catch (sql::SQLException &e) {
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line "
            << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
        fileCount = timeStamp = -1;
        return;
    }
    cout <<"get fileCount and timeStamp from tableInfo" <<endl;
    cout << fileCount << " ," << timeStamp <<endl;
    return;
}

void sqlHandler::setTableInfo(int, int)
{

}
void sqlHandler::addFile(int stamp, const QString &fileName, const QString &content)
{

}
void sqlHandler::changeFile(int stamp, const QString &fileName, const QString &content)
{

}
