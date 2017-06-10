#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "SocketServer.h"
#include "SocketClient.h"
#include "ui/UIEditBox/UIEditBox.h"

namespace cocos2d {namespace ui {
	class EditBox;
}
}

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

	bool init() override;
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    void menuCreditsCallback(cocos2d::Ref* pSender);
	void menuStartCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
};
class StartMenu : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	bool init() override;
	// a selector callback
	void menuServerCallback(cocos2d::Ref* pSender);
	void menuClientCallback(cocos2d::Ref* pSender);
	void menuBackCallback(cocos2d::Ref* pSender);

	// implement the "static create()" method manually
	CREATE_FUNC(StartMenu);
};

class ServerMenu : public cocos2d::Layer,cocos2d::ui::EditBoxDelegate
{
public:
	static cocos2d::Scene* createScene();

	bool init() override;
	// a selector callback
	void menuStartServerCallback(cocos2d::Ref* pSender);
	void menuStartGameCallback(cocos2d::Ref* pSender);
	void menuBackCallback(cocos2d::Ref* pSender);

	// implement the "static create()" method manually
	CREATE_FUNC(ServerMenu);
private:
	void editBoxReturn(cocos2d::ui::EditBox* editBox) override;
	void connectionSchdeule(float f);

	cocos2d::Label* connection_msg_;
	SocketServer* socket_server_{nullptr};
	SocketClient* socket_client_{nullptr};
};

class ClientMenu : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	bool init() override;
	// a selector callback
//	void menuStartServerCallback(cocos2d::Ref* pSender);
	void menuStartGameCallback(cocos2d::Ref* pSender);
	void menuBackCallback(cocos2d::Ref* pSender);

	// implement the "static create()" method manually
	CREATE_FUNC(ClientMenu);
private:
	void wait_start();
	void startSchedule(float f);

	SocketClient* socket_client_{ nullptr };
	cocos2d::Label* connection_msg_;
	int timer_ = 0;
};

class CreditsScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	bool init() override;
	
	void menuBackCallback(cocos2d::Ref* pSender);
	void scheduleMove(float f);

	// implement the "static create()" method manually
	CREATE_FUNC(CreditsScene);
private:
	cocos2d::Label *label;
};

#endif // __HELLOWORLD_SCENE_H__
