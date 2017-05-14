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
	cocos2d::Point frame_points[4]{ { 0, 60 },{ 0, 70 },{ 45, 70 },{ 45,60 } };
	cocos2d::Point bar_points[4]{ { 0, 60 },{ 0, 70 },{ 45, 70 },{ 45,60 } };
	Unit* owner;
};

class UnitManager : public cocos2d::Ref
{
public:
	CREATE_FUNC(UnitManager);
	bool init();
	void setMessageStack(std::vector<GameMessage>* _msgs);
	void setTiledMap(cocos2d::TMXTiledMap* _tiledMap);
	void setGridMap(GridMap* _grid_map);
	void updateUnitsState();
private:
	cocos2d::Map<int, Unit*> id_map;
	cocos2d::Vector<Unit*> own_units;
	cocos2d::Vector<Unit*> enemy_units;

	std::vector<GameMessage>* msgs;
	cocos2d::TMXTiledMap* tiled_map;
	GridMap* grid_map;
	int next_id = 0;

	Unit* createNewUnit(int camp, int uint_type, float cx, float cy);

};

class Unit : public cocos2d::Sprite
{
public:
	static Unit* create(const std::string& filename);

	virtual void setProperties();
	virtual void update(float f) override;

	void initHPBar();
	void addToMaps(cocos2d::TMXTiledMap* _tiled_map, GridMap* _grid_map);
	GridPoint getGridPosition();
protected:
	int id;
	int state;
	int target_id;
	bool selected;

	int cd;
	int hp;

	int atk;
	int atk_range;
	int atk_period;
	int hp_max;
	int cd_max;
	float move_speed;

	cocos2d::TMXTiledMap* tiled_map;
	GridMap* grid_map;

	HPBar* hpbar;

	friend void HPBar::update(float ft);
	friend void UnitManager::updateUnitsState();
};

#endif