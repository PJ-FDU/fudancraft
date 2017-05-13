#pragma once

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
	cocos2d::TMXTiledMap* battle_map;
	GridMap* grid_map;
	UnitManager* unit_manger;

	cocos2d::DrawNode* mouse_rect;

	int mouse_state = 0;
	cocos2d::Point last_touch{0, 0};

	//EventListenerMouse* _mouseListener;
};
