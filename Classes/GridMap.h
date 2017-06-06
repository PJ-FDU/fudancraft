#pragma once

#include "cocos2d.h"
#include "fudancraft.h"


struct GridPoint
{
	GridPoint(int _x = 0, int _y = 0) : x(_x), y(_y) {}
	int x, y;


	bool operator==(const GridPoint& gp2) const;
	friend GridPoint operator+(const GridPoint& gp1, const GridPoint& gp2);
	friend GridPoint operator-(const GridPoint& gp1, const GridPoint& gp2);
	GridPoint getDirectionVector();
};

typedef GridPoint GridVec;

typedef std::vector<GridPoint>  GridPath;

struct GridSize
{
	int width;
	int height;

	GridSize(int _width = 0, int _height = 0) : width(_width), height(_height) {}
};

struct GridRect
{
	GridPoint gp;
	GridSize size;

	GridRect(GridPoint _gp = GridPoint(), GridSize _size = GridSize()) : gp(_gp), size(_size) {}
};

class GridMap : public cocos2d::Ref
{
public:
	static GridMap* create(const cocos2d::TMXTiledMap * tiled_map);
	GridPoint findFreePositionNear(/*const GridVec& gv,*/ const GridPoint& gp);
	cocos2d::Point getPoint(const GridPoint& gp);
	GridPoint getGridPoint(const cocos2d::Point& p);
	cocos2d::Point getGridRectCenter(const GridRect& grec);
	cocos2d::Point getPointWithOffset(const GridPoint& gp);
	GridPoint getGridPointWithOffset(const cocos2d::Point& p);
	bool checkPosition(const GridRect& grec);
	bool checkPosition(const GridPoint& gp);
	bool occupyPosition(const GridPoint& pos);
	bool occupyPosition(const cocos2d::Point& pos);
	bool occupyPosition(const GridRect& grec);
	void leavePosition(const GridPoint& pos);
	void leavePosition(const GridRect& grec);
	std::vector<std::vector<int>>& getLogicalGridMap();

	bool hasApproached(const cocos2d::Point& cur_fp, const GridPoint& dest_gp);
private:
	bool initWithTiledMap(const cocos2d::TMXTiledMap* tiled_map);
	std::vector<std::vector<int>> gmap;
	int map_width, map_height;
	int grid_width, grid_height;
	cocos2d::Vec2 offset_vec;
};