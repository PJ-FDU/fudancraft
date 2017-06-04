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
	atk = 8;
	atk_range = 100;
	atk_period = 20;
	hp_max = 100;
	cd_max = 20;
	move_speed = 3.0f;

	mobile = true;

	z_index = 10;

	size = GridSize(1, 1);

	cd = 0;
	hp = hp_max;
}

Tank* Tank::create(const std::string& filename)
{
	Tank *ret = new (std::nothrow) Tank();
	if (ret && ret->initWithFile(filename))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);

	return nullptr;
}

void Tank::setProperties()
{
	atk = 10;
	atk_range = 100;
	atk_period = 30;
	hp_max = 200;
	cd_max = 30;
	move_speed = 1.0f;

	z_index = 10;

	mobile = true;

	size = GridSize(1, 1);

	cd = 0;
	hp = hp_max;
}

Soldier* Soldier::create(const std::string& filename)
{
	Soldier *ret = new (std::nothrow) Soldier();
	if (ret && ret->initWithFile(filename))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);

	return nullptr;
}

void Soldier::setProperties()
{
	atk = 5;
	atk_range = 50;
	atk_period = 10;
	hp_max = 80;
	cd_max = 10;
	move_speed = 1.5f;

	z_index = 10;

	mobile = true;

	size = GridSize(1, 1);

	cd = 0;
	hp = hp_max;
}