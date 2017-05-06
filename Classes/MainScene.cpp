#include "MainScene.h"

USING_NS_CC;

Scene* MainScene::createScene()
{
	auto scene = Scene::create();

	auto layer = MainScene::create();

	scene->addChild(layer);
	
	return scene;
}

bool MainScene::init()
{
	if( !Layer::init() )
		return false;


	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();


	auto label = Label::createWithTTF("世界,你好", "fonts/SIMLI.TTF", 24);
	label->setColor(Color3B::RED);
	label->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - label->getContentSize().height));
	this->addChild(label,0);
	airplane = Sprite::create("Picture/airplane.png");
//	 auto airplane = Sprite::create("Picture/airplane.png");

	airplane->setScale(0.1, 0.1);
	airplane->setPosition(200, 200);
	airplane->setTag(12);
	this->addChild(airplane,1);
	this->schedule(schedule_selector(MainScene::update));

	rect = DrawNode::create();

	this->addChild(rect, 2);
//	this->setTouchEnabled(true);
	auto listen = EventListenerTouchOneByOne::create();
	listen->onTouchBegan = CC_CALLBACK_2(MainScene::onTouchBegan, this);
	listen->onTouchMoved = CC_CALLBACK_2(MainScene::onTouchMoved, this);
	listen->onTouchEnded = CC_CALLBACK_2(MainScene::onTouchEnded, this);
//	this->addTouchListener();
	listen->setSwallowTouches(true);
//	this->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listen, this);

	return true;

}

void MainScene::update(float f)
{
//	
//	auto sp = this->getChildren();//获取这个层中所有的孩子,也就是所有的精灵，看不懂？别逗我了，点进去看源码吧，注意它的返回值类型。  
//	for (auto a : sp)
//	{
//		if(a->getBoundingBox().containsPoint(touchPoint))
//			continue;
//		auto esp = Point(touchPoint.x - a->getPosition().x, touchPoint.y - a->getPosition().y).getNormalized();
//		a->setPosition(a->getPosition()+esp*moveSpeed);
//		
//	}
	if (!airplane->getBoundingBox().containsPoint(touchPoint))
	{
		auto esp = (touchPoint - airplane->getPosition()).getNormalized();
		airplane->setPosition(airplane->getPosition() + esp*moveSpeed);
	}
//	log("f:%f", f);

//	log("touch point:%f,%f",touchPoint.x,touchPoint.y);
}

bool MainScene::onTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event*)
{
	this->touchPoint = pTouch->getLocation();
	touch_begin = pTouch->getLocation();
	log("touch begin");
	return true;
}

void MainScene::onTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{
	rect->clear();
	rect->drawRect(touch_begin, pTouch->getLocation(), Color4F(1.0, 0, 0, 1.0));

	auto airplane = getChildByTag(12);
	auto touch = pTouch->getLocation();//返回点击的位置  
	auto rectPlayer = airplane->getBoundingBox();//看返回值类型，应该知道这个是飞机所占矩形区域的大小  

	if (rectPlayer.containsPoint(touch)) {//如果点击的点在这个矩形区域内就可以对飞机进行拖动  
		auto temp = pTouch->getDelta();
		airplane->setPosition(airplane->getPosition() + temp);
	}
}

void MainScene::onTouchEnded(cocos2d::Touch*, cocos2d::Event*)
{
	rect->clear();

	log("touch end");
}
