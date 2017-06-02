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

bool HelloWorld::init()
{
    
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
    closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    
    auto start = Label::createWithTTF("Start Game", "fonts/SIMLI.TTF", 24);

	auto start_label = MenuItemLabel::create(start,CC_CALLBACK_1(HelloWorld::menuStartCallback,this));
    start_label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - start->getContentSize().height));

	menu->addChild(start_label, 1);
	
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
	auto scene = BattleScene::createScene();
	Director::getInstance()->replaceScene(scene);
}
