#include "PathFinder.h"

Grid::Grid()
{
	this->m_flag = UNOCCUPIED;
	this->m_x = 0;
	this->m_y = 0;
	this->m_g = 0;
	this->m_h = 0;
	this->m_f = 0;
	this->m_p_parent = nullptr;
}

int Grid::getFlag()
{
	return this->m_flag;
}

void Grid::setFlag(int flag)
{
	this->m_flag = flag;
}

int Grid::getX()
{
	return this->m_x;
}

void Grid::setX(int x)
{
	this->m_x = x;
}

int Grid::getY()
{
	return this->m_y;
}

void Grid::setY(int y)
{
	this->m_y = y;
}

int Grid::getG()
{
	return this->m_g;
}

void Grid::setG(int g)
{
	this->m_g = g;
}

int Grid::getH()
{
	return this->m_h;
}

void Grid::setH(int h)
{
	this->m_h = h;
}

int Grid::getF()
{
	return this->m_f;
}

void Grid::setF(int f)
{
	this->m_f = f;
}

Grid * Grid::getParent()
{
	return this->m_p_parent;
}

void Grid::setParent(Grid * parent)
{
	this->m_p_parent = parent;
}

PathFinder::PathFinder(std::vector<std::vector<int>> & map_data, int x1, int y1, int x2, int y2)
{
	m_width = map_data.size();
	m_height = map_data[0].size();
	std::vector<Grid> col(m_height, Grid());
	m_map_grid.assign(m_width, col);
	for (auto i = 0; i < m_width; ++i)
		for (auto j = 0; j < m_height; ++j)
		{
			m_map_grid[i][j].setX(i);
			m_map_grid[i][j].setY(j);
			if (map_data[i][j] == 0)
			{
				m_map_grid[i][j].setFlag(UNOCCUPIED);
			}
			else
			{
				m_map_grid[i][j].setFlag(OCCUPIED);
			}
		}
	m_map_grid[x1][y1].setFlag(START);
	m_map_grid[x2][y2].setFlag(TERMINAL);
	start_point = &m_map_grid[x1][y1];
	terminal_point = &m_map_grid[x2][y2];
}

Grid * PathFinder::selectNextGrid()
{
	Grid * next_grid = open_list[0];
	for (auto p : open_list)
	{
		if (p->getF() < next_grid->getF())
		{
			next_grid = p;
		}
	}
	return next_grid;
}

bool PathFinder::isInOpenList(Grid & g)
{
	if (g.getFlag() == INOPEN)
	{
		return true;
	}
	return false;
}

void PathFinder::removeFromOpenList(Grid * g)
{
	std::vector<Grid *>::iterator iter = find(open_list.begin(), open_list.end(), g);
	if (iter != open_list.end())
	{
		open_list.erase(iter);
	}
}

bool PathFinder::isInCloseList(Grid & g)
{
	if (g.getFlag() == INCLOSE || g.getFlag() == START)
	{
		return true;
	}
	return false;
}

int PathFinder::calculateEuclideanDistance(Grid & g1, Grid & g2)
{
	if (g1.getX() == g2.getX() || g1.getY() == g2.getY())
	{
		return g2.getG() + 10;
	}
	return g2.getG() + 14;
}

int PathFinder::calculateManhattanDistance(Grid & g1, Grid & g2)
{
	return (abs(g1.getX() - g2.getX()) + abs(g1.getY() - g2.getY())) * DISTANCE;
}

bool PathFinder::isAvailable(Grid & g)
{
	if (isInCloseList(g))
	{
		return false;
	}
	else if (g.getFlag() == OCCUPIED)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool PathFinder::checkCorner(Grid & g1, Grid & g2)
{
	if (g1.getX() == g2.getX() || g1.getY() == g2.getY())
	{
		return true;
	}
	else if (m_map_grid[g1.getX()][g2.getY()].getFlag() == OCCUPIED
		|| m_map_grid[g2.getX()][g1.getY()].getFlag() == OCCUPIED)
	{
		return false;
	}
	else return true;
}

void PathFinder::checkSurroundGrid(Grid & g)
{
	for (auto i = 0; i < 8; i++) {
		auto current_x = g.getX() + DIRECTION[i][0];
		auto current_y = g.getY() + DIRECTION[i][1];
		if (current_x >= 0 && current_x < m_width && current_y >= 0 && current_y < m_height 
			&& checkCorner(g, m_map_grid[current_x][current_y]) 
			&& isAvailable(m_map_grid[current_x][current_y]))
		{
			m_map_grid[current_x][current_y].setG(calculateEuclideanDistance(m_map_grid[current_x][current_y], g));
			m_map_grid[current_x][current_y].setH(calculateManhattanDistance(m_map_grid[current_x][current_y], *terminal_point));
			m_map_grid[current_x][current_y].setF(m_map_grid[current_x][current_y].getG() + m_map_grid[current_x][current_y].getH());
			if (isInOpenList(m_map_grid[current_x][current_y]))
			{
				if (calculateEuclideanDistance(m_map_grid[current_x][current_y], g) < m_map_grid[current_x][current_y].getG()) 
				{
					m_map_grid[current_x][current_y].setG(calculateEuclideanDistance(m_map_grid[current_x][current_y], g));
					m_map_grid[current_x][current_y].setF(m_map_grid[current_x][current_y].getG() + m_map_grid[current_x][current_y].getH());
					m_map_grid[current_x][current_y].setParent(&g);
				}
			}
			else 
			{
				m_map_grid[current_x][current_y].setFlag(INOPEN);
				m_map_grid[current_x][current_y].setParent(&g);
				open_list.push_back(&m_map_grid[current_x][current_y]);
			}
		}
	}
}

void PathFinder::searchPath()
{
	Grid * current_grid = nullptr;
	open_list.push_back(start_point);
	while (terminal_point->getFlag() != INCLOSE && !open_list.empty())
	{
		current_grid = selectNextGrid();
		checkSurroundGrid(*current_grid);
		current_grid->setFlag(INCLOSE);
		close_list.push_back(current_grid);
		removeFromOpenList(current_grid);
	}
}

void PathFinder::generatePath()
{
	Grid * g = terminal_point;
	while (g->getParent() != nullptr) {
		path.push_back(GridPoint(g->getX(), g->getY()));
		g = g->getParent();
	}
}

std::vector<GridPoint> PathFinder::getPath()
{
	return path;
}


