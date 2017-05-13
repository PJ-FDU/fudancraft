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


	bool update();
	bool isActive() const { return active; }
	bool isAlive() const { return alive; }
	bool isSelected() const { return selected; }
	void unselect()
	{
		selected = false;
		if (hpbar)
			hpbar->setVisible(false);
	}
	void select()
	{
		selected = true;
		if (hpbar)
			hpbar->setVisible(true);
	}
	void activate() { active = true; }
	void deactivate() { active = false; }
	void setDest(cocos2d::Point destination) { this->dest = destination; }
	void setTarget(Unit* enemy_plane) { this->target = enemy_plane; }
	void setState(int _state) { this->state = _state; }
	int getState() const { return state; }
	int getHP() const { return hp; }
	int getHPMax() const { return hp_max; }
	Unit* getTarget() const { return target; }
	void decreaseHp(int dh) { this->hp -= dh; }

protected:
	int id;
	static int total_number;
	char owner;//归属者，用以表明属于哪个阵营（1，2，3...)初始化时由服务器分配；

	int state;
	int target_id;
	bool selected{ false };
	bool active{ false };
	bool alive{ true };

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
	cocos2d::Point dest;
	HPBar* hpbar;
	Unit* target;
};

class UnitManager
{

};

#endif