#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "BattleScene.h"

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

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
    closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
	
    auto start = Label::createWithTTF("Start Game", "fonts/SIMLI.TTF", 24);
    // position the label on the center of the screen

	auto start_label = MenuItemLabel::create(start,CC_CALLBACK_1(HelloWorld::menuStartCallback,this));
    // add the label as a child to this layer
    start_label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - start->getContentSize().height));

	menu->addChild(start_label, 1);
	
	this->addChild(menu, 1);




    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
	this->scheduleUpdate();
//	this->schedule(schedule_selector(HelloWorld::update),1.0f/60);
    // add the sprite as a child to this layer
    this->addChild(sprite, 0);
    
    return true;
}

void HelloWorld::update(float dt)
{
	
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
  
	unscheduleUpdate();
    Director::getInstance()->end();    
    
}

void HelloWorld::menuStartCallback(cocos2d::Ref* pSender)
{
	auto scene = BattleScene::createScene();
	Director::getInstance()->replaceScene(scene);
}
