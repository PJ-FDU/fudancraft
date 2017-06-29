#include "GridMap.h"
#include <vector>

USING_NS_CC;

bool GridPoint::operator==(const GridPoint& gp2) const
{
	return(x == gp2.x && y == gp2.y);
}

GridPoint GridPoint::getDirectionVector()
{
	auto sgn = [](int a)->int 
	{
		return ((a)? ((a > 0) ? (1) : (-1)) : (0)); 
	};
	return GridPoint(sgn(x), sgn(y));
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

GridPoint GridMap::findFreePositionNear(const GridPoint& origin_gp)
{
	if (checkPosition(origin_gp))
		return(origin_gp);

	for(int i = 1; i < map_height + map_width; i++)
	{
		GridPath dir_vec = {GridVec(1, 1), GridVec(1, -1), GridVec(-1, -1), GridVec(-1, 1)};
		GridPoint gp = origin_gp + GridVec(-i, 0);
		for (int dir = 0; dir < 4; dir++)
			for (int j = 1; j <= i; j++)
			{
				gp = gp + dir_vec[dir];
				if (checkPosition(gp))
					return(gp);
			}
		
	}

	return(-1, -1 );
}

bool GridMap::initWithTiledMap(const TMXTiledMap* tiled_map)
{
	map_height = int(tiled_map->getMapSize().height);
	map_width = int(tiled_map->getMapSize().width);
	grid_height = int(tiled_map->getTileSize().height);
	grid_width = int(tiled_map->getTileSize().width);
	offset_vec = Vec2(grid_width / 2, grid_height / 2);
	gmap = std::vector<std::vector<int>>(map_width, std::vector<int>(map_height, 0));
	umap = std::vector<std::vector<int>>(map_width, std::vector<int>(map_height, 0));
	fmap = std::vector<std::vector<int>>(map_width, std::vector<int>(map_height, 1));

	auto decoration_layer = tiled_map->getLayer("InfoLayer");
	for (int gx = 0; gx < map_width; gx++)
		for (int gy = 0; gy < map_height; gy++)
		{
			int tile_gid = decoration_layer->getTileGIDAt(Vec2(gx, map_height - 1 - gy));
			if (tile_gid > 0)
			{
				gmap[gx][gy] = 1;
			}
		}

	return(true);
}

Point GridMap::getGridRectCenter(const GridRect& grec)
{
	return(getPoint(grec.gp) + Vec2(grid_width * grec.size.width / 2, grid_height * grec.size.height / 2));
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

bool GridMap::checkPosition(const GridRect& grec)
{
	for (int x = grec.gp.x; x < grec.gp.x + grec.size.width; x++)
		for (int y = grec.gp.y; y < grec.gp.y + grec.size.height; y++)
			if (x < 0 || x >= map_width || y < 0 || y >= map_height || gmap[x][y])
				return(false);
	return(true);
}

bool GridMap::checkPosition(const GridPoint & gp)
{
	if (gp.x >= 0 && gp.x < map_width && gp.y >= 0 && gp.y < map_height && gmap[gp.x][gp.y] == 0)
		return true;
	return false;
}

bool GridMap::occupyPosition(int id, const GridPoint& pos, bool occupy_grid)
{
	if (occupy_grid)
		if (checkPosition(pos))
		{
			gmap[pos.x][pos.y] = 1;
			umap[pos.x][pos.y] = id;
			return(true);
		}
		else
			return(false);
	else
	{
		umap[pos.x][pos.y] = id;
		return(true);
	}

	return(false);
}

bool GridMap::occupyPosition(int id, const Point& pos, bool occupy_grid)
{
	return(occupyPosition(id, getGridPoint(pos)), occupy_grid);
}

bool GridMap::occupyPosition(int id, const GridRect& grec, bool occupy_grid)
{
	if (checkPosition(grec))
	{
		for (int x = grec.gp.x; x < grec.gp.x + grec.size.width; x++)
			for (int y = grec.gp.y; y < grec.gp.y + grec.size.height; y++)
			{
				if (occupy_grid)
					gmap[x][y] = 1;
				umap[x][y] = id;
			}
		return(true);
	}
	else
		return(false);
}

bool GridMap::checkPointInMap(const GridPoint & gp) const
{
	return checkPointInMap(gp.x, gp.y);
}

bool GridMap::checkPointInMap(int x, int y) const
{
	return (x >= 0 && x < map_width && y >= 0 && y < map_height);
}

void GridMap::leavePosition(const GridPoint& pos, bool occupy_grid)
{
	if (occupy_grid)
		gmap[pos.x][pos.y] = 0;
	umap[pos.x][pos.y] = 0;
}

void GridMap::leavePosition(const GridRect& grec, bool occupy_grid)
{
	for (int x = grec.gp.x; x < grec.gp.x + grec.size.width; x++)
		for (int y = grec.gp.y; y < grec.gp.y + grec.size.height; y++)
		{
			if (occupy_grid)
				gmap[x][y] = 0;
			umap[x][y] = 0;
		}
}

std::vector<std::vector<int>>& GridMap::getLogicalGridMap()
{
	return(gmap);
}

std::vector<int> GridMap::getUnitIDAt(const GridRect & grec) const
{
	std::vector<int> unit_ids;
	int i = 0;
	for (int x = grec.gp.x; x < grec.gp.x + grec.size.width; x++)
		for (int y = grec.gp.y; y < grec.gp.y + grec.size.height; y++)
			if (checkPointInMap(x, y) && umap[x][y])
			{
				unit_ids.push_back(umap[x][y]);
				i++;
			}
	return unit_ids;
}

int GridMap::getUnitIDAt(const GridPoint & gp) const
{
	if (checkPointInMap(gp))
		return(umap[gp.x][gp.y]);
	else
		return(0);
}

bool GridMap::hasApproached(const Point& cur_fp, const GridPoint& dest_gp)
{
	Point dest_fp = getPointWithOffset(dest_gp);
	if ((dest_fp - cur_fp).length() < POS_OFFSET)
		return(true);
	return(false);
}

void GridMap::setFogLayer(cocos2d::TMXLayer* _fog_layer)
{
	fog_layer = _fog_layer;
}

void GridMap::clearFog(const GridRect& grec)
{
	for (int x = grec.gp.x; x < grec.gp.x + grec.size.width; x++)
		for (int y = grec.gp.y; y < grec.gp.y + grec.size.height; y++)
			if (checkPointInMap(x, y) && fmap[x][y])
			{
				fmap[x][y] = 0;
				auto fog_tile = fog_layer->getTileAt(Vec2(x, map_height - 1 - y));
				if (fog_tile)
					fog_tile->setVisible(false);
			}
}

GridPoint operator+(const GridPoint & gp1, const GridPoint & gp2)
{
	return GridPoint(gp1.x + gp2.x, gp1.y + gp2.y);
}
GridPoint operator-(const GridPoint & gp1, const GridPoint & gp2)
{
	return GridPoint(gp1.x - gp2.x, gp1.y - gp2.y);
}

GridPoint operator-(const GridPoint & gp, const GridSize & gz)
{
	return GridPoint(gp.x - gz.width, gp.y - gz.height);
}

GridSize operator/(const GridSize & gz, int s)
{
	return GridSize(gz.width / s, gz.height / s);
}

GridRect::GridRect(GridPoint _gp, GridSize _size, bool center)
{
	if (center)
	{
		gp = GridPoint(_gp.x - _size.width / 2, _gp.y - _size.height / 2);
		size = _size;
	}
	else
	{
		gp = _gp;
		size = _size;
	}
}
