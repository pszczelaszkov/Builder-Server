#include "tile.h"

Tile::Tile()
{
        pos = new Position(0,0,0);
        stackItemID = new int32_t[1];
        maxStack = 0;

        for(int32_t i = 0;i <= maxStack;i++)
                setIDOnStack(i,0);
}

Tile::Tile(int32_t stackCount,int32_t x,int32_t y,int32_t z)
{
	pos = new Position(x,y,z);
	stackItemID = new int32_t[stackCount];
	maxStack = stackCount-1;

	for(int32_t i = 0;i <= maxStack;i++)
		setIDOnStack(i,0);
}

TownTile::TownTile()
{
	reserved = false;
}

TownTile::TownTile(int32_t x,int32_t y)
{
        setPosition(Position(x,y,0));
	reserved = false;
}

int32_t Tile::getIDFromStack(int32_t stack)
{
	return stackItemID[stack];
}

bool Tile::setIDOnStack(int32_t stack,int32_t data)
{
	if(stack <= maxStack and stack > -1)
        {
		if(stackItemID[stack] != data)
			stackItemID[stack] = data;
		else
			return false;
	}
	else
		return false;
	return true;
}

Position Tile::getPosition()
{
	return *pos;
}

void Tile::setPosition(Position position)
{
	*pos = position;
}
