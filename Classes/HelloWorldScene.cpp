#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "BattleScene.h"
#include "SocketServer.h"
#include "SocketClient.h"
#include "cocos-ext.h"


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
//	auto scene = Cli::createScene();
//	Director::getInstance()->replaceScene(scene);
}

void StartMenu::menuBackCallback(cocos2d::Ref* pSender)
{
	auto scene = HelloWorld::createScene();
	Director::getInstance()->replaceScene(scene);
}

cocos2d::Scene* ServerMenu::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = ServerMenu::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
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

	auto inputbox = ui::EditBox::create(Size(150, 60), ui::Scale9Sprite::create("/picture/editbox.png"));
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
		//	auto scene = BattleScene::createScene(socket_server,socket_client);
		//	socket_server_->button_start();
		auto scene = BattleScene::createScene();
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
