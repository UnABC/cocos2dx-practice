/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
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

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

	//背景のスプライトを生成
    auto sprite = Sprite::create("back.png");
    if (sprite == nullptr)
    {
        problemLoading("'back.png'");
    }
    else
    {
        // position the sprite on the center of the screen
        sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

        // add the sprite as a child to this layer
        this->addChild(sprite, 0);
    }
    //自機のスプライトを生成
	Vec2 playerPos = Vec2(300, 50);
	player = Sprite::create("dot_reimu.png");
	if (player == nullptr)
	{
		problemLoading("'dot_reimu.png'");
	}
	else
	{
		//位置を設定
		player->setPosition(playerPos);
		//スプライトを追加
		this->addChild(player, 0);
        //一部を切り取る
        player->setTextureRect(Rect(34*(playerMode%4), 45*(playerMode/4), 34, 45));

	}
    //弾幕のスプライト(100個ぐらい)を作成
	const int BULLET_NUM = 100;
	for (int i = 0;i < BULLET_NUM;i++)
	{
		auto bullet = Sprite::create("nc165950.png");
		if (bullet == nullptr)
		{
			problemLoading("'nc165950.png'");
		}
		else
		{
			//位置を設定
			bullet->setPosition(Vec2(100.0+CCRANDOM_0_1() * 440.0, 500.0));
			//スプライトを追加
			bullets.pushBack(bullet);
			this->addChild(bullet, 0);
			int ID = CCRANDOM_0_1() * 8;
			bullet->setTextureRect(Rect(20*ID, 0, 20, 20));
			//弾幕のスピードを設定
			bulletSpeed.push_back(0.0);
			//非表示にする
			bullet->setVisible(false);
			bulletAngle.push_back(0.0);
		}
	}

    //キー入力処理
	auto listener = EventListenerKeyboard::create();
	listener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
		Vec2 Pos = player->getPosition();
		switch (keyCode)
		{
		case EventKeyboard::KeyCode::KEY_UP_ARROW:
			stickID |= 1;
			break;
		case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
			stickID |= 2;
			break;
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
			stickID |= 4;
			break;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
			stickID |= 8;
			break;
		}
	};
	listener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {
		switch (keyCode)
		{
		case EventKeyboard::KeyCode::KEY_UP_ARROW:
			stickID ^= 1;
			break;
		case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
			stickID ^= 2;
			break;
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
			stickID ^= 4;
			break;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
			stickID ^= 8;
			break;
		}
	};
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


    this->scheduleUpdate();
    return true;
}

void HelloWorld::update(float dt)
{
	frame++;
	if (GameOver)
	{
		if (frame > GameOverFrame + 120)
		{
			Director::getInstance()->end();
		}
		return;
	}
	//弾幕発生
	if ((frame % 10) == 0)
	{
		int cnt = 0;
		double tmp_x = 100.0 + CCRANDOM_0_1() * 440.0;
		double tmp_y = 400.0 - CCRANDOM_0_1() * 100.0;
		int ID = CCRANDOM_0_1() * 8;
		double speed = 1.5 + CCRANDOM_0_1() * 4.0;

		bool breakFlag = false;
		for (int i = 0; i < 100; i++)
		{
			auto bullet = bullets.at(i);
			if (bullet->isVisible() == false)
			{
				bullet->setVisible(true);
				bullet->setPosition(Vec2(tmp_x, tmp_y));
				bulletAngle.at(i) = (M_PI / 5) * cnt;
				bulletSpeed.at(i) = speed;
				bullet->setTextureRect(Rect(20 * ID, 0, 20, 20));
				cnt++;
				if (cnt > 9)break;
			}
		}
	}
	Vec2 PlayerPos = player->getPosition();
	for (int i = 0; i < 100; i++)
	{
		auto bullet = bullets.at(i);
		if (bullet->isVisible() == false)continue;
        //弾幕のスプライトを移動
		Vec2 Pos = bullet->getPosition();
		Pos.y += bulletSpeed.at(i) * sin(bulletAngle.at(i));
		Pos.x += bulletSpeed.at(i) * cos(bulletAngle.at(i));
		if ((Pos.y < -20)||(Pos.x < -20)||(Pos.x > 660)||(Pos.y > 500))
		{
			bullet->setVisible(false);
		}
		bullet->setPosition(Pos);
		//当たり判定
		double distance = (Pos.x - PlayerPos.x) * (Pos.x - PlayerPos.x) + (Pos.y - PlayerPos.y) * (Pos.y - PlayerPos.y);
		//GAMEOVER
		if (distance < 100.0) {
			log("GAMEOVER:%d",frame);
			GameOver = true;
			GameOverFrame = frame;
			//exit(0);
		}
	}
	//自機のアニメーション
	if ((frame % 6) == 0){
		int playerModeID = 1;
		if (stickID & 4)playerModeID = 0;
		if (stickID & 8)playerModeID = 2;
		playerMode = 4*playerModeID+(playerMode + 1) % 4;
		player->setTextureRect(Rect(34 * (playerMode % 4), 45 * (playerMode / 4), 34, 45));
	}
	//自機の移動
	const int SPEED = 3;
	if ((stickID & 1) && (PlayerPos.y < 480))PlayerPos.y += SPEED;
	if ((stickID & 2) && (PlayerPos.y > 0))  PlayerPos.y -= SPEED;
	if ((stickID & 4) && (PlayerPos.x > 0))  PlayerPos.x -= SPEED;
	if ((stickID & 8) && (PlayerPos.x < 600))PlayerPos.x += SPEED;
	player->setPosition(PlayerPos);
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
