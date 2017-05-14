#pragma once

#include "cocos2d.h"
#include "GridMap.h"

class GameMessage : public std::vector<int>
{
public:
	GameMessage(const std::initializer_list<int> init_list) : std::vector<int>{init_list} {}

	void genMoveMessage(int unit0_id, GridPath grid_path)
	{
		push_back(2);
		push_back(unit0_id);
		push_back(0);
		for (auto gp : grid_path)
		{
			push_back(gp.x);
			push_back(gp.y);
		}
	}
};

