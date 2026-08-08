#define _CRT_SECURE_NO_WARNINGS
#include "enemy.h"
#include "init.h"
#include "major.h"
#include <stdio.h>
#include <stdlib.h> 
#include "barrier.h" 

extern int gameSpeed;

Enemy enemies[MAX_ENEMIES];
IMAGE imgEnemyAni[21];

void initEnemy()
{
	for (int i = 0; i < 21; i++)
	{
		char path[64];
		sprintf(path, "res/enemy/tentakelding_%03d.png", i + 1);
		loadimage(&imgEnemyAni[i], path);
	}

	for (int i = 0; i < MAX_ENEMIES; i++) 
	{
		enemies[i].isActive = false;
		enemies[i].frameIndex = 0;
	}

}

void updateEnemy() 
{
	static int spawnTimer = 0;
	spawnTimer++;

	int spawnLimit = major.superMode ? 40 : 80;

	if (spawnTimer >= spawnLimit) 
	{
		int loc = rand() % 3; //3个刷新位置
		bool hasSpawned = false;
		 
		if (loc == 2) //坑里刷新判定
		{
			for (int j = 0; j < MAX_BARRIERS; j++)
			{
				if (barriers[j].isActive && barriers[j].type == barPit && barriers[j].x > win_width - 150)
				{
					spawnTimer = 0;

					for (int i = 0; i < MAX_ENEMIES; i++)
					{
						if (!enemies[i].isActive) 
						{
							enemies[i].isActive = true;
							enemies[i].frameIndex = 0;
							double scale = 120.0 / imgEnemyAni[0].getwidth();//等比缩放
							enemies[i].width = 120;
							enemies[i].height = (int)(imgEnemyAni[0].getheight() * scale);
							enemies[i].x = barriers[j].x + 60;
							enemies[i].y = GroundY + 40;
							hasSpawned = true;
							break;
						}
					}
					break; 
				}
			}

			if (!hasSpawned)//不能在坑里刷就改为在地上或者天上刷
			{
				loc = rand() % 2;
			}

		}

		//海、陆生成
		if (!hasSpawned)
		{
			bool isSafe = true;
			for (int j = 0; j < MAX_BARRIERS; j++) 
			{
				if (barriers[j].isActive && (barriers[j].x + barriers[j].width) > win_width - safeDist )
				{
					isSafe = false; break;
				}

			}

			for (int j = 0; j < MAX_ENEMIES; j++)
			{
				if (enemies[j].isActive && (enemies[j].x + enemies[j].width) > win_width - safeDist)
				{
					isSafe = false; break;
				}

			}

			if (isSafe)
			{
				spawnTimer = 0;
				for (int i = 0; i < MAX_ENEMIES; i++)
				{

					if (!enemies[i].isActive)
					{
						enemies[i].isActive = true;
						enemies[i].frameIndex = 0;
						double scale = 120.0 / imgEnemyAni[0].getwidth();
						enemies[i].width = 120;
						enemies[i].height = (int)(imgEnemyAni[0].getheight() * scale);	

						enemies[i].x = win_width;
						if (loc == 0) enemies[i].y = GroundY - enemies[i].height - 180;
						else if (loc == 1) enemies[i].y = GroundY - enemies[i].height - 10;
						break;
					}
				}
			}
			else
			{
				spawnTimer = spawnLimit;
			}

		}
	}

	// 小怪的移动与回收
	for (int i = 0; i < MAX_ENEMIES; i++) 
	{
		if (enemies[i].isActive) 
		{
			enemies[i].x -= gameSpeed; 
			if (enemies[i].x + enemies[i].width < 0)
			{
				enemies[i].isActive = false;
			}

		}
	}
}

void drawEnemy()
{
	static int loopCounter = 0;
	loopCounter++;
	bool changeFrame = (loopCounter % 4 == 0);

	for (int i = 0; i < MAX_ENEMIES; i++) 
	{
		if (enemies[i].isActive)
		{
			int currentFrame = enemies[i].frameIndex;
			IMAGE* curImg = &imgEnemyAni[currentFrame];

			int origW = curImg->getwidth();
			int origH = curImg->getheight();

			double scale = 120.0 / origW;
			int dstW = 120;
			int dstH = (int)(origH * scale);

			drawAlphaCrop(enemies[i].x, enemies[i].y, dstW, dstH, curImg, 0, 0, origW, origH);

			if (changeFrame) //小怪播放
			{
				enemies[i].frameIndex++;
				if (enemies[i].frameIndex >= 21) 
				{
					enemies[i].frameIndex = 0;
				}

			}
		}
	}
}