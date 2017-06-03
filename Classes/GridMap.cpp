#include "GridMap.h"
#include <vector>

USING_NS_CC;

bool GridPoint::operator==(const GridPoint& gp2) const
{
	return(x == gp2.x && y == gp2.y);
}

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
	offset_vec = Vec2(grid_width / 2, grid_height / 2);
	gmap = std::vector<std::vector<int>>(map_width, std::vector<int>(map_height, 0));

	auto decoration_layer = tiled_map->getLayer("TerrainDecorationsLayer");
	for (int gx = 0; gx < map_width; gx++)
		for (int gy = 0; gy < map_height; gy++)
		{
			int tile_gid = decoration_layer->getTileGIDAt(Vec2(gx, map_height - 1 - gy));
			if (tile_gid > 0)
			{
				auto prop = tiled_map->getPropertiesForGID(tile_gid);
				if (prop.isNull())
					continue;
				auto prop_valuemap = prop.asValueMap();
				int z_index = prop_valuemap["z_index"].asInt();
				gmap[gx][gy] = z_index;
			}
		}

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

Point GridMap::getPointWithOffset(const GridPoint& gp)
{
	return(getPoint(gp) + offset_vec);
}

GridPoint GridMap::getGridPointWithOffset(const Point& p)
{
	return(getGridPoint(p + offset_vec));
}

bool GridMap::occupyPosition(const GridPoint& pos)
{
	if (!gmap[pos.x][pos.y])
	{
		gmap[pos.x][pos.y] = 1;
		return(1);
	}
	return(0);
}

bool GridMap::occupyPosition(const Point& pos)
{
	return(occupyPosition(getGridPoint(pos)));
}

void GridMap::leavePosition(const GridPoint& pos)
{
	gmap[pos.x][pos.y] = 0;
}

std::vector<std::vector<int>>& GridMap::getLogicalGridMap()
{
	return(gmap);
}

bool GridMap::hasApproached(const Point& cur_fp, const GridPoint& dest_gp)
{
	Point dest_fp = getPointWithOffset(dest_gp);
	if ((dest_fp - cur_fp).length() < POS_OFFSET)
		return(true);
	return(false);
}