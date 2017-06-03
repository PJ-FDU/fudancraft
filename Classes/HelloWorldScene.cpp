#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "BattleScene.h"
#include "SocketServer.h"
#include "SocketClient.h"
#include "cocos-ext.h"
#include <future>

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);
	
    // return the scene
    return scene;
}

bool HelloWorld::init()
{
    
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();   
   

	MenuItemFont::setFontName("Blackwood Castle Shadow");
	auto start_label = MenuItemFont::create("Start Game",CC_CALLBACK_1(HelloWorld::menuStartCallback,this));
	auto quit_label = MenuItemFont::create("Quit Game",CC_CALLBACK_1(HelloWorld::menuCloseCallback,this));
	MenuItemFont::setFontSize(24);
	
    auto menu = Menu::create(start_label,quit_label,NULL);
    menu->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height/2));
	
	menu->alignItemsVerticallyWithPadding(40);
	this->addChild(menu, 1);

    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
  
	unscheduleUpdate();
    Director::getInstance()->end();    
    
}

void HelloWorld::menuStartCallback(cocos2d::Ref* pSender)
{
	auto scene = StartMenu::createScene();
	Director::getInstance()->replaceScene(scene);
}

cocos2d::Scene* StartMenu::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = StartMenu::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool StartMenu::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	MenuItemFont::setFontName("Blackwood Castle Shadow");
	MenuItemFont::setFontSize(24);
	auto server_label = MenuItemFont::create("Start As Server", CC_CALLBACK_1(StartMenu::menuServerCallback, this));
	auto client_label = MenuItemFont::create("Start As Client", CC_CALLBACK_1(StartMenu::menuClientCallback, this));
	auto back_label = MenuItemFont::create("Back", CC_CALLBACK_1(StartMenu::menuBackCallback, this));
	auto menu = Menu::create(server_label, client_label,back_label, NULL);
	menu->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));

	menu->alignItemsVerticallyWithPadding(40);
	this->addChild(menu, 1);

	return true;
}

void StartMenu::menuServerCallback(cocos2d::Ref* pSender)
{
	log("server button pressed");
	auto scene = ServerMenu::createScene();
	Director::getInstance()->replaceScene(scene);
}

void StartMenu::menuClientCallback(cocos2d::Ref* pSender)
{
	auto scene = ClientMenu::createScene();
	Director::getInstance()->replaceScene(scene);
}

void StartMenu::menuBackCallback(cocos2d::Ref* pSender)
{
	auto scene = HelloWorld::createScene();
	Director::getInstance()->replaceScene(scene);
}

cocos2d::Scene* ServerMenu::createScene()
{
	auto scene = Scene::create();
	auto layer = ServerMenu::create();
	scene->addChild(layer);
	return scene;
}

bool ServerMenu::init()
{
	if (!Layer::init())
	{
		return false;
	}
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto inputbox = ui::EditBox::create(Size(80, 60), ui::Scale9Sprite::create("/picture/editbox.png"));
	inputbox->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height-inputbox->getContentSize().height));
	inputbox->setFontSize(20);
	inputbox->setMaxLength(7);
	inputbox->setFontColor(Color3B::WHITE);
	inputbox->setText("8008");
//	inputbox->setPlaceHolder("8008");
	inputbox->setInputMode(ui::EditBox::InputMode::NUMERIC);
	inputbox->setDelegate(this);

	this->addChild(inputbox, 1);
	
	MenuItemFont::setFontName("Blackwood Castle Shadow");
	MenuItemFont::setFontSize(24);
	auto start_label = MenuItemFont::create("Start Server", CC_CALLBACK_1(ServerMenu::menuStartServerCallback, this));
	auto start_game_label = MenuItemFont::create("Start Game", CC_CALLBACK_1(ServerMenu::menuStartGameCallback, this));
	auto back_label = MenuItemFont::create("Back", CC_CALLBACK_1(ServerMenu::menuBackCallback, this));
	auto menu = Menu::create(start_label, start_game_label,back_label, NULL);
	menu->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));

	menu->alignItemsVerticallyWithPadding(40);
	this->addChild(menu, 1);

	return true;
}

void ServerMenu::menuStartServerCallback(cocos2d::Ref* pSender)
{
	if (!socket_server_)
	{
		socket_server_ = SocketServer::create();
		socket_client_ = SocketClient::create();
		log("create server and client on 8008");
	}

}

void ServerMenu::menuStartGameCallback(cocos2d::Ref* pSender)
{
	if (socket_server_)
	{
		//	auto scene = BattleScene::createScene(socket_client,socket_server);
		socket_server_->button_start();
		auto scene = BattleScene::createScene(socket_client_, socket_server_);
		Director::getInstance()->replaceScene(scene);
		log("start game");
	}
}

void ServerMenu::menuBackCallback(cocos2d::Ref* pSender)
{
	auto scene = StartMenu::createScene();
	Director::getInstance()->replaceScene(scene);
}

void ServerMenu::editBoxReturn(cocos2d::ui::EditBox* editBox)
{
	log(editBox->getText());
	int port = atoi(editBox->getText());
	if (!socket_server_)
	{
		socket_server_ = SocketServer::create(port);
		socket_client_ = SocketClient::create("127.0.0.1", port);
		log("create server and client on %d", port);
	}
}

cocos2d::Scene* ClientMenu::createScene()
{
	auto scene = Scene::create();
	auto layer = create();
	scene->addChild(layer);
	return scene;
}

bool ClientMenu::init()
{
	if (!Layer::init())
	{
		return false;
	}
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto ip_box = ui::EditBox::create(Size(150, 60), ui::Scale9Sprite::create("/picture/editbox.png"));
	ip_box->setPosition(Vec2(origin.x + visibleSize.width / 2-ip_box->getContentSize().width/2,
		origin.y + visibleSize.height - ip_box->getContentSize().height));
	ip_box->setFontSize(20);
	ip_box->setMaxLength(20);
	ip_box->setFontColor(Color3B::WHITE);
	ip_box->setText("127.0.0.1");
	//	inputbox->setPlaceHolder("8008");
//	inputbox->setInputMode(ui::EditBox::InputMode::NUMERIC);
//	iptbox->setDelegate(this);
	ip_box->setTag(1);
	auto port_box = ui::EditBox::create(Size(80, 60), ui::Scale9Sprite::create("/picture/editbox.png"));
	port_box->setPosition(Vec2(origin.x + visibleSize.width / 2 + port_box->getContentSize().width,
		origin.y + visibleSize.height - port_box->getContentSize().height));
	port_box->setFontSize(20);
	port_box->setMaxLength(20);
	port_box->setFontColor(Color3B::WHITE);
	port_box->setText("8008");
	//	inputbox->setPlaceHolder("8008");
	port_box->setInputMode(ui::EditBox::InputMode::NUMERIC);
//	portbox->setDelegate(this);
	port_box->setTag(2);
	this->addChild(ip_box, 1);
	this->addChild(port_box, 1);
	MenuItemFont::setFontName("Blackwood Castle Shadow");
	MenuItemFont::setFontSize(24);
	auto start_label = MenuItemFont::create("Start", CC_CALLBACK_1(ClientMenu::menuStartGameCallback, this));
//	auto start_game_label = MenuItemFont::create("Start Game", CC_CALLBACK_1(ClientMenu::menuStartGameCallback, this));
	auto back_label = MenuItemFont::create("Back", CC_CALLBACK_1(ClientMenu::menuBackCallback, this));
	auto menu = Menu::create(start_label, back_label, NULL);
	menu->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));

	menu->alignItemsVerticallyWithPadding(40);
	this->addChild(menu, 1);

	return true;
}

void ClientMenu::menuStartGameCallback(cocos2d::Ref* pSender)
{
	auto ip_box= static_cast<ui::EditBox*>(getChildByTag(1));
	std::string ip = ip_box->getText();
	auto port_box= static_cast<ui::EditBox*>(getChildByTag(2));
	int port = atoi(port_box->getText());
	log("ip:%s, port:%d", ip.c_str(), port);
	socket_client_ = SocketClient::create(ip,port);

//	std::async(&ClientMenu::wait_start, this);
	wait_start();
}

void ClientMenu::menuBackCallback(cocos2d::Ref* pSender)
{
	auto scene = StartMenu::createScene();
	Director::getInstance()->replaceScene(scene);

}

void ClientMenu::wait_start()
{
	socket_client_->camp();
	log("get the camp");
	log("start game");
	auto scene = BattleScene::createScene(socket_client_, nullptr);
	//	auto scene = BattleScene::createScene(socket_client_);
	Director::getInstance()->replaceScene(scene);
}
