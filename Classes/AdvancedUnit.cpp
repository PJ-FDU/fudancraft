#include "AdvancedUnit.h"

USING_NS_CC;

Fighter* Fighter::create(const std::string& filename)
{
	Fighter *ret = new (std::nothrow) Fighter();
	if (ret && ret->initWithFile(filename))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);

	return nullptr;
}

void Fighter::motivate()
{
	moving = true;
	grid_map->leavePosition(cur_pos, true);
}

void Fighter::setProperties()
{
	type = 1;

	atk = 8;
	atk_range = 100;
  
	hp_max = 100;

	cd_max = 20;

	move_speed = 4.0f;

	mobile = true;

	auto_atk_freq = 80;
	auto_atk_range = GridSize(5, 5);

	z_index = 15;

	size = GridSize(1, 1);

	cd = 0;
	hp = hp_max;
}

void Fighter::move()
{
	auto esp = (grid_map->getPointWithOffset(cur_dest) - getPosition()).getNormalized();
	Point next_pos = getPosition() + esp * move_speed;
	GridPoint next_gp = grid_map->getGridPoint(next_pos);

	if (cur_pos == next_gp)
	{
		setPosition(next_pos);
	}
	else
	{
		grid_map->occupyPosition(id, next_gp, false);
		roc_cnt = 0;
		setPosition(next_pos);
		grid_map->leavePosition(cur_pos, false);
		cur_pos = next_gp;
	}		

	if (hasArrivedAtDest())
		if (grid_path.size())
		{
			cur_dest = grid_path.back();
			grid_path.pop_back();
		}
		else
		{
			if (grid_map->checkPosition(cur_dest))
			{
				log("Unit ID: %d, fighter landing", id);
				grid_map->occupyPosition(id, next_gp, true);
				moving = false;
			}
			else
			{
				log("Unit ID: %d, fighter failed to land", id);
				cur_dest = grid_map->findFreePositionNear(cur_dest);
				final_dest = cur_dest;
			}
		}
}

GridPath Fighter::findPath(const GridPoint & dest) const
{
	return GridPath{dest};
}

Tank* Tank::create(const std::string& filename)
{
	Tank *ret = new (std::nothrow) Tank();
	if (ret && ret->initWithFile(filename))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);

	return nullptr;
}

void Tank::setProperties()
{
	type = 2;
	atk = 50;
	atk_range = 100;

	hp_max = 200;

	cd_max = 30;
	move_speed = 2.0f;

	z_index = 10;

	mobile = true;

	auto_atk_freq = 80;
	auto_atk_range = GridSize(3, 3);

	size = GridSize(1, 1);

	cd = 0;
	hp = hp_max;
}

Soldier* Soldier::create(const std::string& filename)
{
	Soldier *ret = new (std::nothrow) Soldier();
	if (ret && ret->initWithFile(filename))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);

	return nullptr;
}

void Soldier::setProperties()
{
	type = 3;
	atk = 5;
	atk_range = 50;

	hp_max = 80;
	cd_max = 5;
	move_speed = 1.5f;

	z_index = 10;

	mobile = true;

	auto_atk_freq = 80;
	auto_atk_range = GridSize(3, 3);

	size = GridSize(1, 1);

	cd = 0;
	hp = hp_max;
}