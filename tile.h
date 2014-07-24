#ifndef __TILE__
#define __TILE__
#include <stdint.h>

struct Position
{
	int32_t x,y,z;
	Position(int32_t x,int32_t y,int32_t z) : x(x),y(y),z(z){}
};

class Tile
{
	public:
		Tile(int32_t stackCount,int32_t x,int32_t y,int32_t z);
		Tile();
		int32_t getIDFromStack(int32_t stack);
		bool setIDOnStack(int32_t stack,int32_t data);
		Position getPosition();
		void setPosition(Position position);

	private:
		int32_t *stackItemID;
		int32_t maxStack;
                Position *pos;
};

class TownTile : public Tile
{
	public:
	TownTile();
	TownTile(int32_t x,int32_t y);

	bool reserved;
};
#endif
