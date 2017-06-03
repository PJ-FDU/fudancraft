#include "Unit.h"
#include "AdvancedUnit.h"
#include <string>


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

void Unit::setGridPath(const MsgGridPath& _msg_grid_path)
{
	int grid_point_size = _msg_grid_path.grid_point_size();
	grid_path = GridPath(grid_point_size);
	for (int i = 0; i < grid_point_size; i++)
		grid_path[i] = GridPoint{ _msg_grid_path.grid_point(i).x(), _msg_grid_path.grid_point(i).y() };
	final_dest = grid_path[0];
	cur_dest = grid_path.back();
	grid_path.pop_back();
}

void Unit::motivate()
{
	moving = true;
}

void Unit::setState(int _state)
{
	state = _state;
}

void Unit::setTarget(int _target_id)
{
	target_id = _target_id;
}

int Unit::getState() const
{
	return(state);
}
bool Unit::updateGridPostion()
{
	if (state == 1)
	{
		GridPoint tmp_gp = getGridPosition();
		/*Point real_pos = getPosition();
		
		if (hasArrivedAtDest() && !grid_path.size())
		{
			log("Unit ID: %d, Final Dest: (%d, %d), Arrived at: (%d, %d), Real Position: (%f, %f)", id, final_dest.x, final_dest.y, tmp_gp.x, tmp_gp.y, real_pos.x, real_pos.y);
			state = 0;
		}*/

		if (cur_pos == tmp_gp)
			return(true);
		if (grid_map->occupyPosition(tmp_gp))
		{
			grid_map->leavePosition(cur_pos);
			cur_pos = tmp_gp;
			return(true);
		}
		else
		{
			state = 0;
			return(false);
		}
	}

	if (state == 2)
	{
		GridPoint tmp_gp = getGridPosition();
		state = 0;
		if (cur_pos == tmp_gp)
			return(true);
		if (grid_map->occupyPosition(tmp_gp))
		{
			grid_map->leavePosition(cur_pos);
			cur_pos = tmp_gp;
			return(true);
		}
		else
		{
			state = 0;
			return(true);
		}
	}

	return(true);
}

void Unit::addToMaps(TMXTiledMap* _tiled_map, GridMap* _grid_map)
{
	tiled_map = _tiled_map;
	grid_map = _grid_map;

	cur_pos = _grid_map->getGridPoint(getPosition());

	_tiled_map->addChild(this, 1);

	_grid_map->occupyPosition(cur_pos);
}

bool Unit::hasArrivedAtDest()
{
	return(grid_map->hasApproached(getPosition(), cur_dest) && getGridPosition() == cur_dest);
}

void Unit::update(float dt)
{
	if (moving)
	{
		auto esp = (grid_map->getPointWithOffset(cur_dest) - getPosition()).getNormalized();
		Point next_pos = getPosition() + esp * move_speed;
		GridPoint next_gp = grid_map->getGridPoint(next_pos);
		
		if (cur_pos == next_gp)
		{
			setPosition(next_pos);
		}
		else
			if (grid_map->occupyPosition(next_gp))
			{
				setPosition(next_pos);
				grid_map->leavePosition(cur_pos);
				cur_pos = next_gp;
			}
			else
			{
				cur_dest = cur_pos;
				if (grid_path.size() && camp == unit_manager->player_id)
				{
					GridPath grid_path = planToMoveTo(final_dest);
					if (grid_path.size())
						unit_manager->msgs->add_game_message()->genGameMessage(GameMessage::CmdCode::GameMessage_CmdCode_UDP, id, 0, 0, camp, 0, grid_path);
					else
						unit_manager->msgs->add_game_message()->genGameMessage(GameMessage::CmdCode::GameMessage_CmdCode_RFP, id, 0, 0, camp, 0, { final_dest });
				
				}
			}			

		if (hasArrivedAtDest())
			if (grid_path.size())
			{
				cur_dest = grid_path.back();
				grid_path.pop_back();
			}
			else
			{
				moving = false;
			}
	}

	if (state == 2)
	{
		GridPoint target_pos = unit_manager->getUnitPosition(target_id);
		Point target_fp = grid_map->getPointWithOffset(target_pos);
		Point cur_fp = getPosition();
		Vec2 dist_vec = target_fp - cur_fp;
		if (target_pos == GridPoint(-1, -1))
			state = 0;
		else
			if ((dist_vec).length() < atk_range)
			{
				moving = false;
				if (!cd)
				{
					unit_manager->msgs->add_game_message()->genGameMessage(GameMessage::CmdCode::GameMessage_CmdCode_ATK, id, target_id, atk, camp, 0, {});
					cd = cd_max;
				}
				else
					cd--;
			}
			else
				if (!(target_pos == final_dest))
				{
					final_dest = target_pos;
					rfp_cnt--;
					unit_manager->msgs->add_game_message()->genGameMessage(GameMessage::CmdCode::GameMessage_CmdCode_RFP, id, 0, 0, camp, 0, { final_dest });
				}
	}
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

void UnitManager::setSocketClient(SocketClient* _socket_client)
{
	socket_client = _socket_client;
}

void UnitManager::setPlayerID(int _player_id)
{
	player_id = _player_id;
	next_id = _player_id;
}

GridPoint UnitManager::getUnitPosition(int _unit_id)
{
	Unit* unit = id_map.at(_unit_id);
	if (unit)
		return(unit->getGridPosition());
	else
		return{-1, -1};
}

void UnitManager::updateUnitsState()
{

	/*for (const auto& id : id_map.keys())
		if (!id_map.at(id)->updateGridPostion())
		{
			Unit* unit = id_map.at(id);
			GridPath grid_path = unit->planToMoveTo(unit->final_dest);
			msgs->add_game_message()->genGameMessage(GameMessage::CmdCode::GameMessage_CmdCode_MOV, id, 0, 0, player_id, 1, grid_path);
		}*/

	socket_client->send_string(msgs->SerializeAsString());
	std::string msg_str = socket_client->get_string();
	msgs = new GameMessageSet();
	msgs->ParseFromString(msg_str);

	GameMessageSet* new_msgs = new GameMessageSet();

	for (int i = 0; i < msgs->game_message_size(); i++)
	{
		const GameMessage&  msg = msgs->game_message(i);

		if (msg.cmd_code() == GameMessage::CmdCode::GameMessage_CmdCode_CRT)
		{

			int id = msg.unit_0();
			int camp = msg.camp();
			int unit_type = msg.unit_type();
			Unit* new_unit = createNewUnit(id, camp, unit_type, GridPoint(msg.grid_path().grid_point(0).x(), msg.grid_path().grid_point(0).y()));
			id_map.insert(id, new_unit);
		}
		else
		if (msg.cmd_code() == GameMessage::CmdCode::GameMessage_CmdCode_MOV)
		{
			log("Unit ID: %d, Next Point(%d, %d)", msg.unit_0(), msg.grid_path().grid_point(0).x(), msg.grid_path().grid_point(0).y());
			Unit* u0 = id_map.at(msg.unit_0());
			if (u0)
			{
				u0->rfp_cnt = 0;
				u0->setGridPath(msg.grid_path());
				u0->setState(1);

				u0->motivate();

			}
		}
		else
		if (msg.cmd_code() == GameMessage::CmdCode::GameMessage_CmdCode_TRC)
		{
			if (!msg.grid_path().grid_point_size())
				continue;
			Unit* u0 = id_map.at(msg.unit_0());
			if (u0)
			{
				u0->setGridPath(msg.grid_path());
				u0->setState(2);
				u0->setTarget(msg.unit_1());
				u0->motivate();
			}
		}
		else
		if (msg.cmd_code() == GameMessage::CmdCode::GameMessage_CmdCode_RFP)
		{
			int id = msg.unit_0();
			Unit* unit = id_map.at(id);
			if (!unit)
				continue;
			unit->rfp_cnt++;
			if (unit->rfp_cnt >= MAX_PATH_FIND_TIMES)
			{
				unit->rfp_cnt = 0;
				continue;
			}
			GridPoint grid_dest{ msg.grid_path().grid_point(0).x(), msg.grid_path().grid_point(0).y() };
			log("Unit id: %d, Refind Path to: (%d, %d), Times: %d", id, grid_dest.x, grid_dest.y, unit->rfp_cnt);
			GridPath grid_path = unit->planToMoveTo(grid_dest);	//锟街诧拷锟斤拷锟斤拷锟斤拷锟?
			if (grid_path.size())
				msgs->add_game_message()->genGameMessage(GameMessage::CmdCode::GameMessage_CmdCode_UDP, id, 0, 0, player_id, 0, grid_path);

			else
				new_msgs->add_game_message()->genGameMessage(GameMessage::CmdCode::GameMessage_CmdCode_RFP, id, 0, 0, player_id, 0, { grid_dest });
		}
		else
		if (msg.cmd_code() == GameMessage::CmdCode::GameMessage_CmdCode_UDP)
		{
			Unit* u0 = id_map.at(msg.unit_0());
			if (u0)
			{
				u0->rfp_cnt = 0;
				u0->setGridPath(msg.grid_path());
				u0->motivate();
			}
		}
		else
		if (msg.cmd_code() == GameMessage::CmdCode::GameMessage_CmdCode_ATK)
		{
			int unitid_0 = msg.unit_0();
			int unitid_1 = msg.unit_1();
			int damage = msg.damage();
			log("Attack! Unit %d -> Unit %d, Damage %d", unitid_0, unitid_1, damage);
			Unit* unit_1 = id_map.at(unitid_1);
			if (unit_1)
			{

			}
		}
	}
	delete msgs;
	msgs = new_msgs;
}

Unit* UnitManager::createNewUnit(int id, int camp, int unit_type, GridPoint crt_gp)
{
	Unit* nu;
	switch (unit_type)
	{
	case 1:
		nu = Fighter::create("Picture/airplane_red.png");
	default:
		break;
	}

	nu->id = id;
	nu->camp = camp;
	nu->setProperties();
	nu->setPosition(grid_map->getPoint(crt_gp));
	nu->initHPBar();
	nu->setAnchorPoint(Vec2(0.5, 0.5));
	nu->addToMaps(tiled_map, grid_map);
	nu->unit_manager = this;
	nu->schedule(schedule_selector(Unit::update));

	return(nu);
}


//生成新单位测试程序
void UnitManager::genCreateMessage()
{
	GridPoint init_gp = getUnitPosition(player_id);
	auto new_msg = msgs->add_game_message();
	new_msg->genGameMessage(GameMessage::CmdCode::GameMessage_CmdCode_CRT, next_id, 0, 0, player_id, 1, GridPath{ init_gp });
	next_id += MAX_PLAYER_NUM;
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
		int type = dict["type"].asInt();
		GridPoint init_gp = grid_map->getGridPoint({ cx, cy });

			//GameMessage的格式、初始化方法、解释方法有待进一步探讨
		if (camp == player_id)
		{
			auto new_msg = msgs->add_game_message();

			new_msg->genGameMessage(GameMessage::CmdCode::GameMessage_CmdCode_CRT, next_id, 0, 0, player_id, type, GridPath{ init_gp });
			next_id += MAX_PLAYER_NUM;

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
			if (/*id_unit.second->camp != player_id &&*/ id_unit.second->getBoundingBox().containsPoint(select_point))
			{
				for (auto & id : selected_ids)
				{
					log("Unit ID: %d, tracing enemy id: %d", id, id_unit.second->id);
					Unit* unit = id_map.at(id);
					GridPath grid_path = unit->planToMoveTo(id_unit.second->getGridPosition());
					msgs->add_game_message()->genGameMessage(GameMessage::CmdCode::GameMessage_CmdCode_TRC, id, id_unit.second->id, 0, player_id, 0, grid_path);
				}
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

			GridPoint grid_dest = grid_map->getGridPointWithOffset(select_point);
			GridPath grid_path = unit->planToMoveTo(grid_dest);	//锟街诧拷锟斤拷锟斤拷锟斤拷锟?
			if (grid_path.size())
				msgs->add_game_message()->genGameMessage(GameMessage::CmdCode::GameMessage_CmdCode_MOV, id, 0, 0, player_id, 0, grid_path);
			else
				msgs->add_game_message()->genGameMessage(GameMessage::CmdCode::GameMessage_CmdCode_RFP, id, 0, 0, player_id, 0, {grid_dest});

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