#ifndef PATHFINDER_H
#define PATHFINDER_H

#include<vector>
#include "GridMap.h"

enum { UNOCCUPIED, OCCUPIED, START, TERMINAL, INOPEN, INCLOSE };
const int DIRECTION[8][2] = { { -1,-1 },{ -1,0 },{ -1,1 },{ 0,-1 },{ 0,1 },{ 1,-1 },{ 1,0 },{ 1,1 } };
const int DISTANCE = 10;



class Grid
{
private:
	int m_flag;
	int m_x;
	int m_y;
	int m_g;
	int m_h;
	int m_f;
	Grid * m_p_parent;
public:
	Grid();
	int getFlag();
	void setFlag(int flag);
	int getX();
	void setX(int x);
	int getY();
	void setY(int y);
	int getG();
	void setG(int g);
	int getH();
	void setH(int h);
	int getF();
	void setF(int f);
	Grid * getParent();
	void setParent(Grid* parent);
};

class PathFinder {
private:
	int m_width;
	int m_height;
	std::vector<std::vector<Grid>>  m_map_grid;
	Grid * start_point;
	Grid * terminal_point;
	std::vector<Grid *> open_list;
	std::vector<Grid *> close_list;
	std::vector<GridPoint> path;

	/**
	 * \return point with smallest F value
	 */
	Grid * selectNextGrid();
	/**
	 * \return if point exist in open list
	 */
	bool isInOpenList(Grid & g);
	/**
	 * \brief remove a point from open list 
	 */
	void removeFromOpenList(Grid * g);
	/**
	 * \return if the point exist in close list
	 */
	bool isInCloseList(Grid & g);
	/**
	 * \return Euclidean distance between g1 and g2
	 */
	int calculateEuclideanDistance(Grid & g1, Grid & g2);
	/**
	 * \return Manhattan distance between g1 and g2
	 */
	int calculateManhattanDistance(Grid & g1, Grid & g2);
	/**
	 * \return if the point walkable
	 */
	bool isAvailable(Grid & g);
	/**
	 * \brief in this case, unit is forbidden to move oblique 
	 * \return if point on the corner of something
	 */
	bool checkCorner(Grid & g1, Grid & g2);
	/**
	 * \brief find the next step in surrounded point 
	 */
	void checkSurroundGrid(Grid & g);
public:
	/**
	 * \brief after call serchPath and generatePath
	 * \param map_data grid map, with 1 in point that can be passed
	 * \param x1 start point x
	 * \param y1 start point y
	 * \param x2 end point x
	 * \param y2 end point y
	 */
	PathFinder(std::vector<std::vector<int>> & map_data, int x1, int y1, int x2, int y2);
	/**
	 * \brief search if there exisits path
	 */
	void searchPath();
	/**
	 * \brief generate the path
	 */
	void generatePath();
	/**
	 * \brief 
	 * \return path of grid pinoit 
	 */
	std::vector<GridPoint> getPath();
};

#endif

