#pragma once
#ifndef __UNIT_H__
#define __UNIT_H__

#include "cocos2d.h"
#include "GridMap.h"
#include "GameMessage.pb.h"
#include "fudancraft.h"
#include "PathFinder/PathFinder.h"
#include "SocketClient.h"

#include <fstream>

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
	int player_id = 0;

	CREATE_FUNC(UnitManager);
	bool init();
	void setMessageSet(GameMessageSet* _msgs);
	void setTiledMap(cocos2d::TMXTiledMap* _tiledMap);
	void setGridMap(GridMap* _grid_map);
	void setPlayerID(int _player_id);
	void setSocketClient(SocketClient* _socket_client);
	void updateUnitsState();
	void updatePathMessage(int _unit_id, const GridPath& _grid_path);

	GridPoint getUnitPosition(int unit_id);
	void genCreateMessage();

	void initiallyCreateUnits();
	void selectUnits(cocos2d::Point select_point);
	void selectUnits(cocos2d::Rect select_rect);
private:
	cocos2d::Map<int, Unit*> id_map;
	std::vector<int> selected_ids;
	//cocos2d::Vector<Unit*> own_units;
	//cocos2d::Vector<Unit*> enemy_units;

	GameMessageSet* msgs;
	cocos2d::TMXTiledMap* tiled_map = nullptr;
	GridMap* grid_map = nullptr;
	SocketClient* socket_client = nullptr;
	int next_id = 1;

	Unit* createNewUnit(int id, int camp, int uint_type, GridPoint crt_gp);
	void deselectAllUnits();

};

class Unit : public cocos2d::Sprite
{
public:
	int id;
	int camp = 0;
	UnitManager* unit_manager = nullptr;

	static Unit* create(const std::string& filename);

	virtual void setProperties();
	virtual void update(float f) override;

	void initHPBar();
	void displayHPBar();
	void hideHPBar();
	void addToMaps(cocos2d::TMXTiledMap* _tiled_map, GridMap* _grid_map);
	GridPoint getGridPosition();
	void setGridPath(const MsgGridPath& _grid_path);
	void setState(int _state);
	void setTarget(int _target_id);
	int getState() const;
	bool hasArrivedAtDest();
	bool updateGridPostion();

	GridPath planToMoveTo(GridPoint& dest)
	{
		return(searchForPath(grid_map->getLogicalGridMap(), getGridPosition(), dest));
	}
	GridPath searchForPath(std::vector<std::vector<int>>& gmap, const GridPoint& start, const GridPoint& dest)
	{
		std::ofstream out_file;
		out_file.open("pathfind.log", std::ios::app);

		out_file << "(" << start.x << "," << start.y << ")" << "->" << "(" << dest.x << "," << dest.y << ")" << std::endl;

		for (int gy = 127; gy >= 0; gy--)
		{
			for (int gx = 0; gx < 128; gx++)
				if (gx == start.x && gy == start.y)
					out_file << '2';
				else
					if (gx == dest.x && gy == dest.y)
						out_file << '3';
					else 
						out_file << gmap[gx][gy];
			out_file << std::endl;
		}
		

		PathFinder path_finder(gmap, start.x, start.y, dest.x, dest.y);
		path_finder.searchPath();
		path_finder.generatePath();
		GridPath _grid_path = path_finder.getPath();

		out_file << "Path: ";
		for (auto & gp : _grid_path)
			out_file << "(" << gp.x << "," << gp.y << ")" << "->";
		out_file << std::endl;
		out_file.close();

		return(_grid_path);
	}
protected:
	int state;
	int target_id;
	bool selected;
	GridPath grid_path;
	GridPoint final_dest;
	GridPoint cur_pos;
	GridPoint cur_dest;

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