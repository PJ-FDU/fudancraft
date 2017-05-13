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

void BattleScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode, cocos2d::Event*)
{

}
