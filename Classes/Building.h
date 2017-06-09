#pragma once

#include "Unit.h"

class Base : public Unit
{
public:
	static Base* create(const std::string& filename);
	void addToMaps(const GridPoint & crt_gp, cocos2d::TMXTiledMap* _tiled_map, GridMap* _grid_map) override;
	void update(float f) override;
	void setState(int _state) override;
	void initBars() override;
	void displayProdBar();
	void startProduce(int unit_type);
private:
	bool producing = false;

	int cur_prod;
	int prod_process;
	int prod_period;
	std::map<int, int> period_map;
	std::vector<int> prod_list;

	Bar* prod_bar = nullptr;

	void setProperties() override;
};

class Tower : public Unit
{
public:
	static Tower* create(const std::string& filename);
	void update(float f) override;
	void setState(int _state) override;
private:
	void setProperties() override;
};
