#include "Building.h"

USING_NS_CC;

Building* Building::create(const std::string& filename)
{
	Building *ret = new (std::nothrow) Building();
	if (ret && ret->initWithFile(filename))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);

	return nullptr;
}

void Building::setProperties()
{
	atk = 0;
	atk_range = 0;
	atk_period = 0;
	hp_max = 500;
	cd_max = 30;
	move_speed = 0.0f;

	size = GridSize(4, 2);

	cd = 0;
	hp = hp_max;
}

void Building::addToMaps(const GridPoint & crt_gp, cocos2d::TMXTiledMap* _tiled_map, GridMap* _grid_map)
{
	tiled_map = _tiled_map;
	grid_map = _grid_map;

	cur_pos = crt_gp;
	cur_grec = GridRect(cur_pos, size);
	setPosition(grid_map->getGridRectCenter(cur_grec));

	_tiled_map->addChild(this, 1);

	_grid_map->occupyPosition(cur_grec);
}

void Building::update(float f)
{

}