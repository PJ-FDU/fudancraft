#pragma once

#include "cocos2d.h"
#include "GridMap.h"

class GameMessage //: public std::vector<int>
{
public:
	enum CmdCode
	{
		CRT = 0,
		MOV = 1,
		ATK = 2
	};

	CmdCode cmd_code;
	int unit_0;
	int unit_1;

	int damage;

	GridPath grid_path;
	int camp;
	int unit_type;

	//GameMessage(const std::initializer_list<int> init_list) : std::vector<int>{init_list} {}
	GameMessage(CmdCode _cmd_code, int _unit_0, int _unit_1, int _damage, int _camp, int _unit_type, const GridPath& _grid_path) :
		cmd_code{ _cmd_code }, unit_0{ _unit_0 }, unit_1{ _unit_1 }, damage{ _damage }, camp{ _camp }, unit_type{ _unit_type }, grid_path{ _grid_path }
	{}
};

