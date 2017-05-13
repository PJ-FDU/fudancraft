#pragma once
#ifndef __BATTLE_SCENE_H__
#define __BATTLE_SCENE_H__

#include "cocos2d.h"
#include "Unit.h"
class BattleScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init() override;
	void update(float f) override;
	bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*)override;
	void onTouchMoved(cocos2d::Touch*, cocos2d::Event*)override;
	void onTouchEnded(cocos2d::Touch*, cocos2d::Event*)override;

	void onKeyPressed(cocos2d::EventKeyboard::KeyCode, cocos2d::Event *) override;

	CREATE_FUNC(BattleScene);
private:
	std::vector<Unit *> my_units_;
	std::vector<Unit *> enemy_units_;
	cocos2d::DrawNode* mouse_rect;

	cocos2d::TMXTiledMap* battle_map;
	cocos2d::MenuItemFont* enemy_btn;

	cocos2d::Rect map_range;
	int state = 0;
	long timer = 0;
	int enemy_period = 1000;
	int AI_period = 10;
	cocos2d::Vec2 touchPoint{ 500,500 };


	//EventListenerMouse* _mouseListener;
};

#endif
