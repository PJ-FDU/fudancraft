#pragma once
#ifndef __UNIT_H__
#define __UNIT_H__

#include "cocos2d.h"
#include "GridMap.h"
#include "GameMessage.h"
#include "fudancraft.h"

class Unit;
class UnitManager;

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
	CREATE_FUNC(UnitManager);
	bool init();
	void setMessageStack(std::vector<GameMessage>* _msgs);
	void setTiledMap(cocos2d::TMXTiledMap* _tiledMap);
	void setGridMap(GridMap* _grid_map);
	void setPlayerID(int _player_id);
	void setGridPath();
	void updateUnitsState();

	void initiallyCreateUnits();
	void selectUnits(cocos2d::Point select_point);
	void selectUnits(cocos2d::Rect select_rect);
private:
	cocos2d::Map<int, Unit*> id_map;
	std::vector<int> selected_ids;
	//cocos2d::Vector<Unit*> own_units;
	//cocos2d::Vector<Unit*> enemy_units;

	std::vector<GameMessage>* msgs;
	cocos2d::TMXTiledMap* tiled_map = nullptr;
	GridMap* grid_map = nullptr;
	int next_id = 1;
	int player_id = 0;

	Unit* createNewUnit(int camp, int uint_type, GridPoint crt_gp);
	void deselectAllUnits();

};

class Unit : public cocos2d::Sprite
{
public:
	int id;
	int camp = 0;

	static Unit* create(const std::string& filename);

	virtual void setProperties();
	virtual void update(float f) override;

	void initHPBar();
	void displayHPBar();
	void hideHPBar();
	void addToMaps(cocos2d::TMXTiledMap* _tiled_map, GridMap* _grid_map);
	GridPoint getGridPosition();

	GridPath planToMoveTo(const GridPoint& dest)
	{
		return(searchForPath(grid_map->getLogicalGridMap(), getGridPosition(), dest));
	}
	GridPath searchForPath(const std::vector<std::vector<int>>& gmap, const GridPoint& start, const GridPoint& dest)
	{
		GridPath _grid_path;
		_grid_path.push_back(GridPoint(start.x, dest.y));
		_grid_path.push_back(GridPoint(dest.x, dest.y));
		return(_grid_path);
	}
protected:
	int state;
	int target_id;
	bool selected;
	GridPath grid_path;

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
};

#endif