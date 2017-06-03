#pragma once

#define SERVER

#include "Unit.h"
#include "SocketServer.h"
#include "SocketClient.h"

class BattleScene : public cocos2d::Layer
{
public:
	GameMessageSet msg_set;

	//static cocos2d::Scene* createScene();
	//virtual bool init() override;
	static cocos2d::Scene* createScene(SocketClient* _socket_client, SocketServer* _socket_server = nullptr);
	virtual bool init(SocketClient* _socket_client, SocketServer* _socket_server);
	void update(float f) override;
	bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*)override;
	void onTouchMoved(cocos2d::Touch*, cocos2d::Event*)override;
	void onTouchEnded(cocos2d::Touch*, cocos2d::Event*)override;

	void onKeyPressed(cocos2d::EventKeyboard::KeyCode, cocos2d::Event *) override;

	//CREATE_FUNC(BattleScene);

	static BattleScene* create(SocketClient* _socket_client, SocketServer* _socket_server);

private:
	int player_id = 0;

	int frame_cnt = 0;


	SocketServer* socket_server = nullptr;
	SocketClient* socket_client = nullptr;

	int start_flag = 0;

	cocos2d::TMXTiledMap* battle_map = nullptr;
	GridMap* grid_map = nullptr;
	UnitManager* unit_manager = nullptr;

	cocos2d::DrawNode* mouse_rect = nullptr;

	cocos2d::Point last_touch{0, 0};

	void initPlayerID();

	//EventListenerMouse* _mouseListener;
};
