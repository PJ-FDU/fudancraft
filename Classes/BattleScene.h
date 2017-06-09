#pragma once

#define SERVER

#include "Unit.h"
#include "SocketServer.h"
#include "SocketClient.h"
#include <string>

class ControlPanel;
class Money;
class Notice;

class MouseRect : public cocos2d::DrawNode
{
public:
	CREATE_FUNC(MouseRect);
	cocos2d::Point touch_start, touch_end;
	cocos2d::Point start, end;
	void update(float f) override;
};

class BattleScene : public cocos2d::Layer
{
public:
	GameMessageSet msg_set;

	void win();
	void lose();

	static cocos2d::Scene* createScene(SocketClient* _socket_client, SocketServer* _socket_server = nullptr);
	virtual bool init(SocketClient* _socket_client, SocketServer* _socket_server);
	void update(float f) override;
	bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*)override;
	void onTouchMoved(cocos2d::Touch*, cocos2d::Event*)override;
	void onTouchEnded(cocos2d::Touch*, cocos2d::Event*)override;

	void scrollMap();

	void focusOnBase();

	void destroyReward(int destroyed_type);


	void onKeyPressed(cocos2d::EventKeyboard::KeyCode, cocos2d::Event *) override;


	static BattleScene* create(SocketClient* _socket_client, SocketServer* _socket_server);

	void menuBackCallback(cocos2d::Ref* pSender);
private:
//	void create_figher(Ref*);


	int player_id = 0;

	unsigned long long frame_cnt = 0;


	SocketServer* socket_server = nullptr;
	SocketClient* socket_client = nullptr;

	int start_flag = 0;

	cocos2d::TMXTiledMap* battle_map = nullptr;
	GridMap* grid_map = nullptr;
	UnitManager* unit_manager = nullptr;
	ControlPanel* control_panel_;


	MouseRect* mouse_rect = nullptr;

	Money* money = nullptr;
	Notice* notice = nullptr;

	cocos2d::Point last_touch{0, 0};
	cocos2d::Point crusor_position{0, 0};

	void initPlayerID();

	//EventListenerMouse* _mouseListener;
};

class ControlPanel:public cocos2d::Menu
{
public:
	CREATE_FUNC(ControlPanel);
	bool init() override;
	void setFighterCallback(std::function<void(Ref*)>);

	void setTankCallback(std::function<void(Ref*)>);
	void setSoldierCallback(std::function<void(Ref*)>);
private:
	cocos2d::MenuItemImage *fighter;
	cocos2d::MenuItemImage *tank;
	cocos2d::MenuItemImage *soldier;

};

class Money : public cocos2d::LabelBMFont
{
public:
	void update(float f) override;
	CREATE_FUNC(Money);
	bool init() override;
	void updateMoneyDisplay();
	bool checkMoney(int cost) const;
	void spendMoney(int cost);
	void increaseMoney(int amount);
private:
	int money = 0;
	int timer = 0;

	int inc_prd = MONEY_INC_PERIOD;
	int inc_amt = MONEY_INC_AMOUNT;
};

class Notice : public cocos2d::LabelBMFont
{
public:
	CREATE_FUNC(Notice);
	void update(float f) override;
	void displayNotice(std::string ntc, int _ntc_life);
	void displayNotice(std::string ntc);
	bool init() override;
private:
	int timer = 0;
	int ntc_life = 0;
};