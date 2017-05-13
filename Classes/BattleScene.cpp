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
//TODO: 搬运更多过来	

	return true;
}

void BattleScene::update(float f)
{

}

bool BattleScene::onTouchBegan(cocos2d::Touch*, cocos2d::Event*)
{
	return true;
}

void BattleScene::onTouchMoved(cocos2d::Touch*, cocos2d::Event*)
{

}

void BattleScene::onTouchEnded(cocos2d::Touch*, cocos2d::Event*)
{

}

void BattleScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keycode, cocos2d::Event*)
{
	auto screen_center = getPosition();
	Director::getInstance()->getVisibleSize();
	switch (keycode)
	{
	case EventKeyboard::KeyCode::KEY_W:
		screen_center += Vec2(0, -50);
		if (battle_map->getBoundingBox().containsPoint(Vec2(0, 0) - screen_center + Director::getInstance()->getVisibleSize()))
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
//	case EventKeyboard::KeyCode::KEY_C:
//		Airplane* my_plane;
//		my_plane = Airplane::createPlane("Picture/airplane_red.png");
//		my_plane->setPosition(Vec2(0, 0) - screen_center + 0.5 * Director::getInstance()->getVisibleSize());
//		this->addChild(my_plane, 1);
//		this->my_planes.push_back(my_plane);
//		break;
//	case EventKeyboard::KeyCode::KEY_X:
//		Airplane* plane;
//		plane = Airplane::createPlane("Picture/airplane.png");
//		plane->setPosition(Vec2(0, 0) - screen_center + 0.5 * Director::getInstance()->getVisibleSize());
//		this->addChild(plane, 1);
//		this->enemy_planes.push_back(plane);
//		break;
	default:
		break;
	}
}
