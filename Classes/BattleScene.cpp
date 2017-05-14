#include "BattleScene.h"

USING_NS_CC;

Scene* BattleScene::createScene()
{
	auto scene = Scene::create();

	auto layer = BattleScene::create();

	scene->addChild(layer);

	return scene;
}

bool BattleScene::init()
{
	if (!Layer::init())
		return false;

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();



	battle_map = TMXTiledMap::create("map/test_tiled.tmx");
	battle_map->setPosition(0, 0);
	addChild(battle_map, 0);

	grid_map = GridMap::create(battle_map);
	grid_map->retain();

	unit_manager = UnitManager::create();
	unit_manager->retain();
	unit_manager->setMessageStack(&msg_stack);
	unit_manager->setTiledMap(battle_map);
	unit_manager->setGridMap(grid_map);

	mouse_rect = DrawNode::create();

	schedule(schedule_selector(BattleScene::update));

	auto* obj_group = battle_map->getObjectGroup("init_unit");
	auto& objs = obj_group->getObjects();

	for (auto& obj : objs)
	{
		auto& dict = obj.asValueMap();
		float cx = dict["x"].asFloat();
		float cy = dict["y"].asFloat();
		int camp = dict["camp"].asInt();

		if (camp == player_id)
			//GameMessage的格式、初始化方法、解释方法有待进一步探讨
			msg_stack.push_back({ Value(1), Value(0), Value(0), Value(cx), Value(cy), Value(camp), Value(1) });
	}

	auto mouse_listener = EventListenerTouchOneByOne::create();
	mouse_listener->onTouchBegan = CC_CALLBACK_2(BattleScene::onTouchBegan, this);
	mouse_listener->onTouchMoved = CC_CALLBACK_2(BattleScene::onTouchMoved, this);
	mouse_listener->onTouchEnded = CC_CALLBACK_2(BattleScene::onTouchEnded, this);
	mouse_listener->setSwallowTouches(true);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(mouse_listener, this);

	auto keyboard_listener = EventListenerKeyboard::create();
	keyboard_listener->onKeyPressed = CC_CALLBACK_2(BattleScene::onKeyPressed, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard_listener, this);

	return true;
}

void BattleScene::initPlayerID()
{
	player_id = 0;
}

void BattleScene::update(float f)
{
	frame_cnt++;

	if (frame_cnt % KEY_FRAME == 0)
	{
		unit_manager->updateUnitsState();
	}
}

bool BattleScene::onTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event*)
{
	return true;
}

void BattleScene::onTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{

}

void BattleScene::onTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{

}

void BattleScene::onKeyPressed(EventKeyboard::KeyCode keycode, cocos2d::Event* pEvent)
{

}