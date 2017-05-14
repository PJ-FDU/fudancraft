#include "GridMap.h"
#include <vector>

USING_NS_CC;

GridMap* GridMap::create(const cocos2d::TMXTiledMap * tiled_map)
{
	GridMap *ret = new (std::nothrow) GridMap();
	if (ret->initWithTiledMap(tiled_map))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool GridMap::initWithTiledMap(const TMXTiledMap* tiled_map)
{
	map_height = int(tiled_map->getMapSize().height);
	map_width = int(tiled_map->getMapSize().width);
	grid_height = int(tiled_map->getTileSize().height);
	grid_width = int(tiled_map->getTileSize().width);
	gmap = std::vector<std::vector<int>>(map_width, std::vector<int>(map_height, 0));
	return(true);
}

Point GridMap::getPoint(const GridPoint& gp)
{
	return(Point(gp.x * grid_width, gp.y * grid_height));
}

GridPoint GridMap::getGridPoint(const Point& p)
{
	return(GridPoint(int(p.x) / grid_width, int(p.y) / grid_height));
}

void GridMap::occupyPosition(const GridPoint& pos)
{
	gmap[pos.x][pos.y] = 1;
}

void GridMap::occupyPosition(const Point& pos)
{
	occupyPosition(getGridPoint(pos));
}

const std::vector<std::vector<int>>& GridMap::getLogicalGridMap()
{
	return(gmap);
}