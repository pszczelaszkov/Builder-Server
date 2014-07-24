#ifndef __DATABASE__
#define __DATABASE__

#include <mysql/mysql.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <stdint.h>
#include "toolbox.h"

class Database
{
	public:

		Database();
		void connect(std::string host,std::string usr,std::string passwd,std::string db);
		void disconnect();
		void setQuery(std::string qr);
		void sendQuery();
		void storeResult();
                void getData(std::stringstream *buffer);
		void ping();

		std::string getString(int32_t pos);
		std::string escapeBlob(const char *from, unsigned long length);
                std::string getQuery();

		int32_t getInt(int32_t pos);
		int32_t getFieldSize(int32_t pos);
                int32_t resultsState();

		void freeResult();

		bool isResult();
                bool fetchRow();

		my_ulonglong numRows();
	private:

        	MYSQL *conn;
        	MYSQL_RES *result;
        	MYSQL_ROW row;
		Toolbox *tools;

        	int32_t rowPosition;
		std::string query;
};
#endif
