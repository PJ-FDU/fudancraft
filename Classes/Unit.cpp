#include "Unit.h"

USING_NS_CC;
int Unit::total_number = 0;

Unit* Unit::create(const std::string& filename)
{
	Unit *ret = new (std::nothrow) Unit();
	if (ret && ret->initWithFile(filename))
	{
		ret->id = ++total_number;
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);

	return nullptr;
}

void Unit::initProperties()
{
	selected = 0;
	state = 0;
	target_id = -1;

	cd = cd_max;
	hp = hp_max;

	hpbar = HPBar::create();
	hpbar->monitor(this);
}

GridPoint Unit::getGridPosition()
{
	if (grid_map)
		return(grid_map->getGridPoint(getPosition()));
	else
		return(GridPoint(0, 0));
}

bool Unit::update()
{
	if (selected)
		if (hpbar)
		{
			hpbar->clear();
			hpbar->update();
		}
		else
		{
			hpbar = HPBar::create();
			hpbar->monitor(this);
			addChild(hpbar, 10);//make it high level
		}

	if (state == 1)
		if ((dest - getPosition()).getLength() < 10)
		{
			active = 0;
			state = 0;
		}
		else
		{
			auto esp = (dest - getPosition()).getNormalized();
			setPosition(getPosition() + esp * move_speed);
		}
	if (state == 2)
		if ((getPosition() - target->getPosition()).getLength() < atk_range)
			state = 3;
		else
		{
			auto esp = (target->getPosition() - getPosition()).getNormalized();
			setPosition(getPosition() + esp * move_speed);
		}
	if (state == 3)
		if (target->isAlive())
			if (!cd)
			{
				target->decreaseHp(atk);
				cd = atk_period;
			}
			else
				cd--;
		else
		{
			state = 0;
			target = nullptr;
			active = 0;
		}

	if (alive && hp <= 0)
	{
		alive = false;
		return true;
	}

	return false;
}

void Unit::addToMaps(TMXTiledMap* _tiled_map, GridMap* _grid_map)
{
	tiled_map = _tiled_map;
	grid_map = _grid_map;

	_tiled_map->addChild(this);

	_grid_map->occupyPosition(getPosition());
}

