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

	initPlayerID();

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
	unit_manager->setPlayerID(player_id);

	mouse_rect = DrawNode::create();
	addChild(mouse_rect, 3);

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
	Point touch = pTouch->getLocation();//返回点击的位置
	last_touch = touch;

	return true;
}

void BattleScene::onTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{
	Point touch = pTouch->getLocation();//返回点击的位置

	mouse_rect->clear();
	Vec2 mouse_rect_points[4];
	mouse_rect_points[0] = last_touch;
	mouse_rect_points[1] = Vec2(last_touch.x, touch.y);
	mouse_rect_points[2] = touch;
	mouse_rect_points[3] = Vec2(touch.x, last_touch.y);

	//绘制空心多边形
	//填充颜色：Color4F(1, 0, 0, 0), 透明
	//轮廓颜色：Color4F(0, 1, 0, 1), 绿色
	mouse_rect->drawPolygon(mouse_rect_points, 4, Color4F(1, 0, 0, 0), 1, Color4F(0, 1, 0, 1));
	mouse_rect->setVisible(true);
}

void BattleScene::onTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{
	Point touch = pTouch->getLocation();//返回点击的位置

	mouse_rect->setVisible(false);

	Point maptouch = touch - battle_map->getPosition();
	Point last_maptouch = last_touch - battle_map->getPosition();

	if ((maptouch - last_maptouch).length() < MIN_SELECT_RECT_SIZE)
		unit_manager->selectUnits(maptouch);
	else
	{
		Rect select_rect{ MIN(last_maptouch.x, maptouch.x), MIN(last_maptouch.y, maptouch.y),
			abs(last_maptouch.x - maptouch.x), abs(last_maptouch.y - maptouch.y) };
		unit_manager->selectUnits(select_rect);
	}
}

void BattleScene::onKeyPressed(EventKeyboard::KeyCode keycode, cocos2d::Event* pEvent)
{
	auto map_center = battle_map->getPosition();
	Director::getInstance()->getVisibleSize();
	switch (keycode)
	{
	case EventKeyboard::KeyCode::KEY_W:
		map_center += Vec2(0, -50);
		if (battle_map->getBoundingBox().containsPoint(Vec2(0, 0) - map_center + Director::getInstance()->getVisibleSize()))
			battle_map->setPosition(map_center);
		break;
	case EventKeyboard::KeyCode::KEY_A:
		map_center += Vec2(50, 0);
		if (battle_map->getBoundingBox().containsPoint(Vec2(0, 0) - map_center))
			battle_map->setPosition(map_center);
		break;
	case EventKeyboard::KeyCode::KEY_S:
		map_center += Vec2(0, 50);
		if (battle_map->getBoundingBox().containsPoint(Vec2(0, 0) - map_center))
			battle_map->setPosition(map_center);
		break;
	default:
		break;
	}
}