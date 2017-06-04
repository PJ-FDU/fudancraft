#include "BattleScene.h"

USING_NS_CC;

BattleScene* BattleScene::create(SocketClient* _socket_client, SocketServer* _socket_server)
{
	BattleScene *battle_scene = new (std::nothrow) BattleScene();
	if (battle_scene && battle_scene->init(_socket_client, _socket_server))
	{
		battle_scene->autorelease();
		return battle_scene;
	}
	CC_SAFE_DELETE(battle_scene);

	return nullptr;
}

void BattleScene::create_figher(Ref*)
{

}

Scene* BattleScene::createScene(SocketClient* _socket_client, SocketServer* _socket_server)
{
	auto scene = Scene::create();

	auto layer = BattleScene::create(_socket_client, _socket_server);

	scene->addChild(layer);

	return scene;
}

bool BattleScene::init(SocketClient* _socket_client, SocketServer* _socket_server)
{
	if (!Layer::init())
		return false;

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	socket_client = _socket_client;
	socket_server = _socket_server;

	battle_map = TMXTiledMap::create("map/DefanceOfTheAncient.tmx");
	battle_map->setAnchorPoint(Vec2(0, 0));
	battle_map->setPosition(0, 0);
	addChild(battle_map, 0);

	grid_map = GridMap::create(battle_map);
	grid_map->retain();

	unit_manager = UnitManager::create();
	unit_manager->retain();
	unit_manager->setMessageSet(&msg_set);
	unit_manager->setTiledMap(battle_map);
	unit_manager->setGridMap(grid_map);
	unit_manager->setSocketClient(socket_client);
	//unit_manager->setPlayerID(player_id);

	control_panel_ = ControlPanel::create();
	
	control_panel_->setPosition(Vec2(origin.x + visibleSize.width,
	                                 origin.y + visibleSize.height));
	control_panel_->setFighterCallback([&](Ref*)
		{
			unit_manager->genCreateMessage(1, grid_map->getGridPoint(Vec2(Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleSize().height / 2)));
		}
	);

	addChild(control_panel_,4);

	mouse_rect = DrawNode::create();
	addChild(mouse_rect, 3);

	schedule(schedule_selector(BattleScene::update));

	//unit_manager->initiallyCreateUnits();

	auto mouse_listener = EventListenerTouchOneByOne::create();
	mouse_listener->onTouchBegan = CC_CALLBACK_2(BattleScene::onTouchBegan, this);
	mouse_listener->onTouchMoved = CC_CALLBACK_2(BattleScene::onTouchMoved, this);
	mouse_listener->onTouchEnded = CC_CALLBACK_2(BattleScene::onTouchEnded, this);
	mouse_listener->setSwallowTouches(true);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(mouse_listener, this);

	auto keyboard_listener = EventListenerKeyboard::create();
	keyboard_listener->onKeyPressed = CC_CALLBACK_2(BattleScene::onKeyPressed, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard_listener, this);

	
	initPlayerID();
	unit_manager->setPlayerID(player_id);
	unit_manager->initiallyCreateUnits();
	start_flag = 1;

	return true;
}

void BattleScene::initPlayerID()
{
	player_id = socket_client->camp();
}

bool ControlPanel::init()
{
	if (!Menu::init())
		return false;
	fighter = MenuItemImage::create("/Picture/menu/airplane-menu-up.png",
	                                     "/Picture/menu/airplane-menu-down.png"
	);
	fighter->setAnchorPoint(Vec2(1, 1));
	fighter->setPosition(Menu::getContentSize().width, Menu::getContentSize().height);

	
	Menu::addChild(fighter);
	Menu::alignItemsVertically();
	return true;
}

void ControlPanel::setFighterCallback(std::function<void(Ref*)> callback)
{
	fighter->setCallback(callback);
}



void BattleScene::update(float f)
{
	frame_cnt++;

	if (frame_cnt % KEY_FRAME == 0 && start_flag)
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

	GridPoint map_touch_grid_point = grid_map->getGridPoint(maptouch);

	log("Map Touch Grid Point: (%d, %d)", map_touch_grid_point.x, map_touch_grid_point.y);

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
		if (battle_map->getBoundingBox().containsPoint(Vec2(0, 50) + Director::getInstance()->getVisibleSize()))
			battle_map->setPosition(map_center);
		break;
	case EventKeyboard::KeyCode::KEY_A:
		map_center += Vec2(50, 0);
		if (battle_map->getBoundingBox().containsPoint(Vec2(-50, 0)))
			battle_map->setPosition(map_center);
		break;
	case EventKeyboard::KeyCode::KEY_S:
		map_center += Vec2(0, 50);
		if (battle_map->getBoundingBox().containsPoint(Vec2(0, -50)))
			battle_map->setPosition(map_center);
		break;
	case EventKeyboard::KeyCode::KEY_D:
		map_center += Vec2(-50, 0);
		if (battle_map->getBoundingBox().containsPoint(Vec2(50, 0) + Director::getInstance()->getVisibleSize()))
			battle_map->setPosition(map_center);
		break;
	case EventKeyboard::KeyCode::KEY_K:
		break;
	case EventKeyboard::KeyCode::KEY_X:
		unit_manager->genCreateMessage(1, grid_map->getGridPoint(Vec2(Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleSize().height / 2)));
		break;
	default:
		break;
	}
}