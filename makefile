CXXFLAGS = -s -ggdb -g -std=c++0x
LIBS = -lboost_system -lpthread -lboost_thread-mt -lmysqlclient

tcpserver.o: tcpserver.cpp
	g++ $(CXXFLAGS) -c tcpserver.cpp

server.o: server.cpp
	g++ $(CXXFLAGS) -c server.cpp

database.o: database.cpp
	g++ $(CXXFLAGS) -c database.cpp

toolbox.o: toolbox.cpp
	g++ $(CXXFLAGS) -c toolbox.cpp

session.o: session.cpp
	g++ $(CXXFLAGS) -c session.cpp

tile.o: tile.cpp
	g++ $(CXXFLAGS) -c tile.cpp

server: tile.o server.o tcpserver.o database.o toolbox.o session.o
	g++ $(CXXFLAGS) $(LDFLAGS) $(LIBS) tile.o server.o tcpserver.o database.o toolbox.o session.o -o server
clean:
	rm *.o
.PHONY: clean
