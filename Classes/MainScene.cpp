#include "MainScene.h"
#include <random>

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
	addChild(label,1);

	battle_map = TMXTiledMap::create("map/test_tiled.tmx");
	battle_map->setPosition(0, 0);
	addChild(battle_map, 0);

	auto* init_group = battle_map->getObjectGroup("init_unit");
	auto& objs = init_group->getObjects();
	for (auto& obj : objs)
	{
		auto& dict = obj.asValueMap();
		float cx = dict["x"].asFloat();
		float cy = dict["y"].asFloat();
		int camp = dict["camp"].asInt();

		Airplane* plane;
		if (camp == 0)
		{
			plane = Airplane::createPlane("Picture/airplane_red.png");
			plane->setPosition(cx, cy);
			addChild(plane, 1);
			my_planes.push_back(plane);
		}
		else
			if (camp == 1)
			{
				plane = Airplane::createPlane("Picture/airplane.png");
				plane->setPosition(cx, cy);
				addChild(plane, 1);
				enemy_planes.push_back(plane);
			}
	}

	//std::random_device rd;						//采用非确定性随机数发生器产生随机数种子
	//std::default_random_engine gen(rd());		//采用默认随机数引擎产生随机数
	//std::uniform_int_distribution<> unif_x(origin.x, origin.x + visibleSize.width);		//采用整数均匀分布器产生x均匀分布的随机数
	//std::uniform_int_distribution<> unif_y(origin.y, origin.y + visibleSize.height);		//采用整数均匀分布器产生y均匀分布的随机数

	//for (int i = 0; i < 5; i++)
	//{
	//	Airplane* plane;
	//	plane = Airplane::createPlane("Picture/airplane.png");
	//	plane->setPosition(unif_x(gen), unif_y(gen));
	//	this->addChild(plane, 1);
	//	this->my_planes.push_back(plane);
	//}

	//for (int i = 0; i < 5; i++)
	//{
	//	Airplane* plane;
	//	plane = Airplane::createPlane("Picture/airplane_red.png");
	//	plane->setPosition(unif_x(gen), unif_y(gen));
	//	this->addChild(plane, 1);
	//	this->enemy_planes.push_back(plane);
	//}
	

	this->schedule(schedule_selector(MainScene::update));


	auto listen = EventListenerTouchOneByOne::create();
	listen->onTouchBegan = CC_CALLBACK_2(MainScene::onTouchBegan, this);
	listen->onTouchMoved = CC_CALLBACK_2(MainScene::onTouchMoved, this);
	listen->onTouchEnded = CC_CALLBACK_2(MainScene::onTouchEnded, this);
	listen->setSwallowTouches(true);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listen, this);

	auto keyboard_listener = EventListenerKeyboard::create();
	keyboard_listener->onKeyPressed = CC_CALLBACK_2(MainScene::onKeyPressed, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard_listener, this);
	return true;

}

void MainScene::update(float f)
{
	for (auto plane_it = this->my_planes.begin(); plane_it != this->my_planes.end(); )
		if ((*plane_it)->isAlive() && (*plane_it)->isActive())
			if ((*plane_it)->update())
				this->my_planes.erase(plane_it);
			else
				plane_it++;
		else
			plane_it++;

	auto enemy_begin = enemy_planes.begin();
	auto enemy_end = enemy_planes.end();
	
	for (auto plane_it = this->enemy_planes.begin(); plane_it != this->enemy_planes.end(); )
		if ((*plane_it)->isAlive() && (*plane_it)->isActive())
			if ((*plane_it)->update())
				plane_it = this->enemy_planes.erase(plane_it);
			else
				plane_it++;
		else
			plane_it++;

}

bool MainScene::onTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event*)
{
	
	Point touch = pTouch->getLocation() - getPosition();//返回点击的位置
	this->touchPoint = touch;

	log("Touch Point:(%f, %f)", touch.x, touch.y);

	for (auto my_plane : this->my_planes)
		if (my_plane->getBoundingBox().containsPoint(touch))
		{
			for (auto other_plane : this->my_planes)
				other_plane->unselect();
			my_plane->select();
			this->state = 3;
			return true;
		}

	for (auto enemy_plane : this->enemy_planes)
		if (enemy_plane->isAlive() && enemy_plane->getBoundingBox().containsPoint(touch))
		{
			for (auto my_plane : this->my_planes)
				if (my_plane->isSelected())
				{
					my_plane->setState(2);
					my_plane->setTarget(enemy_plane);
					my_plane->activate();
					enemy_plane->activate();
				}
			this->state = 3;
			return true;
		}

	return true;
}

void MainScene::onTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{
	Point touch = pTouch->getLocation() - getPosition();//返回点击的位置

	if (state != 2)
	{
		this->mouse_rect = DrawNode::create();
		this->addChild(this->mouse_rect, 2);
	}

	this->state = 2;
	

	this->mouse_rect->clear();
	Vec2 mouse_rect_points[4];
	mouse_rect_points[0] = this->touchPoint;
	mouse_rect_points[1] = Vec2(this->touchPoint.x, touch.y);
	mouse_rect_points[2] = touch;
	mouse_rect_points[3] = Vec2(touch.x, this->touchPoint.y);

	//绘制空心多边形
	//填充颜色：Color4F(1, 0, 0, 0), 透明
	//轮廓颜色：Color4F(0, 1, 0, 1), 绿色
	this->mouse_rect->drawPolygon(mouse_rect_points, 4, Color4F(1, 0, 0, 0), 1, Color4F(0, 1, 0, 1));
}

void MainScene::onTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{
	Point touch = pTouch->getLocation() - getPosition();//返回点击的位置

	if (this->state == 3)
	{
		this->state = 1;
		return;
	}

	if (this->state == 1)
	{
		for (auto my_plane : my_planes)
			if (my_plane->isSelected())
			{
				my_plane->setDest(touch);
				my_plane->setState(1);
				my_plane->activate();
			}
	}

	if (this->state == 2)
	{
		this->removeChild(this->mouse_rect);
		this->state = 1;

		Rect select_rect{ MIN(this->touchPoint.x, touch.x), MIN(this->touchPoint.y, touch.y), abs(this->touchPoint.x - touch.x), abs(this->touchPoint.y - touch.y) };
	
		for (auto other_plane : this->my_planes)
			other_plane->unselect();

		for (auto my_plane : my_planes)
		{
			if (select_rect.containsPoint(my_plane->getPosition()))
				my_plane->select();
		}
	}
}

void MainScene::onKeyPressed(EventKeyboard::KeyCode keycode, cocos2d::Event* pEvent)
{
	auto screen_center = getPosition();
	log("Screen Center: (%f, %f)", screen_center.x, screen_center.y);
	log("Visible Origin: (%f, %f)", Director::getInstance()->getVisibleOrigin().x, Director::getInstance()->getVisibleOrigin().y);
	Director::getInstance()->getVisibleSize();
	switch(keycode)
	{
	case EventKeyboard::KeyCode::KEY_W:
		screen_center += Vec2(0, -50);
		if (battle_map->getBoundingBox().containsPoint(Vec2(0,0) - screen_center + Director::getInstance()->getVisibleSize()))
			setPosition(screen_center);
		break;
	case EventKeyboard::KeyCode::KEY_A:
		screen_center += Vec2(50, 0);
		if (battle_map->getBoundingBox().containsPoint(Vec2(0, 0) - screen_center))
			setPosition(screen_center);
		break;
	case EventKeyboard::KeyCode::KEY_S:
		screen_center += Vec2(0, 50);
		if (battle_map->getBoundingBox().containsPoint(Vec2(0, 0) - screen_center))
			setPosition(screen_center);
		break;
	case EventKeyboard::KeyCode::KEY_D:
		screen_center += Vec2(-50, 0);
		if (battle_map->getBoundingBox().containsPoint(Vec2(0, 0) - screen_center + Director::getInstance()->getVisibleSize()))
			setPosition(screen_center);
		break;
	case EventKeyboard::KeyCode::KEY_C:
		Airplane* plane;
		plane = Airplane::createPlane("Picture/airplane_red.png");
		plane->setScale(0.1, 0.1);
		plane->setPosition(Vec2(0, 0) - screen_center + 0.5 * Director::getInstance()->getVisibleSize());
		this->addChild(plane, 1);
		this->enemy_planes.push_back(plane);
		break;
	default:
		break;
	}
}

bool Airplane::update()
{
	if (state == 1)
		if ((dest - getPosition()).getLength() < 10)
		{
			active = 0;
			state = 0;
		}
		else
		{
			auto esp = (dest - getPosition()).getNormalized();
			setPosition(getPosition() + esp * move_speed);
		}
	if (state == 2)
		if ((getPosition() - target->getPosition()).getLength() < atk_range)
			state = 3;
		else
		{
			auto esp = (target->getPosition() - getPosition()).getNormalized();
			setPosition(getPosition() + esp * move_speed);
		}
	if (state == 3)
		if (target->isAlive())
			if (!cd)
			{
				log("target address: %d", target);
				target->decreaseHp(atk);
				cd = atk_period;
			}
			else
				cd--;
		else
		{
			state = 0;
			target = nullptr;
			active = 0;
		}

	if (alive && hp <= 0  && (unsigned int(_textureAtlas) != 0xdddddddd))
	{
		removeFromParent();
		alive = 0;
		return true;
	}

	return false;
}