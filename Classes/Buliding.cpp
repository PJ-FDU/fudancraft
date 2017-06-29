#include "Building.h"
#include "SimpleAudioEngine.h"

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
	type = 5;
	atk = 0;
	atk_range = 0;
	hp_max = 800;
	cd_max = 30;
	move_speed = 0.0f;

	z_index = 5;

	mobile = false;

	vision_range = GridSize(11, 11);
	size = GridSize(2, 2);
	period_map[1] = 300;
	period_map[2] = 200;
	period_map[3] = 80;

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

	_grid_map->occupyPosition(id, cur_grec);
	if (camp == unit_manager->player_id)
		grid_map->clearFog(GridRect(cur_pos, vision_range, true));
}

void Base::update(float f)
{
	if (state == 1)
	{
		if (++prod_process >= prod_period)
		{
			state = 2;
			prod_process = 0;
			GridPoint free_pos = grid_map->findFreePositionNear(cur_pos);
			unit_manager->genCreateMessage(cur_prod, free_pos);
			prod_bar->setVisible(false);
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("audio/unitready.wav");

		}
		else
			displayProdBar();
	}

	if (state == 2)
		if (prod_list.size())
		{
			state = 1;
			prod_process = 0;
			cur_prod = prod_list.back();
			prod_list.pop_back();
			prod_period = period_map.at(cur_prod);
		}
		else
			state = 0;
	
}

void Base::setState(int _state)
{
}

void Base::initBars()
{
	float unit_width = size.width * tiled_map->getTileSize().width;
	float unit_height = size.height * tiled_map->getTileSize().height;

	hpbar = Bar::create();
	hpbar->setLength(unit_width);
	hpbar->setVisible(false);
	addChild(hpbar, 20);
	hpbar->setPosition(Point(0, unit_height + 5));

	prod_bar = Bar::create();
	prod_bar->setLength(unit_width);
	prod_bar->setColor(Color4F(0, 0, 0.8, 0.8));
	prod_bar->setVisible(false);
	addChild(prod_bar, 20);
	prod_bar->setPosition(Point(0, unit_height + 13));
}

void Base::displayProdBar()
{
	if (prod_bar)
		prod_bar->updateBarDisplay(float(prod_process) / float(prod_period));
}

void Base::startProduce(int unit_type)
{
	prod_list.insert(prod_list.begin(), unit_type);
	if (state != 1)
		state = 2;
}


Tower* Tower::create(const std::string& filename)
{
	Tower *ret = new (std::nothrow) Tower();
	if (ret && ret->initWithFile(filename))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);

	return nullptr;
}

void Tower::update(float f)
{
	if (camp == unit_manager->player_id)
	{
		if (auto_atking)
			auto_atk();
		else
			if (timer % auto_atk_freq == 0)
				searchForNearbyEnemy();
	}
}

void Tower::setState(int _state)
{
}

void Tower::setProperties()
{
	type = 6;
	atk = 15;
	atk_range = 300;
	hp_max = 300;
	cd_max = 20;
	move_speed = 0.0f;

	z_index = 10;

	mobile = false;

	auto_atk_freq = 50;
	auto_atk_range = GridSize(11, 11);

	size = GridSize(1, 1);
	vision_range = GridSize(11, 11);

	cd = 0;
	hp = hp_max;
}

