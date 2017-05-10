#pragma once
#ifndef __UNIT_H__
#define __UNIT_H__

#include "cocos2d.h"
#include "GridMap.h"

class Unit;

class HPBar : public cocos2d::DrawNode
{
public:
	void update();
	CREATE_FUNC(HPBar);
	void monitor(Unit* _owner) { owner = _owner; }
private:
	cocos2d::Point frame_points[4]{ { 0, 60 },{ 0, 70 },{ 45, 70 },{ 45,60 } };
	cocos2d::Point bar_points[4]{ { 0, 60 },{ 0, 70 },{ 45, 70 },{ 45,60 } };
	Unit* owner;
};

class Unit : public cocos2d::Sprite
{
public:
	static Unit* create(const std::string& filename);
	virtual void initProperties();
	void addToMaps(cocos2d::TMXTiledMap* _tiled_map, GridMap* _grid_map);
	GridPoint getGridPosition();
private:
	int state;
	int target_id;
	bool selected = 0;

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
};

class UnitManager
{

};

#endif