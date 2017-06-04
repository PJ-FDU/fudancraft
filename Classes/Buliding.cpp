#include "Building.h"

USING_NS_CC;

Base* Base::create(const std::string& filename)
{
	Base *ret = new (std::nothrow) Base();
	if (ret && ret->initWithFile(filename))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);

	return nullptr;
}

void Base::setProperties()
{
	atk = 0;
	atk_range = 0;
	atk_period = 0;
	hp_max = 500;
	cd_max = 30;
	move_speed = 0.0f;

	z_index = 5;

	size = GridSize(4, 2);
	period_map[1] = 100;
	period_map[2] = 200;
	period_map[3] = 300;

	cd = 0;
	hp = hp_max;
}

void Base::addToMaps(const GridPoint & crt_gp, cocos2d::TMXTiledMap* _tiled_map, GridMap* _grid_map)
{
	tiled_map = _tiled_map;
	grid_map = _grid_map;

	cur_pos = crt_gp;
	cur_grec = GridRect(cur_pos, size);
	setPosition(grid_map->getGridRectCenter(cur_grec));

	_tiled_map->addChild(this, 1);

	_grid_map->occupyPosition(cur_grec);
}

void Base::update(float f)
{
	if (producing)
	{
		if (++prod_process >= prod_period)
		{
			producing = false;
			prod_process = 0;
			GridPoint free_pos = grid_map->findFreePositionNear(cur_pos);
			unit_manager->genCreateMessage(cur_prod, free_pos);
		}
	}

	if (prod_list.size())
	{
		producing = true;
		prod_process = 0;
		cur_prod = prod_list.back();
		prod_list.pop_back();
		prod_period = period_map.at(cur_prod);
	}
	else
		state = 0;
	
}

void Base::startProduce(int unit_type)
{
	prod_list.insert(prod_list.begin(), 1, unit_type);
	//producing = true;
	//state = 2;
}
