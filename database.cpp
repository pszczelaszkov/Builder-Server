#include "database.h"

Database::Database()
{
	tools = new Toolbox();
	conn = mysql_init(NULL);
        if (conn == NULL)
        {
                std::cout << "Error " << mysql_errno(conn) << " " << mysql_error(conn) << std::endl;
                exit(1);
        }
}

void Database::connect(std::string host,std::string usr,std::string passwd,std::string db)
{
        std::cout << "Connecting with MYSQL...\n";

        if (mysql_real_connect(conn, host.c_str(), usr.c_str(), passwd.c_str(), db.c_str(), 3306, NULL, 0) == NULL)
        {
                std::cout << "Error " << mysql_errno(conn) << " " << mysql_error(conn) << std::endl;
                exit(1);
        }
}

void Database::setQuery(std::string qr)
{
	query = "";
        query = qr;
}

void Database::sendQuery()
{
	mysql_real_query(conn,query.c_str(),query.size());
}

void Database::storeResult()
{
	result = mysql_store_result(conn);
}

bool Database::fetchRow()
{
	if(!result)
		return false;

	if(row = mysql_fetch_row(result))
	{
		rowPosition = -1;
		return true;
	}
	else
		return false;
}

void Database::getData(std::stringstream *stream)
{
	rowPosition++;
	if(row[rowPosition])
		*stream << row[rowPosition] << "\n";
	else
		*stream << "UNKNOWN \n";
}

std::string Database::getString(int32_t pos)
{
	int32_t dataSize = mysql_fetch_lengths(result)[pos];
	char* temp = new char[dataSize];
	temp = {};
	temp = row[pos];
	std::string result;

	for(int32_t i = 0;i < dataSize;i++)
	{
		result.insert(result.end(),temp[i]);
	}

	return result;
}

int32_t Database::getInt(int32_t pos)
{
	return tools->stringToInt(row[pos]);
}

int32_t Database::getFieldSize(int32_t pos)
{
	return mysql_fetch_lengths(result)[pos];
}

void Database::disconnect()
{
	mysql_close(conn);
}

void Database::freeResult()
{
	mysql_free_result(result);
}

my_ulonglong Database::numRows()
{
	return mysql_num_rows(result);
}

int32_t Database::resultsState()
{
	return mysql_next_result(conn);
}

std::string Database::getQuery()
{
	return query;
}

std::string Database::escapeBlob(const char *source, unsigned long length)
{
        if(!source)
                return "''";

        char* output = new char[length * 2 + 1];
        mysql_real_escape_string(conn, output, source, length);

        std::string result = "'";
        result += output;
        result += "'";

        delete[] output;
        return result;
}

void Database::ping()
{
        mysql_ping(conn);
}
