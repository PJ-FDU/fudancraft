#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "cocos2d.h"

class Airplane : public cocos2d::Sprite
{
public:
	void update();
	bool isActive() { return active; }
	bool isSelected() { return selected; }
	void select() { selected = true; }
	void unselect() { selected = false; }
	void activate() { active = true; }
	void setDest(cocos2d::Point destination) { this->dest = destination; }
	static Airplane* createPlane(const std::string& filename)
	{
		Airplane *airplane = new (std::nothrow) Airplane();
		if (airplane && airplane->initWithFile(filename))
		{
			airplane->autorelease();
			return airplane;
		}
		CC_SAFE_DELETE(airplane);
		return nullptr;
	}
private:
	bool selected = 0;
	bool active = 0;
	int state = 0;

	const float move_speed = 10.0;

	cocos2d::Point dest;
};

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
	std::vector<Airplane*> my_planes;
	std::vector<Airplane*> enemy_planes;
	cocos2d::DrawNode* mouse_rect;
	int state = 0;
	cocos2d::Vec2 touchPoint{500,500};
	

	//EventListenerMouse* _mouseListener;
};



#endif

