#include "AdvancedUnit.h"

USING_NS_CC;

Fighter* Fighter::create(const std::string& filename)
{
	Fighter *ret = new (std::nothrow) Fighter();
	if (ret && ret->initWithFile(filename))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);

	return nullptr;
}

void Fighter::setProperties()
{
	atk = 10;
	atk_range = 100;
	atk_period = 20;
	hp_max = 100;
	cd_max = 5;
	move_speed = 3.0f;

	size = GridSize(1, 1);

	cd = 0;
	hp = hp_max;
}