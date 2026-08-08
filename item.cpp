#define _CRT_SECURE_NO_WARNINGS
#include "item.h"
#include "init.h"
#include "barrier.h"
#include "enemy.h"
#include <stdlib.h>
#include "major.h"

extern int gameSpeed;
extern int gameScore; 

Item attackItem;
Item dashItem;
IMAGE imgItem;
IMAGE imgDash;



extern void drawAlphaCrop(int dstX, int dstY, int dstW, int dstH, IMAGE* srcImg, int srcX, int srcY, int srcW, int srcH);

void initItem() 
{

	attackItem.isActive = false;
	loadimage(&imgItem, "res/star.png");
	int origW = imgItem.getwidth();
	int origH = imgItem.getheight();

	attackItem.height = 40;
	attackItem.width = (int)(origW * (40.0 / origH));


	dashItem.isActive = false;
	loadimage(&imgDash, "res/dash.png"); // 记得准备一张冲刺道具图
	int dW = imgDash.getwidth();
	int dH = imgDash.getheight();

	dashItem.height = 40;
	dashItem.width = (int)(dW * (40.0 / dH));
	
	major.dashMode = false;
	major.dashModeTimer = 0;
	major.dashRecover = false;
}

void updateItem()
{
	static int spawnTimer = 0;

	if (!attackItem.isActive && !dashItem.isActive && !major.dashMode)
	{
		spawnTimer++;

		if (spawnTimer >= 300)
		{
			// 避免道具直接刷在刚出生的怪物和障碍物脸上
			bool isSafe = true;

			for (int i = 0; i < MAX_BARRIERS; i++)
			{
				if (barriers[i].isActive && barriers[i].x > win_width - 350) isSafe = false;
			}

			for (int i = 0; i < MAX_ENEMIES; i++)
			{
				if (enemies[i].isActive && enemies[i].x > win_width - 350) isSafe = false;
			}
				
			if (isSafe)
			{
				spawnTimer = 0;
				// 50%概率刷星星，50%概率刷冲刺
				if (rand() % 2 == 0)
				{
					attackItem.isActive = true;
					attackItem.x = win_width;
					attackItem.y = GroundY - 130 - (rand() % 150);
				}

				else
				{
					dashItem.isActive = true;
					dashItem.x = win_width;
					dashItem.y = GroundY - 130 - (rand() % 150);
				}

			}
			else
			{
				spawnTimer = 300; // 不安全则下一帧重新尝试
			}

		}
	}

	if (dashItem.isActive)
	{
		dashItem.x -= gameSpeed;
		if (dashItem.x + dashItem.width < 0)
			dashItem.isActive = false;
	}

	if (attackItem.isActive)
	{
		attackItem.x -= gameSpeed;

		if (attackItem.x + attackItem.width < 0)
			attackItem.isActive = false;
	}

	int item_hX = major.x + major.width * 0.2;
	int item_hW = major.width * 0.6;
	int item_hY = major.y + major.height * 0.05;
	int item_hH = major.height * 0.7;

	if (attackItem.isActive)
	{
		if (item_hX + item_hW > attackItem.x && item_hX < attackItem.x + attackItem.width &&
			item_hY + item_hH > attackItem.y && item_hY < attackItem.y + attackItem.height)
		{

			attackItem.isActive = false;
			major.superMode = true;
			major.superModeTimer = 450;
			gameScore += 200;
		}

	}

	if (dashItem.isActive)
	{
		if (item_hX + item_hW > dashItem.x && item_hX < dashItem.x + dashItem.width &&
			item_hY + item_hH > dashItem.y && item_hY < dashItem.y + dashItem.height)
		{

			dashItem.isActive = false;
			major.dashMode = true;
			major.dashModeTimer = 300; // 冲刺 5 秒
			gameScore += 500;
		}
	}

	//冲刺
	if (major.dashMode)
	{
		major.y = win_height / 2 - major.height / 2;
		major.vy = 0;
		major.state = JUMP;
		major.frameIndex = 4;

		if (major.dashModeTimer <= 0)
		{
			major.dashMode = false;
			major.dashRecover = true;
		}
	}

	if (major.dashRecover)
	{
		major.dashModeTimer = 200;//落地无敌帧
		major.dashModeTimer--;
	}
}

void drawItem()
	{
		if (attackItem.isActive) 
		{
			int origW = imgItem.getwidth();
			int origH = imgItem.getheight();
			drawAlphaCrop(attackItem.x, attackItem.y, attackItem.width, attackItem.height, &imgItem, 0, 0, origW, origH);
		}

		if (dashItem.isActive) 
		{
			int origW = imgDash.getwidth();
			int origH = imgDash.getheight();
			drawAlphaCrop(dashItem.x, dashItem.y, dashItem.width, dashItem.height, &imgDash, 0, 0, origW, origH);
		}

	}
