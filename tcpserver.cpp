#include "tcpserver.h"

using boost::asio::ip::tcp;

int tcp_connection::conNum = 0;

typedef boost::shared_ptr<tcp_connection> pointer;

  	pointer tcp_connection::create(boost::asio::io_service& io_service)
  	{
    		return pointer(new tcp_connection(io_service));
  	}

  	tcp::socket& tcp_connection::socket()
  	{
    		return socket_;
  	}

	void tcp_connection::flush()
	{
		if (!writePending)
		{
			writeRequested = false;
			writePending = true;

                        std::cout<<conNum_<< ": " << "Sending: " << backbuf->length() << " Bytes" << std::endl;
			//std::cout<<conNum_<< ": " << *backbuf << std::endl;
			frontbuf->clear();
			std::swap(frontbuf, backbuf);
			ostream.str(*backbuf);

			boost::asio::async_write(socket_, boost::asio::buffer(*frontbuf),
        			boost::bind(&tcp_connection::handle_write, shared_from_this(), _1, _2));
		}
		else
		{
  			writeRequested = true;
		}
	}

	void tcp_connection::start_read()
	{
		inbuf.prepare(socket_.available());
        	boost::asio::async_read_until(socket_, inbuf, "ENDSTREAM", boost::bind(&tcp_connection::handle_read, shared_from_this(), _1, _2));
	}


  	tcp_connection::tcp_connection(boost::asio::io_service& io_service):
		conNum_(conNum++),
		socket_(io_service)
  	{
		ostream.str(bufs[0]);
		backbuf = &bufs[0];
		frontbuf = &bufs[1];

		writePending = false;
		writeRequested = false;

		session = NULL;
		baza = NULL;
  	}

  	void tcp_connection::handle_write(const boost::system::error_code& /*error*/,size_t /*bytes_transferred*/)
  	{
		if (writeRequested)
		{
			flush();
		}
		else
		{
			writePending = false;
		}
  	}

    	void tcp_connection::handle_read(const boost::system::error_code& error,size_t bytes_read)
    	{
        	if (error)
        	{
            		std::cout<<conNum_<< ": " << error.message() << std::endl;
            		return;
        	}
		inbuf.commit(bytes_read);
		parse();
		inbuf.consume(bytes_read);
		start_read();
	}

	void tcp_connection::parse()
	{
		std::istream is(&inbuf);
		separateMessage(&is);

		switch(cmd)
		{
			case 0x01://hello
				if(!session)
				{
					session = new Session(baza,shared_from_this());
					session->motd();
				}
			break;

			case 0x02://login
				if(session)
					if(!session->isLogged())
						session->login();
                	break;

			case 0x03://load building
				if(session)
					if(session->isLogged())
						session->loadBuilding();
			break;

			case 0x04://additem to map
				if(session)
					if(session->isLogged() && session->isLoaded())
                                       		session->putItem();
                        break;

			case 0x05://delete item from map
				if(session)
					if(session->isLogged() && session->isLoaded())
						session->deleteItem();
			break;

			case 0x06://set players list(available owners of the house)
                                if(session)
                                        if(session->isLogged() && session->isLoaded())
                                                session->setPlayersList();
                        break;

			case 0x07://save building
				if(session)
					if(session->isLogged() && session->isLoaded())
						session->saveBuilding();
			break;

			case 0x08://set items list
				if(session)
					session->setItemsList();
			break;

			case 0x09://add item to list
				if(session)
					if(session->isServiceMode())
						session->addItemToList();
			break;

			case 0x0a://delete item from list
                                if(session)
					if(session->isServiceMode())
                        	              	session->deleteItemFromList();
			break;

                        case 0x0b://save rendered town image
                                if(session)
					if(session->isServiceMode())
                                        	session->saveTownImage();
                        break;

			case 0x0c://get size of town image
                                if(session)
                                        session->getTownImageSize();
                        break;

                        case 0x0d://get town tile's info
                                if(session)
                                        session->getTownData();
                        break;

			case 0x0e://get rendered image of town
                                if(session)
                                        session->getTownImage();
                        break;

                        case 0x0f://get towns list
                                if(session)
                                        session->getTownsList();
                        break;

                        case 0x10://setActiveTown
                                if(session)
                                        session->setActiveTown();
                        break;

			case 0x11://addTown
                                if(session)
					if(session->isServiceMode())
                                        	session->addTown();
                        break;

                        case 0x12://removeTown
                                if(session)
                                        if(session->isServiceMode())
                                                session->removeTown();
                        break;

		        case 0x13://getTownTilesStatus
                                if(session)
                                        if(session->isLogged())
                                                session->getTownTilesStatus();
                        break;

                        case 0x14://addBuilding
                                if(session)
                                        if(session->isLogged())
                                                session->addBuilding();
                        break;

                        case 0x15://getReservedTerrain
                                if(session)
                                        if(session->isLogged())
                                                session->getReservedTerrains();
                        break;

			default://something wrong
				std::cout<<conNum_<< ": " << "Corrupted header\n";
			break;
		}
	}

	void tcp_connection::separateMessage(std::istream *is)
	{
		std::string buffer;
		char byte;

		while(!is->eof())
		{
			byte = is->get();
			buffer = buffer + byte;
		}

		std::cout<<conNum_<< ": " << "Received: " << buffer.length() << " Bytes" << std::endl;
		cmd = buffer[0];

		//if(buffer[buffer.length()-1] != 13)
		//	msg = buffer.substr(1,buffer.length()-10);
		//else
		//	msg = buffer.substr(1,buffer.length()-11);

		msg = buffer.substr(1,buffer.find("ENDSTREAM")-1);
	}

	void tcp_connection::setMessage(std::string message)
	{
		        *backbuf = "";
                        *backbuf = message + "\n";
	}

	void tcp_connection::setMessage(char message)
        {
                        *backbuf = "";
                        *backbuf = *backbuf + message + "\n";
        }

        void tcp_connection::appendMessage(std::string message)
        {
                        *backbuf = *backbuf + message + "\n";
        }

        void tcp_connection::appendMessage(char message)
        {
                        *backbuf = *backbuf + message + "\n";
        }


/////////////////////////////////////////TCP_SERVER CLASS////////////////////////////////////////////////////////////////////////////////////////////
  	tcp_server::tcp_server(boost::asio::io_service& io_service,int32_t port,Database *db): acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
  	{
		baza = db;
    		start_accept();
  	}

  	void tcp_server::start_accept()
  	{
    		tcp_connection::pointer new_connection = tcp_connection::create(acceptor_.get_io_service());
		new_connection->baza = baza;
    		acceptor_.async_accept(new_connection->socket(),
    		boost::bind(&tcp_server::handle_accept, this, new_connection,
    		boost::asio::placeholders::error));
  	}

  	void tcp_server::handle_accept(tcp_connection::pointer new_connection,const boost::system::error_code& error)
  	{
		if (!error)
    		{
      			new_connection->start_read();
    		}
		start_accept();
  	}
