#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "cocos2d.h"

class Airplane;

class HPBar : public cocos2d::DrawNode
{
public:
	void update();
	CREATE_FUNC(HPBar);
	void hpbarInit(Airplane* _owner) { owner = _owner; }
private:
	cocos2d::Point frame_points[4]{ {0, 60}, {0, 70}, {45, 70},{45,60} };
	cocos2d::Point bar_points[4]{ { 0, 60 },{ 0, 70 },{ 45, 70 },{ 45,60 } };
	Airplane* owner;
};

class Airplane : public cocos2d::Sprite
{
public:
	bool update();
	bool isActive() { return active; }
	bool isAlive() { return alive; }
	bool isSelected() { return selected; }
	void select() { selected = true; }
	void unselect() { selected = false; }
	void activate() { active = true; }
	void setDest(cocos2d::Point destination) { this->dest = destination; }
	void setTarget(Airplane* enemy_plane) { this->target = enemy_plane; }
	void setState(int _state) { this->state = _state; }
	int getHP() { return hp; }
	int getHPMax() { return hp_max; }
	void decreaseHp(int dh) { this->hp -= dh; }

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
	bool alive = 1;
	int state = 0;
	int cd = 0;
	int hp = 100;

	Airplane* target = nullptr;
	HPBar* hpbar = nullptr;

	const int atk = 10;
	const int atk_range = 100;
	const int atk_period = 20;
	const int hp_max = 100;
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

	void onKeyPressed(cocos2d::EventKeyboard::KeyCode , cocos2d::Event *) override;

	CREATE_FUNC(MainScene);
private:
	std::vector<Airplane*> my_planes;
	std::vector<Airplane*> enemy_planes;
	cocos2d::DrawNode* mouse_rect;
	cocos2d::TMXTiledMap* battle_map;
	cocos2d::MenuItemFont* enemy_btn;

	cocos2d::Rect map_range;
	int state = 0;
	int enemy_timer = 10;
	cocos2d::Vec2 touchPoint{500,500};
	

	//EventListenerMouse* _mouseListener;
};



#endif

