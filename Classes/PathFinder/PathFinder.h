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
public:
	PathFinder(std::vector<std::vector<int>> & map_data, int x1, int y1, int x2, int y2);
	Grid * selectNextGrid();
	bool isInOpenList(Grid & g);
	void removeFromOpenList(Grid * g);
	bool isInCloseList(Grid & g);
	int calculateEuclideanDistance(Grid & g1, Grid & g2);
	int calculateManhattanDistance(Grid & g1, Grid & g2);
	bool isAvailable(Grid & g);
	bool checkCorner(Grid & g1, Grid & g2);
	void checkSurroundGrid(Grid & g);
	void searchPath();
	void generatePath();
	std::vector<GridPoint> getPath();
};

#endif

