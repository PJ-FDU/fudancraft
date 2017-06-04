#pragma once

#include "Unit.h"

class Building : public Unit
{
public:
	static Building* create(const std::string& filename);
	void addToMaps(const GridPoint & crt_gp, cocos2d::TMXTiledMap* _tiled_map, GridMap* _grid_map) override;
	void update(float f) override;
private:
	void setProperties() override;
};
