#pragma once
#ifndef __UNIT_H__
#define __UNIT_H__

#include "cocos2d.h"
#include "GridMap.h"
#include "GameMessage.pb.h"
#include "fudancraft.h"
#include "PathFinder/PathFinder.h"
#include "SocketClient.h"

class Unit;
class UnitManager;
class Base;

class HPBar : public cocos2d::DrawNode
{
public:
	void update(float f) override;
	CREATE_FUNC(HPBar);
	void monitor(Unit* _owner) { owner = _owner; }
private:
	cocos2d::Point frame_points[4]{ { 0, 40 },{ 0, 43 },{ 32, 43 },{ 32, 40 } };
	cocos2d::Point bar_points[4]{ { 0, 40 },{ 0, 43 },{ 32, 43 },{ 32, 40 } };
	Unit* owner = nullptr;
};

class UnitManager : public cocos2d::Ref
{
public:
	int player_id = 0;
	GameMessageSet* msgs;

	CREATE_FUNC(UnitManager);
	bool init();
	void setMessageSet(GameMessageSet* _msgs);
	void setTiledMap(cocos2d::TMXTiledMap* _tiledMap);
	void setGridMap(GridMap* _grid_map);
	void setPlayerID(int _player_id);
	void setSocketClient(SocketClient* _socket_client);
	void updateUnitsState();
	void deleteUnit(int id);

	GridPoint getUnitPosition(int unit_id);
	GridPoint getBasePosition();
	void genCreateMessage(int _unit_type, const GridPoint& _crt_gp);
	void produceInBase(int _unit_type);

	void initiallyCreateUnits();
	void selectUnits(cocos2d::Point select_point);
	void selectUnits(cocos2d::Rect select_rect);
private:
	cocos2d::Map<int, Unit*> id_map;
	std::vector<int> selected_ids;
	//cocos2d::Vector<Unit*> own_units;
	//cocos2d::Vector<Unit*> enemy_units;

	cocos2d::TMXTiledMap* tiled_map = nullptr;
	GridMap* grid_map = nullptr;
	SocketClient* socket_client = nullptr;
	int next_id = 1;
	int base_id = 1;

	Base* base = nullptr;

	Unit* createNewUnit(int id, int camp, int uint_type, GridPoint crt_gp);
	void deselectAllUnits();

	//friend class Unit;
};

class Unit : public cocos2d::Sprite
{
public:
	int id;
	int camp = 0;
	int z_index;
	UnitManager* unit_manager = nullptr;

	static Unit* create(const std::string& filename);

	virtual void setProperties();
	virtual void update(float f) override;

	void initHPBar();
	void displayHPBar();
	void hideHPBar();
	virtual void addToMaps(const GridPoint & crt_gp, cocos2d::TMXTiledMap* _tiled_map, GridMap* _grid_map);
	void removeFromMaps();
	GridPoint getGridPosition();
	void setGridPath(const MsgGridPath& _grid_path);
	void motivate();
	void setState(int _state);
	void setTarget(int _target_id);
	int getState() const;
	bool hasArrivedAtDest();
	bool underAttack(int damage);

	GridPath planToMoveTo(GridPoint& dest)
	{
		return(searchForPath(grid_map->getLogicalGridMap(), getGridPosition(), dest));
	}
	GridPath searchForPath(std::vector<std::vector<int>>& gmap, const GridPoint& start, const GridPoint& dest)
	{
		PathFinder path_finder(gmap, start.x, start.y, dest.x, dest.y);
		path_finder.searchPath();
		path_finder.generatePath();
		GridPath _grid_path = path_finder.getPath();

		return(_grid_path);
	}
protected:
	int state = 0;
	bool moving = false;
	int target_id;
	bool selected = false;
	GridPath grid_path;
	GridPoint final_dest;
	GridPoint cur_pos;
	GridPoint cur_dest;
	GridPoint target_lastpos;

	GridSize size;
	GridRect cur_grec;

	int rfp_cnt = 0;
	int roc_cnt = 0;

	int cd;
	int hp;

	int atk;
	int atk_range;
	int atk_period;
	int hp_max;
	int cd_max;
	float move_speed;

	cocos2d::TMXTiledMap* tiled_map = nullptr;
	GridMap* grid_map = nullptr;

	HPBar* hpbar = nullptr;

	friend void HPBar::update(float ft);
	friend void UnitManager::updateUnitsState();

	//friend class UnitManager;
};



#endif