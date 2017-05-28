#pragma once

#include "cocos2d.h"

struct GridPoint
{
	GridPoint(int _x = 0, int _y = 0) : x(_x), y(_y) {}
	int x, y;
};

typedef std::vector<GridPoint>  GridPath;

//struct GridPath : public std::vector<GridPoint>
//{
//	GridPath() {};
//	GridPath(const std::initializer_list<GridPoint> init_gps) : std::vector<GridPoint>{ init_gps } {}
//};

class GridMap : public cocos2d::Ref
{
public:
	static GridMap* create(const cocos2d::TMXTiledMap * tiled_map);
	cocos2d::Point getPoint(const GridPoint& gp);
	GridPoint getGridPoint(const cocos2d::Point& p);
	void occupyPosition(const GridPoint& pos);
	void occupyPosition(const cocos2d::Point& pos);
	const std::vector<std::vector<int>>& getLogicalGridMap();
private:
	bool initWithTiledMap(const cocos2d::TMXTiledMap* tiled_map);
	std::vector<std::vector<int>> gmap;
	int map_width, map_height;
	int grid_width, grid_height;
};