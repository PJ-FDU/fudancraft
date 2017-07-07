#pragma once

#define SERVER

#include "Unit.h"
#include "SocketServer.h"
#include "SocketClient.h"
#include <string>

class ControlPanel;
class Money;
class Notice;
class MiniMap;

class MouseRect : public cocos2d::DrawNode
{
public:
	CREATE_FUNC(MouseRect);
	cocos2d::Point touch_start, touch_end;
	cocos2d::Point start, end;
	void update(float f) override;
	void reset();
};

class BattleScene : public cocos2d::Layer
{
public:
	GameMessageSet msg_set;
	void onExit()override;

	/**
	 * \brief notify winning and stoping controlling
	 */
	void win();
	/**
	 * \brief notify lost and stoping controlling
	 */
	void lose();

	static cocos2d::Scene* createScene(SocketClient* _socket_client, SocketServer* _socket_server = nullptr);
	virtual bool init(SocketClient* _socket_client, SocketServer* _socket_server);
	/**
	 * \brief call this function every frame
	 */
	virtual void update(float f) override;
	/**
	 * \brief get touch point 
	 */
	bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*)override;
	/**
	 * \brief moving the mouse, change the mouseRect
	 */
	void onTouchMoved(cocos2d::Touch*, cocos2d::Event*)override;
	/**
	 * \brief ending of touch, call select function
	 */
	void onTouchEnded(cocos2d::Touch*, cocos2d::Event*)override;

	/**
	 * \brief scroll map based on mouse position
	 */
	void scrollMap();

	/**
	 * \brief focus on user's base
	 */
	void focusOnBase();

	void focusOn(cocos2d::Point pos);

	/**
	 * \brief get reward when destroy unit
	 */
	void destroyReward(int destroyed_type);


	/**
	 * \brief keyboard press callback function
	 */
	void onKeyPressed(cocos2d::EventKeyboard::KeyCode, cocos2d::Event *) override;


	static BattleScene* create(SocketClient* _socket_client, SocketServer* _socket_server);

	/**
	 * \brief back to main menu
	 */
	void menuBackCallback(cocos2d::Ref* pSender);

	cocos2d::Rect getVisionRect();
private:
//	void create_figher(Ref*);


	int player_id = 0;

	unsigned long long frame_cnt = 0;


	SocketServer* socket_server = nullptr;
	SocketClient* socket_client = nullptr;

	bool start_flag = false;
	bool end_flag = false;

	cocos2d::TMXTiledMap* battle_map = nullptr;
	cocos2d::TMXTiledMap* warfog_map = nullptr;
	GridMap* grid_map = nullptr;
	UnitManager* unit_manager = nullptr;
	ControlPanel* control_panel_;


	MouseRect* mouse_rect = nullptr;

	Money* money = nullptr;
	Notice* notice = nullptr;
	MiniMap* mini_map = nullptr;

	cocos2d::Point last_touch{0, 0};
	cocos2d::Point crusor_position{0, 0};

	cocos2d::Rect mini_map_rect{};

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
	/**
	 * \brief update money on screen
	 */
	void updateMoneyDisplay();
	/**
	 * \param cost cost 
	 * \return if money can cover the cost
	 */
	bool checkMoney(int cost) const;
	/**
	 * \brief decrease cost in money
	 * \param cost 
	 */
	void spendMoney(int cost);
	/**
	 * \brief increase amount in money
	 * \param amount 
	 */
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
	/**
	 * \brief show notice
	 * \param ntc type of notification
	 * \param _ntc_life disapper after this 
	 */
	void displayNotice(std::string ntc, int _ntc_life);
	/**
	 * \brief notification that exist forever
	 * \param ntc type of notification
	 */
	void displayNotice(std::string ntc);
	bool init() override;
private:
	int timer = 0;
	int ntc_life = 0;
};

class MiniMap : public cocos2d::DrawNode
{
public:
	CREATE_FUNC(MiniMap);
	void update(float f) override;
	void setGridMap(GridMap* _grid_map);
	void setUnitManager(UnitManager* _unit_manager);
	void setBattleScene(BattleScene* _battle_scene);
private:
	GridMap* grid_map;
	UnitManager* unit_manager;
	BattleScene* battle_scene;
};