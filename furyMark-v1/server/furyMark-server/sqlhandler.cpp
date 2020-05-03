#include "sqlhandler.h"

sqlHandler::sqlHandler(const sql::SQLString &host, const sql::SQLString &user_name, const sql::SQLString &password)
{
    driver = get_driver_instance();

    con = driver->connect(host, user_name, password);

    con->setSchema("fileData");
}
sqlHandler::~sqlHandler()
{
    delete con;
}
generalData sqlHandler::upload(const generalData & info)
{
    generalData data;

    return data;
}
