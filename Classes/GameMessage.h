#pragma once

#include "cocos2d.h"

class GameMessage : public cocos2d::ValueVector
{
public:
	GameMessage(const std::initializer_list<cocos2d::Value> value_list) : cocos2d::ValueVector{value_list} {}

};

