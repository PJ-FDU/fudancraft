#include "Unit.h"
#include "AdvancedUnit.h"

USING_NS_CC;

void HPBar::update(float dt)
{
	drawPolygon(frame_points, 4, Color4F(1, 0, 0, 0), 1, Color4F(0, 0, 1, 1));
	bar_points[2].x = frame_points[0].x + (frame_points[3].x - frame_points[0].x) * owner->hp / owner->hp_max;
	bar_points[3].x = bar_points[2].x;
	drawPolygon(bar_points, 4, Color4F(0, 0, 1, 1), 1, Color4F(0, 0, 1, 1));
}

Unit* Unit::create(const std::string& filename)
{
	Unit *ret = new (std::nothrow) Unit();
	if (ret && ret->initWithFile(filename))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);

	return nullptr;
}

void Unit::setProperties()
{
	selected = 0;
	state = 0;
	target_id = -1;

	cd = cd_max;
	hp = hp_max;

	hpbar = HPBar::create();
	hpbar->monitor(this);
}

void Unit::initHPBar()
{
	hpbar = HPBar::create();
	hpbar->monitor(this);
	hpbar->setVisible(false);
	addChild(hpbar);
}

GridPoint Unit::getGridPosition()
{
	if (grid_map)
		return(grid_map->getGridPoint(getPosition()));
	else
		return(GridPoint(0, 0));
}

void Unit::addToMaps(TMXTiledMap* _tiled_map, GridMap* _grid_map)
{
	tiled_map = _tiled_map;
	grid_map = _grid_map;

	_tiled_map->addChild(this);

	_grid_map->occupyPosition(getPosition());
}

void Unit::update(float dt)
{

}

bool UnitManager::init()
{
	return true;
}

void UnitManager::setMessageStack(std::vector<GameMessage>* _msgs)
{
	msgs = _msgs;
}

void UnitManager::setTiledMap(cocos2d::TMXTiledMap* _tiledMap)
{
	tiled_map = _tiledMap;
}
void UnitManager::setGridMap(GridMap* _grid_map)
{
	grid_map = _grid_map;
}

void UnitManager::updateUnitsState()
{
	while (msgs->size())
	{
		GameMessage msg = msgs->back();
		msgs->pop_back();

		int cmd = msg[0].asInt();
		if (cmd == 1)
		{
			float cx = msg[3].asFloat();
			float cy = msg[4].asFloat();
			int camp = msg[5].asInt();
			int unit_type = msg[6].asInt();
			Unit* new_unit = createNewUnit(camp, unit_type, cx, cy);
			id_map.insert(next_id, new_unit);
			next_id++;
		}
		else
			;
	}
}

Unit* UnitManager::createNewUnit(int camp, int unit_type, float cx, float cy)
{
	Unit* nu;
	switch (unit_type)
	{
	case 1:
		nu = Fighter::create("Picture/airplane_red.png");
	default:
		break;
	}

	nu->setProperties();
	nu->setPosition(cx, cy);
	nu->addToMaps(tiled_map, grid_map);

	return(nu);
}