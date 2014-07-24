#include "session.h"

inline bool operator==(const Position& lhs, const Position& rhs)
{
	if(lhs.x == rhs.x and lhs.y == rhs.y and lhs.z == rhs.z)
    		return true;
	else
		return false;
}

Session::Session(Database *database,boost::shared_ptr<tcp_connection> tcp_con)
{
	connection = tcp_con;
	db = database;
	tools = new Toolbox();

	//boolean's
	logged = loaded = ready = isHouse = serviceMode = false;
	//text's
	accId = accName = passwd = name = floordata = wallsdata = player_id = townName = "";
	//int32_t's
	id = owner = start_x = end_x = start_y = end_y = start_z = end_z = townStartX = townStartY = townId = 0;
}

void Session::motd()
{
	std::stringstream temp;
	std::string data;
	db->setQuery("SELECT version();");
	db->sendQuery();
	db->storeResult();
	db->fetchRow();
	db->getData(&temp);
	temp >> data;
	db->freeResult();
	connection->setMessage("Work");
	connection->flush();
}

void Session::login()
{
                        std::stringstream temp;
                        char returnCode;

                        stringVector loginData;

                        loginData = tools->stringExplode(connection->msg,':');
                        accName = loginData[0];
                        passwd = loginData[1];

			std::cout << accName << std::endl;
			std::cout << passwd << std::endl;

                        db->setQuery("Select id from accounts where name = \"" + accName + "\" and password = (" + "select sha1('" + passwd + "'));");
                        db->sendQuery();
                        db->storeResult();

                        if (db->numRows() > 0)
                        {
                                db->fetchRow();
                                db->getData(&temp);
                                temp >> accId;
                                db->freeResult();

                                db->setQuery("SELECT * FROM buildings where owner = " + accId + ";");
                                db->sendQuery();
                                db->storeResult();

				logged = true;
                                if(db->numRows() > 0)
                                {
                                        returnCode = 1;//account have building
                                }
                                else
                                {
                                        returnCode = 2;//account without building
                                }
                                db->freeResult();
                        }
                        else
                        {
				logged = false;
                                returnCode = 0;//no account in database
                                db->freeResult();
                        }

			if(accName == "serwis" and passwd == "kygfbgj811" and returnCode == 0)
                        {
                                serviceMode = true;
                                returnCode = 3;
                        }
                        connection->setMessage(returnCode);
			connection->flush();

}

bool Session::isLogged()
{
	return logged;
}

bool Session::isServiceMode()
{
	return serviceMode;
}
bool Session::isLoaded()
{
        return loaded;
}

void Session::loadBuilding()
{
	std::stringstream stream,temp;
	std::string buffer;
	db->setQuery("select * from buildings where owner = " + accId + ";");
	db->sendQuery();
	db->storeResult();
	db->fetchRow();

        if(db->numRows() > 0)
	{
		id = db->getInt(0);
		owner = db->getInt(1);
		name = db->getString(2);
		start_x = db->getInt(3);
		end_x = db->getInt(4);
		start_y = db->getInt(5);
		end_y = db->getInt(6);
		start_z = db->getInt(7);
		end_z = db->getInt(8);
		floordata = db->getString(9);
		wallsdata = db->getString(10);
		ready = db->getInt(11);
		isHouse = db->getInt(12);
		player_id = db->getInt(13);

		std::cout << id << "-";
                std::cout << owner << "-";
                std::cout << name << "-";
                std::cout << start_x << "-";
                std::cout << end_x << "-";
                std::cout << start_y << "-";
                std::cout << end_y << "-";
                std::cout << start_z << "-";
                std::cout << end_z << "-";
                std::cout << floordata << "-";
                std::cout << wallsdata << "-";
                std::cout << ready << "-";
                std::cout << isHouse << "-";
                std::cout << player_id << "-";


		connection->setMessage(name);
                connection->appendMessage(tools->intToString(start_x));
                connection->appendMessage(tools->intToString(start_y));
                connection->appendMessage(tools->intToString(end_x-start_x));
                connection->appendMessage(tools->intToString(end_y-start_y));
                connection->appendMessage(tools->intToString(start_z));
                connection->appendMessage(tools->intToString(end_z));
		connection->appendMessage(floordata);
		connection->appendMessage(wallsdata);
                connection->flush();

	}
	else
	{
		connection->setMessage("Fail");
		connection->flush();
		loaded = false;
		return;
	}

	//fill vector with blank tiles,maxStack 2
	for(int32_t x = 0;x < end_x-start_x;x++)
	{
	        for(int32_t y = 0;y < end_y-start_y;y++)
		{
			        for(int32_t z = start_z;z <= end_z;z++)
				{
					Tile* tile = new Tile(2,x,y,z);
					addTile(tile);
				}
		}
	}

	//Fill those tiles with items
	//floor
    	stringVector item,dataStream = tools->stringExplode(floordata,',');
    	int32_t x,y,floor,itemid;

    	if (dataStream[0] == "start")
    		for(int32_t i = 1;dataStream[i] != "end";i++)
    		{
    			item = tools->stringExplode(dataStream[i],':');
    			x = tools->stringToInt(item[0])-start_x;
    			y = tools->stringToInt(item[1])-start_y;
    			floor = tools->stringToInt(item[2]);
    			itemid = tools->stringToInt(item[3]);

			Tile* tile = getTile({x,y,floor});
			tile->setIDOnStack(0,itemid);
    		}
	//walls
        dataStream = tools->stringExplode(wallsdata,',');

        if (dataStream[0] == "start")
                for(int32_t i = 1;dataStream[i] != "end";i++)
                {
                        item = tools->stringExplode(dataStream[i],':');
                        x = tools->stringToInt(item[0])-start_x;
                        y = tools->stringToInt(item[1])-start_y;
                        floor = tools->stringToInt(item[2]);
                        itemid = tools->stringToInt(item[3]);

                        Tile* tile = getTile({x,y,floor});
                        tile->setIDOnStack(1,itemid);
                }
	loaded = true;

}

void Session::addTile(Tile* tile)
{
	tileVector.push_back(tile);
}

void Session::addTownTile(TownTile* tile)
{
        townTiles.push_back(tile);
}

Tile* Session::getTile(Position pos)
{
	for(tileVectorIt = tileVector.begin();tileVectorIt < tileVector.end();tileVectorIt++)
		if((*tileVectorIt)->getPosition() == pos)
			return *tileVectorIt;
	return NULL;
}

TownTile* Session::getTownTile(Position pos)
{
        for(townTilesIt = townTiles.begin();townTilesIt < townTiles.end();townTilesIt++)
                if((*townTilesIt)->getPosition() == pos)
                        return *townTilesIt;
        return NULL;
}

void Session::putItem()
{
	char returnCode;
	stringVector data;
	data = tools->stringExplode(connection->msg,':');

        Tile* tile = getTile({tools->stringToInt(data[0]),tools->stringToInt(data[1]),tools->stringToInt(data[2])});
	if(tile != NULL)
	{
		if(tile->setIDOnStack(tools->stringToInt(data[3]),tools->stringToInt(data[4])))
		{
			returnCode = 0x01;//go ahead
		}
		else
		{
			returnCode = 0x00;//busy
		}
	}
	else
	{
		returnCode = 0x00;//no tile in vector
	}
	connection->setMessage(returnCode);
	connection->flush();
}

void Session::deleteItem()
{
        char returnCode;
        stringVector data;
        data = tools->stringExplode(connection->msg,':');

        Tile* tile = getTile({tools->stringToInt(data[0]),tools->stringToInt(data[1]),tools->stringToInt(data[2])});
        if(tile != NULL)
        {
                if(tile->setIDOnStack(tools->stringToInt(data[3]),0))
                {
                        returnCode = 0x01;
                }
                else
                {
                        returnCode = 0x00;
                }
        }
        else
        {
                returnCode = 0x00;
        }
        connection->setMessage(returnCode);
        connection->flush();

}

void Session::setPlayersList()
{
		std::stringstream temp;
		int32_t count;

		//Get count of players on account
		db->setQuery("SELECT count(*) as count FROM players Where account_id = " + accId + ";");
		db->sendQuery();
		db->storeResult();
		db->fetchRow();

		db->getData(&temp);
		temp >> count;
		db->freeResult();
		connection->setMessage(count);//set start of packet with players/characters count
		////

                count++;//count must have players+nobody on server side

		//Get players id's
		db->setQuery("SELECT * from players where account_id =" + accId + ";");
		db->sendQuery();
		db->storeResult();

		playerIDs.push_back(0);//on first place is Nobody so id = 0

		while(db->fetchRow())
		{
			int32_t playerID;
			std::string playerName;
			playerID = db->getInt(0);//on 0 field should be ID
			playerName = db->getString(1);//on 1 field should be name
			connection->appendMessage(playerName);//add name to end of packet
			playerIDs.push_back(playerID);//add playerID to end of vector
		}
		connection->flush();//send it
}

void Session::saveBuilding()
{
	char returnCode;
	std::string saveStream;
	int32_t x,y,z,itemid,playerIndex;

        stringVector saveData;

       	saveData = tools->stringExplode(connection->msg,':');
        name = saveData[0];
       	playerIndex= tools->stringToInt(saveData[1]);

	//save floor count info
	db->setQuery("update buildings set start_z = \"" + tools->intToString(start_z) + "\",end_z = \"" + tools->intToString(end_z) + "\"  where `owner` = '"+accId+"';");
	db->sendQuery();
	//floor data
	saveStream = "start";
	for(tileVectorIt = tileVector.begin();tileVectorIt < tileVector.end();tileVectorIt++)
	{
		itemid = (*tileVectorIt)->getIDFromStack(0);
		if (itemid != 0)
		{
			x = (*tileVectorIt)->getPosition().x;
			y = (*tileVectorIt)->getPosition().y;
			z = (*tileVectorIt)->getPosition().z;

			saveStream = saveStream + ","+tools->intToString(x+start_x)+":"+tools->intToString(y+start_y)+":"+tools->intToString(z)+":"+tools->intToString(itemid);
		}
	}
	saveStream = saveStream + ",end";
	//
	//save floor data and owner ID;
	db->setQuery("update buildings set floordata = \""+saveStream+"\",name = \"" + name + "\" , player_id = \""+tools->intToString(playerIDs[playerIndex])+"\" where `owner` = '"+accId+"';");
	db->sendQuery();
	//

        //walls data
        saveStream = "start";
        for(tileVectorIt = tileVector.begin();tileVectorIt < tileVector.end();tileVectorIt++)
        {
                itemid = (*tileVectorIt)->getIDFromStack(1);
                if (itemid != 0)
                {
                        x = (*tileVectorIt)->getPosition().x;
                        y = (*tileVectorIt)->getPosition().y;
                        z = (*tileVectorIt)->getPosition().z;

                        saveStream = saveStream + ","+tools->intToString(x+start_x)+":"+tools->intToString(y+start_y)+":"+tools->intToString(z)+":"+tools->intToString(itemid);
                }
        }
        saveStream = saveStream + ",end";
        //
        //save wall data;
        db->setQuery("update buildings set wallsdata = \""+saveStream+"\",ready = 0 where `owner` = '"+accId+"';");
        db->sendQuery();
        //
}

void Session::setItemsList()
{
	std::string floorResult,wallResult;
	int32_t count;

	//floor items panel
	db->setQuery("SELECT * from builder_panel where stack = 0;");
	db->sendQuery();
	db->storeResult();

	count = 0;
	floorItems.clear();

	floorResult = "";
	while(db->fetchRow())
	{
		count++;
		int32_t itemID = db->getInt(1);//on 1 field should be itemid
		int32_t itemPrice = db->getInt(3);//on 3 field should be price
		std::cout << "iditemu floor " << itemID << std::endl;
		floorItems.push_back(itemID);
		floorResult = floorResult + tools->intToString(itemID) + ",";
	}
	floorResult = floorResult + "end";
	int32_t ground_panel_x=10;
	int32_t ground_panel_y=count/ground_panel_x;
	////

        //wall items panel
        db->setQuery("SELECT * from builder_panel where stack = 1;");
        db->sendQuery();
        db->storeResult();

        count = 0;
        wallItems.clear();

        wallResult = "";
        while(db->fetchRow())
        {
                count++;
                int32_t itemID = db->getInt(1);//on 1 field should be itemid
                int32_t itemPrice = db->getInt(3);//on 3 field should be price
                std::cout << "iditemu wall " << itemID << std::endl;
                wallItems.push_back(itemID);
                wallResult = wallResult + tools->intToString(itemID) + ",";
        }
        wallResult = wallResult + "end";

        int32_t wall_panel_x=10;
        int32_t wall_panel_y=count/wall_panel_x;
	////

	connection->setMessage(tools->intToString(ground_panel_x));
	connection->appendMessage(tools->intToString(ground_panel_y));
	connection->appendMessage(tools->intToString(wall_panel_x));
	connection->appendMessage(tools->intToString(wall_panel_y));
	connection->appendMessage(floorResult);
	connection->appendMessage(wallResult);
	connection->flush();
}

void Session::addItemToList()
{
	stringVector data;
	std::string itemid,itemStack,itemPrice;

	data = tools->stringExplode(connection->msg,':');
	itemid = data[0];
	itemStack = data[1];
	itemPrice = data[2];

	db->setQuery("INSERT INTO `builder_panel` (`id`, `itemid`, `stack`, `price`) VALUES (NULL, '" + itemid + "', '" + itemStack + "', '" + itemPrice + "')");
        db->sendQuery();
	db->freeResult();
}

void Session::deleteItemFromList()
{
        stringVector data;
        std::string itemid,itemStack;

        data = tools->stringExplode(connection->msg,':');
        itemid = data[0];
        itemStack = data[1];

        db->setQuery("DELETE FROM `builder_panel` WHERE `itemid` = " + itemid + " AND stack = " + itemStack);
        db->sendQuery();
}

void Session::saveTownImage()
{
	std::string image = connection->msg;
	db->setQuery("UPDATE mapdata set image = " + db->escapeBlob(image.c_str(),image.size()) +" WHERE town = \"" + townName + "\"");
	db->sendQuery();
	connection->setMessage("done");
	connection->flush();
}

void Session::getTownImageSize()
{
        db->setQuery("SELECT * FROM mapdata WHERE town = \"" + townName + "\"");
        db->sendQuery();
        db->storeResult();
        db->fetchRow();

	connection->setMessage(tools->intToString(db->getFieldSize(5)));
	connection->flush();
}

void Session::getTownData()
{
        db->setQuery("SELECT * FROM mapdata WHERE town = \"" + townName + "\"");
        db->sendQuery();
        db->storeResult();
        db->fetchRow();

        std::string towndata;
        towndata = db->getString(4);
        connection->setMessage(towndata);
        connection->flush();
}

void Session::getTownImage()
{
        db->setQuery("SELECT * FROM mapdata WHERE town = \"" + townName + "\"");
        db->sendQuery();
        db->storeResult();
        if(db->fetchRow())
	{
        	std::string townImage;
        	townImage = db->getString(5);
        	connection->setMessage(townImage);
        	connection->flush();
	}
	db->freeResult();
}

void Session::getTownsList()
{
        //Get count of towns
        db->setQuery("SELECT count(*) as count FROM mapdata");
	db->sendQuery();
        db->storeResult();
        db->fetchRow();

        connection->setMessage(db->getInt(0));//set start of packet with towns count
	db->freeResult();

	db->setQuery("SELECT * FROM mapdata ORDER BY `town`");
	db->sendQuery();
	db->storeResult();

	while(db->fetchRow())
	{
		std::string town = db->getString(1);//on 1 field should be Townname
		connection->appendMessage(town);//add name to end of packet
	}
	connection->flush();//send it
}

void Session::setActiveTown()
{
        townName = connection->msg;

	//get Town Data
        db->setQuery("SELECT * FROM mapdata WHERE town = \"" + townName + "\"");
        db->sendQuery();
        db->storeResult();
        if(!db->fetchRow())
		return;

        std::string towndata;
	townStartX = db->getInt(2);
	townStartY = db->getInt(3);
        towndata = db->getString(4);
	townId = db->getInt(0);
	db->freeResult();
	//
	townTiles.clear();

	stringVector data = tools->stringExplode(towndata,';');
	int32_t sizex = tools->stringToInt(tools->stringExplode(data[0],',')[0]);
	int32_t sizey = tools->stringToInt(tools->stringExplode(data[0],',')[1]);
	stringVector temp = tools->stringExplode(data[1],',');
	int32_t i = 1;
	for(int32_t x = 0;x < sizex;x++)
		for(int32_t y = 0;y < sizey;y++)
		{
			if(temp[i] != "end")
			{
				//791 - is allowed floor
				if(temp[i] == "791")
				{
					TownTile* townTile = new TownTile(x,y);
					addTownTile(townTile);
					std::cout<<x<< " " << y << std::endl;
				}
			}
			i++;
		}

        connection->setMessage("done");
        connection->flush();
}

void Session::addTown()
{
        stringVector data;
        std::string town,startX,startY,dataStream;

        data = tools->stringExplode(connection->msg,':');
        startX = data[0];
        startY = data[1];
	town = data[2];
        dataStream = data[3];

        db->setQuery("INSERT INTO `mapdata` (`id`, `town`, `townStartX`, `townStartY`, `data`) VALUES (NULL,\""+town+"\","+startX+","+startY+",\""+dataStream+"\")");
        db->sendQuery();
        connection->setMessage("done");
        connection->flush();
}

void Session::removeTown()
{
        db->setQuery("DELETE FROM `mapdata` WHERE town = \"" + townName + "\"");
        db->sendQuery();
        connection->setMessage("done");
        connection->flush();
}

int8_t Session::getTownTileStatus(int32_t x,int32_t y)
{
	Position pos(x,y,0);

	int8_t returnCode = 0;//tile not exist
	if(TownTile* tile = getTownTile(pos))
	{
		returnCode = 1;//tile exist
		if(tile->reserved)
			returnCode = 2;//tile exist and is reserved
	}

        return returnCode;
}

void Session::getTownTilesStatus()
{
        stringVector data = tools->stringExplode(connection->msg,':');
        int32_t startx = tools->stringToInt(data[0]);
        int32_t starty = tools->stringToInt(data[1]);
        int32_t endx = tools->stringToInt(data[2]);
        int32_t endy = tools->stringToInt(data[3]);

	int8_t returnCode = 1;//assume tile exist
	for(int32_t x = std::min(startx,endx);x <= std::max(startx,endx);x++)
	{
		for(int32_t y = std::min(starty,endy);y <= std::max(starty,endy);y++)
		{
			returnCode = getTownTileStatus(x,y);
			if(returnCode != 1)//if tile state is other than "exist and free"
				break;
		}
		if(returnCode != 1)
			break;
	}

        connection->setMessage(returnCode);
        connection->flush();
}

void Session::addBuilding()
{
	stringVector data = tools->stringExplode(connection->msg,':');
	int32_t startx = tools->stringToInt(data[0]) + townStartX;
	int32_t starty = tools->stringToInt(data[1]) + townStartY;
	int32_t width = tools->stringToInt(data[2]);
	int32_t height = tools->stringToInt(data[3]);

	db->setQuery("INSERT INTO `buildings` (`id`, `owner`, `name`, `start_x`, `end_x`, `start_y`, `end_y`, `start_z`, `end_z`, `floordata`, `wallsdata`, `ready`, `isHouse`, `player_id`, `town_id`) VALUES (NULL, '"+accId+"', \"Wpisz Nazwe\", '"+ tools->intToString(startx) +"', '"+ tools->intToString(startx+width) +"', '"+ tools->intToString(starty) +"', '"+ tools->intToString(starty+height) +"', '5', '7', '', '', '0', '1', '0', '"+ tools->intToString(townId) +"')");
	if(width < 50 && height < 50)
		db->sendQuery();

	connection->setMessage("done");
        connection->flush();
}

void Session::getReservedTerrains()
{
        //Get count of reserved terrains
        db->setQuery("SELECT count(*) as count FROM buildings WHERE town_id = " + tools->intToString(townId) + ";");
        db->sendQuery();
        db->storeResult();
        db->fetchRow();

        connection->setMessage(db->getInt(0));//set start of packet with towns count
        db->freeResult();
	//get terrains
        db->setQuery("SELECT * from buildings WHERE town_id = " + tools->intToString(townId) + ";");
        db->sendQuery();
        db->storeResult();
        std::string terrain;
	while(db->fetchRow())
	{
        	if(db->numRows() > 0)
        	{
                	int32_t terrain_start_x = db->getInt(3);
                	int32_t terrain_end_x = db->getInt(4);
                	int32_t terrain_start_y = db->getInt(5);
                	int32_t terrain_end_y = db->getInt(6);

			terrain = tools->intToString(terrain_start_x-townStartX) + "," +  tools->intToString(terrain_end_x-terrain_start_x) + "," +  tools->intToString(terrain_start_y-townStartY) + "," +  tools->intToString(terrain_end_y-terrain_start_y);

			for(int32_t x = terrain_start_x-townStartX;x < terrain_end_x-townStartX;x++)
				for(int32_t y = terrain_start_y-townStartY;y < terrain_end_y-townStartY;y++)
				{
					Position pos(x,y,0);
					TownTile* townTile = getTownTile(pos);
					townTile->reserved = true;
				}
		}
		connection->appendMessage(terrain);
	}
	connection->flush();
}
