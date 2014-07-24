#ifndef __SESSION__
#define __SESSION__

#include "tcpserver.h"
#include "database.h"
#include "toolbox.h"
#include "tile.h"
#include <vector>

class tcp_connection;
class Session
{
	public:
		Session(Database *database,boost::shared_ptr<tcp_connection> tcp_con);

		void motd();
		void login();
		bool isLogged();
		bool isLoaded();
		bool isServiceMode();
		void loadBuilding();
		void putItem();
		void deleteItem();
		void setPlayersList();
		void saveBuilding();
		void setItemsList();
		void addItemToList();
		void deleteItemFromList();
		void saveTownImage();
		void getTownImageSize();
		void getTownData();
		void getTownImage();
		void getTownsList();
		void setActiveTown();
		void addTown();
		void removeTown();
		void getTownTilesStatus();
		void addBuilding();
		void getReservedTerrains();

                void addTile(Tile* tile);
		void addTownTile(TownTile* tile);
                Tile* getTile(Position pos);
		TownTile* getTownTile(Position pos);

		int8_t getTownTileStatus(int32_t x,int32_t y);
	private:
		Toolbox *tools;
		Database *db;
		boost::shared_ptr<tcp_connection> connection;

		bool logged,loaded,serviceMode;
		std::string accId,accName,passwd;

		int32_t id;
		int32_t owner;
		std::string name;
		int32_t start_x;
		int32_t end_x;
		int32_t start_y;
		int32_t end_y;
		int32_t start_z;
		int32_t end_z;
		std::string floordata;
		std::string wallsdata;
		bool ready;
		bool isHouse;
		std::string player_id;

		//Tile variables
		std::vector<Tile*> tileVector;
		std::vector<Tile*>::iterator tileVectorIt;
		//
		std::vector<int32_t> playerIDs;

		std::vector<int32_t> floorItems;
		std::vector<int32_t> wallItems;
		//Town variables
		int32_t townId;
		std::string townName;
		int32_t townStartX,townStartY;
                std::vector<TownTile*> townTiles;
                std::vector<TownTile*>::iterator townTilesIt;

};
#endif
