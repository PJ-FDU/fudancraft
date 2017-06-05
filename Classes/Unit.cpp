#include "Unit.h"
#include "AdvancedUnit.h"
#include "Building.h"
#include <string>


USING_NS_CC;

void HPBar::update(float dt)
{
	if (!owner)
		return;
	clear();
	/*drawRect(frame_points, 4, Color4F(1, 0, 0, 0), 1, Color4F(0, 0, 1, 1));
	bar_points[2].x = frame_points[0].x + (frame_points[3].x - frame_points[0].x) * owner->hp / owner->hp_max;
	bar_points[3].x = bar_points[2].x;
	drawPolygon(bar_points, 4, Color4F(0, 0, 1, 1), 1, Color4F(0, 0, 1, 1));*/
	drawRect(Point(0, 0), Point(length, height), Color4F(0, 0, 1, 1));
	Point endpoint{ length * owner->hp / owner->hp_max, height };
	drawSolidRect(Point(0, 0), endpoint, Color4F(0, 0, 1, 1));
}

void HPBar::setLength(float _length)
{
	length = _length;
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
	float unit_width = size.width * tiled_map->getTileSize().width;
	float unit_height = size.height * tiled_map->getTileSize().height;
	hpbar->setLength(unit_width);
	hpbar->monitor(this);
	hpbar->setVisible(false);
	addChild(hpbar, 20);
	hpbar->setPosition(Point(0, unit_height + 5));
}

void Unit::initFlag()
{
	flag = DrawNode::create();
	flag->drawSolidRect(Point(0, 0), Point(5, 5), getCampColor());
	addChild(flag);
}

cocos2d::Color4F Unit::getCampColor()
{
	std::vector<Color4F> camp_color_list = { { 0, 0, 0, 0 },{ 1, 0, 0, 1 },{ 0, 1, 0, 1 },{ 0, 0, 1, 1 } };
	return camp_color_list[camp % 4];
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

void Unit::setDestination(const GridPoint& grid_dest)
{
	final_dest = grid_dest;
}

void Unit::setTarget(int _target_id)
{
	target_lastpos = unit_manager->getUnitPosition(_target_id);
	target_id = _target_id;
}

int Unit::getState() const
{
	return(state);
}

bool Unit::underAttack(int damage)
{
	hp -= damage;
	if (hp < 0)
		return(true);
	else
		return(false);
}


bool Unit::isMobile()
{
	return mobile;
}

bool TrajectoryEffect::init()
{
	if(!ParticleFire::init())
		return false;
	setScale(0.1);
//	setPosition(Vec2(400,20));
//	log("fire start position:%f,%f", getPosition().x, getPosition().y);
	setPositionType(PositionType::RELATIVE);
	return true;
}

/**
 * \brief set the path and start move
 * \param from sender's position
 * \param to target's position
 */
void TrajectoryEffect::setPath(cocos2d::Vec2 from, cocos2d::Vec2 to)
{
	from_ = from;
	to_ = to;
	setPosition(from_);
//	log("fire start position:%f,%f", getPosition().x, getPosition().y);
//	log("start position:%f,%f", from.x, from.y);
	move_ =(to_-from_).getNormalized()*speed_;
	schedule(schedule_selector(TrajectoryEffect::updatefire));
}

void TrajectoryEffect::updatefire(float)
{
//	log("fire position:%f,%f", getPosition().x, getPosition().y);
	if (((abs(getPosition().x-to_.x)<speed_ )&& (abs(getPosition().y - to_.y)<speed_)))
		removeFromParent();
	else
		setPosition(getPosition() + move_);
}

bool ExplosionEffect::init()
{
	if (!ParticleFire::init())
		return false;
	setScale(0.0000008);
	setDuration(1);
	auto action = ScaleBy::create(0.5, 500000);
	runAction(action);
	scheduleOnce(schedule_selector(ExplosionEffect::remove), 1.2);	
	setPositionType(PositionType::RELATIVE);
	return true;
}

void ExplosionEffect::remove(float f)
{
	removeFromParent();
}

void Unit::addToMaps(const GridPoint & crt_gp, TMXTiledMap* _tiled_map, GridMap* _grid_map)
{
	tiled_map = _tiled_map;
	grid_map = _grid_map;

	cur_pos = crt_gp;
	setPosition(grid_map->getPointWithOffset(crt_gp));

	_tiled_map->addChild(this, z_index);

	_grid_map->occupyPosition(cur_pos);
}

void Unit::removeFromMaps()
{
	// add explosion effect
	auto explosion_effect = ExplosionEffect::create();
	explosion_effect->setPosition(this->getPosition());
	getParent()->addChild(explosion_effect,20);
	grid_map->leavePosition(cur_pos);
	tiled_map->removeChild(this,1);
}

bool Unit::hasArrivedAtDest()
{
	return(grid_map->hasApproached(getPosition(), cur_dest) && getGridPosition() == cur_dest);
}

void Unit::move()
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
			roc_cnt = 0;
			setPosition(next_pos);
			grid_map->leavePosition(cur_pos);
			cur_pos = next_gp;
		}
		else
		{
			/*if (roc_cnt < MAX_REOCCUPY_TIMES)
			{
			grid_path.push_back(cur_dest);
			roc_cnt++;
			}*/
			cur_dest = cur_pos;

			Point final_fp = grid_map->getPointWithOffset(final_dest);

			if (camp == unit_manager->player_id && (final_fp - getPosition()).length() > DISREFINDPATH_RANGE)
			{
				tryToFindPath();
			}

			/*if (grid_path.size() && camp == unit_manager->player_id)
			{
			GridPath grid_path = planToMoveTo(final_dest);
			if (grid_path.size())
			unit_manager->msgs->add_game_message()->genGameMessage(GameMessage::CmdCode::GameMessage_CmdCode_UDP, id, 0, 0, camp, 0, grid_path);
			else
			unit_manager->msgs->add_game_message()->genGameMessage(GameMessage::CmdCode::GameMessage_CmdCode_RFP, id, 0, 0, camp, 0, { final_dest });

			}*/
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

void Unit::stall()
{
	if (stl_cnt > 0)
		--stl_cnt;
	else
		if (camp == unit_manager->player_id)
			tryToFindPath();
}

void Unit::tryToFindPath()
{
	if (camp != unit_manager->player_id)
		return;

	if (!grid_map->checkPosition(final_dest))
	{
		final_dest = grid_map->findFreePositionNear(final_dest);

		log("Change Destination due to occupied: -> (%d, %d)", final_dest.x, final_dest.y);

	}
	GridPath grid_path = findPath(final_dest);
	if (grid_path.size())
	{
		log("Unit %d, Success FP, RFP: %d", id, rfp_cnt);
		rfp_cnt = 0;
		stl_cnt = -1;
		unit_manager->msgs->add_game_message()->genGameMessage(GameMessage::CmdCode::GameMessage_CmdCode_UDP, id, 0, 0, camp, 0, grid_path);
	}
	else
	{
		log("Unit %d, Failure FP, RFP: %d", id, rfp_cnt);
		stl_cnt = 1 << (2 + rfp_cnt);
		rfp_cnt++;
	}
}

GridPath Unit::findPath(const GridPoint & dest)
{
	std::vector<std::vector<int>>& gmap = grid_map->getLogicalGridMap();
	GridPoint start = getGridPosition();

	PathFinder path_finder(gmap, start.x, start.y, dest.x, dest.y);
	path_finder.searchPath();
	path_finder.generatePath();
	GridPath _grid_path = path_finder.getPath();

	return(_grid_path);
}

void Unit::update(float dt)
{
	if (moving)
		move();

	if (stl_cnt >= 0)
		stall();

	if (state == 2)
	{
		GridPoint target_gp = unit_manager->getUnitPosition(target_id);
		Point target_fp = grid_map->getPointWithOffset(target_gp);
		Point last_fp = grid_map->getPointWithOffset(target_lastpos);
		Point cur_fp = getPosition();
		Vec2 dist_vec = target_fp - cur_fp;
		Vec2 offset_vec = target_fp - last_fp;
		
		if (target_gp == GridPoint(-1, -1))
			state = 1;
		else
			if ((dist_vec).length() < atk_range && camp == unit_manager->player_id)
			{
				moving = false;
				if (!cd)
				{
					unit_manager->msgs->add_game_message()->genGameMessage(GameMessage::CmdCode::GameMessage_CmdCode_ATK, id, target_id, atk, camp, 0, {});
				
					//log("position %f,%f,%f,%f", cur_fp.x, cur_fp.y, target_fp.x, target_fp.y);
					auto trajectory_effect = TrajectoryEffect::create();
					trajectory_effect->setPath(cur_fp,(target_fp));
					getParent()->addChild(trajectory_effect, 20);
					
					cd = cd_max;
				}
				else
					cd--;
			}
			else
				if (offset_vec.length() > TRACING_SENSOR * dist_vec.length())
				{
					target_lastpos = target_gp;
					final_dest = target_gp;
					if (rfp_cnt)
						rfp_cnt--;
					log("Unit %d, Tracing FP", id);
					tryToFindPath();
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

GridPoint UnitManager::getBasePosition()
{
	return getUnitPosition(base_id);
}



void UnitManager::produceInBase(int _unit_type)
{
	//Base* base = id_map.at(base_id);
	if (id_map.at(base_id))
		base->startProduce(_unit_type);
}

void UnitManager::updateUnitsState()
{
	socket_client->send_string(msgs->SerializeAsString());
	std::string msg_str = socket_client->get_string();
	msgs = new GameMessageSet();
	msgs->ParseFromString(msg_str);

	GameMessageSet* new_msgs = new GameMessageSet();

	for (int i = 0; i < msgs->game_message_size(); i++)
	{
		const GameMessage&  msg = msgs->game_message(i);
		if (msg.cmd_code() == GameMessage::CmdCode::GameMessage_CmdCode_EMP)
		{
			log("Empty Message, there must be something wrong");
		}
		else
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
			log("MOV?");
			//log("Unit ID: %d, Final Dest(%d, %d)", msg.unit_0(), msg.grid_path().grid_point(0).x(), msg.grid_path().grid_point(0).y());
			//Unit* u0 = id_map.at(msg.unit_0());
			//if (u0)
			//{
			//	u0->rfp_cnt = 0;
			//	u0->setGridPath(msg.grid_path());
			//	u0->setState(1);

			//	u0->motivate();

			//}
		}
		else
		if (msg.cmd_code() == GameMessage::CmdCode::GameMessage_CmdCode_TRC)
		{
			log("TRC?");
			//if (!msg.grid_path().grid_point_size())
			//	continue;
			//Unit* u0 = id_map.at(msg.unit_0());
			//if (u0)
			//{
			//	u0->setGridPath(msg.grid_path());
			//	u0->setState(2);
			//	u0->setTarget(msg.unit_1());
			//	u0->motivate();
			//}
		}
		else
		if (msg.cmd_code() == GameMessage::CmdCode::GameMessage_CmdCode_RFP)
		{
			log("RFP?");
			/*int id = msg.unit_0();
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
			unit->setDestination(grid_dest);
			unit->tryToFindPath();*/
		}
		else
		if (msg.cmd_code() == GameMessage::CmdCode::GameMessage_CmdCode_UDP)
		{
			Unit* u0 = id_map.at(msg.unit_0());
			if (u0)
			{
				u0->rfp_cnt = 0;
				const MsgGridPath& msg_grid_path = msg.grid_path();
				if (msg_grid_path.grid_point_size())
				{
					u0->setGridPath(msg.grid_path());
					u0->motivate();
				}
			}
		}
		else
		if (msg.cmd_code() == GameMessage::CmdCode::GameMessage_CmdCode_ATK)
		{
			int unitid_0 = msg.unit_0();
			int unitid_1 = msg.unit_1();
			int damage = msg.damage();
			//log("Attack! Unit %d -> Unit %d, Damage %d", unitid_0, unitid_1, damage);
			Unit* unit_1 = id_map.at(unitid_1);
			if (unit_1)
			{
				unit_1->displayHPBar();
				if (unit_1->underAttack(damage))
					deleteUnit(unitid_1);
			}
		}
	}
	delete msgs;
	msgs = new_msgs;
}

void UnitManager::deleteUnit(int id)
{
	Unit* unit = id_map.at(id);
	if (unit)
	{
		auto itor = std::find(selected_ids.begin(), selected_ids.end(), id);
		if (itor != selected_ids.end())
			selected_ids.erase(itor);

		unit->removeFromMaps();

		id_map.erase(id);
	}
}

Unit* UnitManager::createNewUnit(int id, int camp, int unit_type, GridPoint crt_gp)
{
	Unit* nu;
	Base* tmp_base;
	switch (unit_type)
	{
	case 1:
		nu = Fighter::create("Picture/units/fighter.png");
		break;
	case 2:
		nu = Tank::create("Picture/units/tank.png");
		break;
	case 3:
		nu = Soldier::create("Picture/units/soldier.png");
		break;
	case 5:
		tmp_base = Base::create("Picture/factory.jpg");
		if (camp == player_id)
			base = tmp_base;
		nu = tmp_base;
		break;
	default:
		break;
	}

	nu->unit_manager = this;
	nu->id = id;
	nu->camp = camp;
	nu->setProperties();
	nu->setAnchorPoint(Vec2(0.5, 0.5));
	nu->addToMaps(crt_gp, tiled_map, grid_map);
	nu->initHPBar();
	nu->initFlag();
	nu->schedule(schedule_selector(Unit::update));

	return(nu);
}


//生成新单位测试程序
void UnitManager::genCreateMessage(int _unit_type, const GridPoint & crt_gp)
{
	auto new_msg = msgs->add_game_message();
	new_msg->genGameMessage(GameMessage::CmdCode::GameMessage_CmdCode_CRT, next_id, 0, 0, player_id, _unit_type, GridPath{ crt_gp });
	next_id += MAX_PLAYER_NUM;
}

void UnitManager::initiallyCreateUnits()
{
	auto* obj_group = tiled_map->getObjectGroup("InitialUnits");
	auto& objs = obj_group->getObjects();

	for (auto& obj : objs)
	{
		auto& dict = obj.asValueMap();
		float cx = dict["x"].asFloat();
		float cy = dict["y"].asFloat();
		int camp = dict["camp"].asInt();
		int type = dict["type"].asInt();
		GridPoint crt_gp = grid_map->getGridPoint({ cx, cy });

		if (camp == player_id && type == BASE_TYPE_NO)
		{
			base_id = next_id;
		}
		if (camp == player_id)
		{
			genCreateMessage(type, crt_gp);
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
					//log("Unit ID: %d, tracing enemy id: %d", id, id_unit.second->id);
					Unit* unit = id_map.at(id);
					if (!unit || !unit->isMobile())
						continue;
					GridPoint target_pos = getUnitPosition(id_unit.first);
					unit->setState(2);
					unit->setTarget(id_unit.second->id);
					unit->setDestination(target_pos);
					log("Unit %d, start tracing FP", id);
					unit->tryToFindPath();
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
			Unit* unit = id_map.at(id);

			if (!unit || !unit->isMobile())
				continue;

			GridPoint grid_dest = grid_map->getGridPoint(select_point);
			log("Unit ID: %d, plan to move to:(%d, %d)", id, grid_dest.x, grid_dest.y);

			unit->setDestination(grid_dest);
			log("Unit %d, start moving FP", id);
			unit->tryToFindPath();

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