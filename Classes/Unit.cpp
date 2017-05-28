#include "Unit.h"
#include "AdvancedUnit.h"

USING_NS_CC;

void HPBar::update(float dt)
{
	clear();
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

}

void Unit::initHPBar()
{
	hpbar = HPBar::create();
	hpbar->monitor(this);
	hpbar->setVisible(false);
	addChild(hpbar);
}

void Unit::displayHPBar()
{
	if (hpbar)
	{
		hpbar->scheduleUpdate();
		hpbar->setVisible(true);
	}
}

void Unit::hideHPBar()
{
	if (hpbar)
	{
		hpbar->unscheduleUpdate();
		hpbar->setVisible(false);
	}
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

	_tiled_map->addChild(this, 1);

	_grid_map->occupyPosition(getPosition());
}

void Unit::update(float dt)
{

}

bool UnitManager::init()
{
	return true;
}

void UnitManager::setMessageSet(GameMessageSet* _msgs)
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

void UnitManager::setPlayerID(int _player_id)
{
	player_id = _player_id;
}

void UnitManager::updateUnitsState()
{
	for (int i = 0; i < msgs->game_message_size(); i++)
	{
		GameMessage msg = msgs->game_message(i);

		if (msg.cmd_code() == GameMessage::CmdCode::GameMessage_CmdCode_CRT)
		{
			int camp = msg.camp();
			int unit_type = msg.unit_type();
			Unit* new_unit = createNewUnit(camp, unit_type, GridPoint(msg.grid_path().grid_point(0).x(), msg.grid_path().grid_point(0).y()));
			id_map.insert(next_id, new_unit);
			next_id++;
		}
		else
			if (msg.cmd_code() == GameMessage::CmdCode::GameMessage_CmdCode_MOV)
			{
				log("Unit ID: %d, Next Point(%d, %d)", msg.unit_0(), msg.grid_path().grid_point(0).x(), msg.grid_path().grid_point(0).y());

			}
	}
	msgs->clear_game_message();
}

Unit* UnitManager::createNewUnit(int camp, int unit_type, GridPoint crt_gp)
{
	Unit* nu;
	switch (unit_type)
	{
	case 1:
		nu = Fighter::create("Picture/airplane_red.png");
	default:
		break;
	}

	nu->camp = camp;
	nu->setProperties();
	nu->setPosition(grid_map->getPoint(crt_gp));
	nu->initHPBar();

	nu->addToMaps(tiled_map, grid_map);

	return(nu);
}

void UnitManager::initiallyCreateUnits()
{
	auto* obj_group = tiled_map->getObjectGroup("init_unit");
	auto& objs = obj_group->getObjects();

	for (auto& obj : objs)
	{
		auto& dict = obj.asValueMap();
		float cx = dict["x"].asFloat();
		float cy = dict["y"].asFloat();
		int camp = dict["camp"].asInt();
		GridPoint init_gp = grid_map->getGridPoint({ cx, cy });

		if (camp == player_id)
			//GameMessage的格式、初始化方法、解释方法有待进一步探讨
		{
			auto new_msg = msgs->add_game_message();
			new_msg->genGameMessage(GameMessage::CmdCode::GameMessage_CmdCode_CRT, next_id, 0, 0, player_id, 1, GridPath{ init_gp });
		}
	}
}

void UnitManager::deselectAllUnits()
{
	for (auto& id : selected_ids)
		id_map.at(id)->hideHPBar();
	selected_ids.clear();
}

void UnitManager::selectUnits(Point select_point)
{
	if (selected_ids.size())
	{
		for (auto & id_unit : id_map)
			if (id_unit.second->camp != player_id && id_unit.second->getBoundingBox().containsPoint(select_point))
			{
				for (auto & id : selected_ids)
					log("Unit ID: %d, tracing enemy id:", id, id_unit.second->id);
				return;
			}
		for (auto & id_unit : id_map)
			if (id_unit.second->camp == player_id && id_unit.second->getBoundingBox().containsPoint(select_point))
			{
				deselectAllUnits();
				selected_ids.push_back(id_unit.first);
				id_unit.second->displayHPBar();
				return;
			}
		for (auto & id : selected_ids)
		{
			log("Unit ID: %d, plan to move to:(%f, %f)", id, select_point.x, select_point.y);
			Unit* unit = id_map.at(id);
			GridPath grid_path = unit->planToMoveTo(grid_map->getGridPoint(select_point));
			msgs->add_game_message()->genGameMessage(GameMessage::CmdCode::GameMessage_CmdCode_MOV, next_id, 0, 0, player_id, 1, grid_path);
		}
		return;
	}
	else
		for (auto & id_unit : id_map)
			if (id_unit.second->camp == player_id && id_unit.second->getBoundingBox().containsPoint(select_point))
			{
				deselectAllUnits();
				selected_ids.push_back(id_unit.first);
				id_unit.second->displayHPBar();
				return;
			}

	return;
}

void UnitManager::selectUnits(Rect select_rect)
{
	deselectAllUnits();
	for (auto & id_unit : id_map)
		if (id_unit.second->camp == player_id && select_rect.containsPoint(id_unit.second->getPosition()))
		{
			selected_ids.push_back(id_unit.first);
			id_unit.second->displayHPBar();
		}
	return;
}