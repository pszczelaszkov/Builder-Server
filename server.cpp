#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "tcpserver.h"
#include <mysql/mysql.h>

using boost::asio::ip::tcp;

void TCPthread(int32_t port,Database *db)
{
        std::cout << "Starting Server on Port: " << port << "...\n";
        try
        {
                boost::asio::io_service io_service;
                tcp_server server(io_service,port,db);
                io_service.run();
        }
        catch (std::exception& e)
        {
                std::cerr << e.what() << std::endl;
        }

}

void keepAlive(Database *db,int32_t delay)
{
	while(true)
	{
		db->ping();
		sleep(delay);
		std::cout<<"Keeping Alive\n";
	}
}

int32_t main()
{
	int32_t port;
	std::string host,user,password,database;
	std::fstream config;

        config.open("config.ini",std::fstream::in);

        config >> port;
	config >> host;
	config >> user;
	config >> password;
	config >> database;

        Database *db;
        db = new Database();

        //boost::thread thread(TCPthread,port,db);

        db->connect(host, user, password, database);
	boost::thread thread(keepAlive,db,1);

        std::cout << "Starting Server on Port: " << port << "...\n";
        try
        {
                boost::asio::io_service io_service;
                tcp_server server(io_service,port,db);
                io_service.run();
        }
        catch (std::exception& e)
        {
                std::cerr << e.what() << std::endl;
        }

	std::cin >> port;
  	return 0;
}
