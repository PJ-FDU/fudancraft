#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "cocos2d.h"

class MainScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init() override;
	void update(float f) override;
	bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*)override;
	void onTouchMoved(cocos2d::Touch*, cocos2d::Event*)override;
	void onTouchEnded(cocos2d::Touch*, cocos2d::Event*)override;

	CREATE_FUNC(MainScene);
private:
	cocos2d::Sprite* airplane;
	cocos2d::DrawNode* rect;
	cocos2d::Vec2 touchPoint{500,500},touch_begin;
	float moveSpeed = 10.0;
};

#endif
