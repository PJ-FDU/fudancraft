#include "BattleScene.h"
#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
USING_NS_CC;

void MouseRect::update(float f)
{
	clear();
	Node* parent = getParent();
	end = touch_end - parent->getPosition();
	drawRect(start, end, Color4F(0, 1, 0, 1));
}

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

void BattleScene::menuBackCallback(cocos2d::Ref* pSender)
{
	unscheduleAllCallbacks();
/*	if (socket_server)
	{	
		socket_server->close();
		delete socket_server;
		socket_server = nullptr;
	}
	socket_client->close();
	delete socket_client;
	socket_client = nullptr;*/
	auto scene = HelloWorld::createScene();
	Director::getInstance()->replaceScene(TransitionSplitCols::create(0.5, scene));
}


void BattleScene::onExit()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	unscheduleAllCallbacks();
	if(socket_client)
	{
		socket_client->close();
		delete socket_client;
		socket_client = nullptr;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	if (socket_server)
	{
		socket_server->close();
		delete socket_server;
		socket_server = nullptr;
	}

	if (_onExitCallback)
		_onExitCallback();

	if (_componentContainer && !_componentContainer->isEmpty())
	{
		_componentContainer->onExit();
	}

	this->pause();

	_running = false;

	for (const auto &child : _children)
		child->onExit();
}

void BattleScene::win()
{
	notice->displayNotice("You Win!");
}

void BattleScene::lose()
{
	notice->displayNotice("You Lose");
	start_flag = 0;
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

	battle_map = TMXTiledMap::create("map/LostTemple.tmx");
	battle_map->setAnchorPoint(Vec2(0, 0));
	addChild(battle_map, 0);

	grid_map = GridMap::create(battle_map);
	grid_map->retain();

	unit_manager = UnitManager::create();
	unit_manager->retain();
	unit_manager->setMessageSet(&msg_set);
	unit_manager->setTiledMap(battle_map);
	unit_manager->setGridMap(grid_map);
	unit_manager->setBattleScene(this);
	unit_manager->setSocketClient(socket_client);
	unit_manager->initRandomGenerator();

	control_panel_ = ControlPanel::create();
	

	control_panel_->setPosition(Vec2(origin.x + visibleSize.width-10,
	                                 origin.y + visibleSize.height/2));

	control_panel_->setFighterCallback([&](Ref*)
	{
		if (money->checkMoney(5000))
		{
			unit_manager->produceInBase(1);
			money->spendMoney(5000);
		}
		else
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("audio/insufficientfound.wav");

	});


	control_panel_->setTankCallback([&](Ref*)
	{
		if (money->checkMoney(4000))
		{
			unit_manager->produceInBase(2);
			money->spendMoney(4000);
		}
		else
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("audio/insufficientfound.wav");

	});
	control_panel_->setSoldierCallback([&](Ref*){
		if (money->checkMoney(1000))
		{
			unit_manager->produceInBase(3);
			money->spendMoney(1000);
		}
		else
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("audio/insufficientfound.wav");

	});
	
	addChild(control_panel_,10);

	mouse_rect = MouseRect::create();
	mouse_rect->setVisible(false);
	battle_map->addChild(mouse_rect, 15);

	schedule(schedule_selector(BattleScene::update));

	auto mouse_listener = EventListenerTouchOneByOne::create();
	mouse_listener->onTouchBegan = CC_CALLBACK_2(BattleScene::onTouchBegan, this);
	mouse_listener->onTouchMoved = CC_CALLBACK_2(BattleScene::onTouchMoved, this);
	mouse_listener->onTouchEnded = CC_CALLBACK_2(BattleScene::onTouchEnded, this);
	mouse_listener->setSwallowTouches(true);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(mouse_listener, this);

	auto mouse_event = EventListenerMouse::create();
	mouse_event->onMouseMove = [&](Event *event)
	{
		EventMouse* e = static_cast<EventMouse*>(event);
		crusor_position = Vec2(e->getCursorX(), e->getCursorY());
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(mouse_event, 1);


	auto keyboard_listener = EventListenerKeyboard::create();
	keyboard_listener->onKeyPressed = CC_CALLBACK_2(BattleScene::onKeyPressed, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard_listener, this);

	
	initPlayerID();
	unit_manager->setPlayerID(player_id);
	unit_manager->initiallyCreateUnits();

	auto *money_icon = Sprite::create("Picture/ui/gold.png");
	addChild(money_icon, 40);
	money_icon->setPosition(visibleSize.width - 130, 20);
	money_icon->setScale(0.03);

	money = Money::create();
	addChild(money, 40);
	money->setPosition(visibleSize.width - 80, 20);
	money->schedule(schedule_selector(Money::update));

	notice = Notice::create();
	addChild(notice, 40);
	notice->setAnchorPoint(Vec2(0, 0));
	notice->setPosition(10, visibleSize.height / 2);
	notice->setScale(0.7);
	notice->schedule(schedule_selector(Notice::update));
	unit_manager->setNotice(notice);
	
	auto back_label = MenuItemFont::create("Back", CC_CALLBACK_1(BattleScene::menuBackCallback, this));
	back_label->setPosition(Vec2(origin.x + visibleSize.width - back_label->getContentSize().width,
		origin.y + visibleSize.height-back_label->getContentSize().height));
	back_label->setColor(Color3B(255, 236, 139));
//	back_label->setFontSize(16);
	back_label->setFontName("fonts/Blackwood Castle.ttf");
	auto menu = Menu::create(back_label, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 20);

	CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.3);
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("audio/killbill.wav",true);
	log("if back ground music playing %d", CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying());
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("audio/battlefieldcontrol.wav");



	start_flag = 1;

	return true;
}

void BattleScene::focusOnBase()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	GridPoint base_gp = unit_manager->getBasePosition();
	Vec2 base_vec = grid_map->getPoint(base_gp) - visibleSize / 2;
	if (battle_map->getBoundingBox().size.height < base_vec.y + visibleSize.height)
		base_vec.y = battle_map->getBoundingBox().size.height - visibleSize.height;
	if (battle_map->getBoundingBox().size.width < base_vec.x + visibleSize.width)
		base_vec.x = battle_map->getBoundingBox().size.width - visibleSize.width;
	if (base_vec.x < 0)
		base_vec.x = 0;
	if (base_vec.y < 0)
		base_vec.y = 0;
	battle_map->setPosition(Point(0, 0) - base_vec);
}

void BattleScene::destroyReward(int destroyed_type)
{
	std::vector<int> reward_list = { 0, 5000, 4000, 2000, 0, 0, 10000 };
	money->increaseMoney(reward_list[destroyed_type]);
	money->updateMoneyDisplay();
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

	fighter->setScale(0.8);
	fighter->setAnchorPoint(Vec2(1, 1));
	fighter->setPosition(Menu::getContentSize().width, Menu::getContentSize().height);
	

	tank = MenuItemImage::create("/Picture/menu/tank-menu-up.png",
	                                     "/Picture/menu/tank-menu-down.png"
	);
	tank->setScale(0.8);
	tank->setAnchorPoint(Vec2(1, 1));
	tank->setPosition(Menu::getContentSize().width,
	                     Menu::getContentSize().height - tank->getContentSize().height);

	soldier = MenuItemImage::create("/Picture/menu/soldier-menu-up.png",
	                                "/Picture/menu/soldier-menu-down.png"
	);
	soldier->setScale(0.8);
	soldier->setAnchorPoint(Vec2(1, 1));
	soldier->setPosition(Menu::getContentSize().width,
	                     Menu::getContentSize().height - 2 * soldier->getContentSize().height);

	
	Menu::addChild(fighter);
	Menu::addChild(tank);
	Menu::addChild(soldier);

	Menu::alignItemsVertically();
	return true;
}

void ControlPanel::setFighterCallback(std::function<void(Ref*)> callback)
{
	fighter->setCallback(callback);
}


void ControlPanel::setTankCallback(std::function<void(Ref*)> callback)
{
	tank->setCallback(callback);
}

void ControlPanel::setSoldierCallback(std::function<void(Ref*)> callback)
{
	soldier->setCallback(callback);
}



void BattleScene::update(float f)
{
	frame_cnt++;
	if (socket_client->error() || (socket_server&&socket_server->error()))
		menuBackCallback(nullptr);
	scrollMap();
	if (frame_cnt % KEY_FRAME == 0 && start_flag)
	{
		unit_manager->updateUnitsState();
	}
}

bool BattleScene::onTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event*)
{

	Point touch = pTouch->getLocation();//杩斿洖鐐瑰嚮鐨勪綅缃?
	last_touch = touch;
	mouse_rect->start = touch - battle_map->getPosition();
	mouse_rect->touch_start = touch;
	mouse_rect->touch_end = touch;
	mouse_rect->schedule(schedule_selector(MouseRect::update));

	return true;
}

void BattleScene::onTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{

	Point touch = pTouch->getLocation();//杩斿洖鐐瑰嚮鐨勪綅缃?

	mouse_rect->touch_end = touch;
	mouse_rect->clear();
	mouse_rect->setVisible(true);
}

void BattleScene::onTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{

	Point touch = pTouch->getLocation();//杩斿洖鐐瑰嚮鐨勪綅缃?

	mouse_rect->setVisible(false);
	if (mouse_rect->isScheduled(schedule_selector(MouseRect::update)))
		mouse_rect->unschedule(schedule_selector(MouseRect::update));
	mouse_rect->end = touch - battle_map->getPosition();

	Point maptouch = mouse_rect->end;
	Point last_maptouch = mouse_rect->start;

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

void BattleScene::scrollMap()
{
	auto map_center = battle_map->getPosition();
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	int horizontal_state, vertical_state;
	horizontal_state = (origin.x + visibleSize.width - BOX_EDGE_WITDH_SMALL < crusor_position.x)
		+ (origin.x + visibleSize.width - BOX_EDGE_WITDH < crusor_position.x)
		- (origin.x + BOX_EDGE_WITDH_SMALL > crusor_position.x)
		- (origin.x + BOX_EDGE_WITDH > crusor_position.x);
	//	log("horizontal state: %d", horizontal_state);
	vertical_state = (origin.y + visibleSize.height - BOX_EDGE_WITDH_SMALL < crusor_position.y)
		+ (origin.y + visibleSize.height - BOX_EDGE_WITDH < crusor_position.y)
		- (origin.y + BOX_EDGE_WITDH_SMALL > crusor_position.y)
		- (origin.y + BOX_EDGE_WITDH > crusor_position.y);
	Vec2 scroll(0,0);
/*	if(horizontal_state == -1)
		scroll += Vec2(SCROLL_LENGTH,0 );
	else if(horizontal_state == 1)
		scroll += Vec2(-SCROLL_LENGTH, 0);
	if(vertical_state == -1)
		scroll += Vec2(0, SCROLL_LENGTH);
	else if(vertical_state == 1)
		scroll += Vec2(0, -SCROLL_LENGTH);*/
	scroll += Vec2(-SCROLL_LENGTH, 0)*horizontal_state;
	scroll += Vec2(0, -SCROLL_LENGTH)*vertical_state;
	map_center += scroll;
	if (battle_map->getBoundingBox().containsPoint((-scroll) + Director::getInstance()->getVisibleSize())
		&& battle_map->getBoundingBox().containsPoint(-scroll))
		battle_map->setPosition(map_center);
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
	case EventKeyboard::KeyCode::KEY_SPACE:
		focusOnBase();
		break;
	default:
		break;
	}
}

void Money::update(float f)
{
	if (++timer % inc_prd == 0)
	{
		money += inc_amt;
		updateMoneyDisplay();
	}
}

bool Money::init()
{
	money = INITIAL_BUDGET;
	char money_str[30];
	sprintf(money_str, "%d", money);
	return initWithString(money_str, "fonts/MoneyFont.fnt");
}

void Money::updateMoneyDisplay()
{
	char money_str[30];
	sprintf(money_str, "%d", money);
	setString(money_str);
}

void Money::spendMoney(int cost)
{
	money -= cost;
	updateMoneyDisplay();
}

void Money::increaseMoney(int amount)
{
	money += amount;
}

bool Money::checkMoney(int cost) const
{
	return(money >= cost);
}

void Notice::update(float f)
{
	if (++timer == ntc_life)
	{
		setString("");
		ntc_life = 0;
		timer = 0;
		unschedule(schedule_selector(Notice::update));
	}
}

void Notice::displayNotice(std::string ntc, int _ntc_life)
{
	setString(ntc);
	ntc_life = _ntc_life;
	timer = 0;
	if (!isScheduled(schedule_selector(Notice::update)))
		schedule(schedule_selector(Notice::update));
}

void Notice::displayNotice(std::string ntc)
{
	setString(ntc);
	ntc_life = 0;
	timer = 0;
	if (isScheduled(schedule_selector(Notice::update)))
		unschedule(schedule_selector(Notice::update));
}

bool Notice::init()
{
	ntc_life = 100;
	return initWithString("Welcome to FudanCraft!", "fonts/NoticeFont.fnt");
}
