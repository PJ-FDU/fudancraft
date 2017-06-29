#pragma once

#include "cocos2d.h"
#include "fudancraft.h"

class GridSize;

struct GridPoint
{
	GridPoint(int _x = 0, int _y = 0) : x(_x), y(_y) {}
	int x, y;


	bool operator==(const GridPoint& gp2) const;
	friend GridPoint operator+(const GridPoint& gp1, const GridPoint& gp2);
	friend GridPoint operator-(const GridPoint& gp1, const GridPoint& gp2);
	friend GridPoint operator-(const GridPoint& gp, const GridSize& gz);
	/**
	 * \brief render a point to point limit to({-1,0,1},{-1,0,1})
	 * \return one of point in eight direction
	 */
	GridPoint getDirectionVector();
};

typedef GridPoint GridVec;

typedef std::vector<GridPoint>  GridPath;

/**
 * \brief similar to cocos2d::Size, but implements in integer
 */
struct GridSize
{
	int width;
	int height;

	GridSize(int _width = 0, int _height = 0) : width(_width), height(_height) {}
	friend GridSize operator/(const GridSize& gz, int s);
};

/**
 * \brief similar to cocos2d::Rect, but implements in integer
 */
struct GridRect
{
	GridPoint gp;
	GridSize size;

	GridRect(GridPoint _gp = GridPoint(), GridSize _size = GridSize(), bool center = false);
};

class GridMap : public cocos2d::Ref
{
public:
	static GridMap* create(const cocos2d::TMXTiledMap * tiled_map);
	/**
	 * \return the nearest grid point
	 */
	GridPoint findFreePositionNear(/*const GridVec& gv,*/ const GridPoint& gp);
	/**
	 * \return get coordinate of point 
	 */
	cocos2d::Point getPoint(const GridPoint& gp);
	GridPoint getGridPoint(const cocos2d::Point& p);
	cocos2d::Point getGridRectCenter(const GridRect& grec);
	/** 
	 * \return get coordinate of point with offset 
	 */
	cocos2d::Point getPointWithOffset(const GridPoint& gp);
	/** 
	 * \return return grid point of point with offset
	 */
	GridPoint getGridPointWithOffset(const cocos2d::Point& p);
	/**
	 * \return if the rectangle has been taken
	 */
	bool checkPosition(const GridRect& grec);
	/**
	 * \return if a point has been taken
	 */
	bool checkPosition(const GridPoint& gp);
	/**
	 * \brief try to occupy the position
	 * \return true if success
	 */
	bool occupyPosition(int id, const GridPoint& pos, bool occupy_grid = true);
	bool occupyPosition(int id, const cocos2d::Point& pos, bool occupy_grid = true);
	bool occupyPosition(int id, const GridRect& grec, bool occupy_grid = true);
	/**
	 * \return if the point is in the map
	 */
	bool checkPointInMap(const GridPoint& gp) const;
	bool checkPointInMap(int x, int y) const;
	/**
	 * \brief leave the point 
	 */
	void leavePosition(const GridPoint& pos, bool occupy_grid = true);
	/**
	 * \brief leave the rectangle
	 */
	void leavePosition(const GridRect& grec, bool occupy_grid = true);
	/**
	 * \return a logical grid map
	 */
	std::vector<std::vector<int>>& getLogicalGridMap();
	/**
	 * \return all the ID that in the rectangle
	 */
	std::vector<int> getUnitIDAt(const GridRect& range) const;
	/**
	 * \return unit id at point gp
	 */
	int getUnitIDAt(const GridPoint& gp) const;

	/**
	 * \brief 
	 * \param cur_fp current point
	 * \param dest_gp destination grid point
	 * \return  if the current point is neat the center of destination
	 */
	bool hasApproached(const cocos2d::Point& cur_fp, const GridPoint& dest_gp);
	void setFogLayer(cocos2d::TMXLayer* _fog_layer);
	void clearFog(const GridRect& grec);
private:
	bool initWithTiledMap(const cocos2d::TMXTiledMap* tiled_map);
	
	std::vector<std::vector<int>> gmap;
	std::vector<std::vector<int>> umap;
	std::vector<std::vector<int>> fmap;

	cocos2d::TMXLayer* fog_layer;
	int map_width, map_height;
	int grid_width, grid_height;
	cocos2d::Vec2 offset_vec;
};