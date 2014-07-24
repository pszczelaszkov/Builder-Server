#ifndef __TCPSERVER__
#define __TCPSERVER__

#include <ctime>
#include <iostream>
#include <string>
#include <sstream>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "database.h"
#include "toolbox.h"
#include "session.h"

using boost::asio::ip::tcp;

class Session;
class tcp_connection : public boost::enable_shared_from_this<tcp_connection>
{
	public:

		static int conNum;
		const int conNum_;

		typedef boost::shared_ptr<tcp_connection> pointer;

		tcp_connection();
		static pointer create(boost::asio::io_service& io_service);

		tcp::socket& socket();

        	void setMessage(std::string message);
        	void setMessage(char message);
                void appendMessage(std::string message);
                void appendMessage(char message);
		void flush();
		void start_read();
		//std::string * getMessage(){return msg&;}

        	Database *baza;
                Session *session;

		std::string msg;
                char cmd;

	private:

		tcp_connection(boost::asio::io_service& io_service);

		void handle_write(const boost::system::error_code& /*error*/,size_t /*bytes_transferred*/);
		void handle_read(const boost::system::error_code& /*error*/,size_t /*bytes_transferred*/);
		void separateMessage(std::istream *is);

		void parse();

		tcp::socket socket_;
		std::ostringstream ostream;

		boost::asio::streambuf inbuf;
		std::string bufs[2];
		std::string* frontbuf;
		std::string* backbuf;

		bool writePending;
		bool writeRequested;

};

class tcp_server
{
	public:

		tcp_server(boost::asio::io_service& io_service,int32_t port,Database *db);

	private:

  		void start_accept();
		void handle_accept(tcp_connection::pointer new_connection,const boost::system::error_code& error);
		tcp::acceptor acceptor_;

 		Database *baza;
};
#endif
