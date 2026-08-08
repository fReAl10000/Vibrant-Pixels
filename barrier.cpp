#include "barrier.h"
#include "init.h" 
#include "major.h" 
#include <stdlib.h>
#include "enemy.h" 

extern int gameSpeed;

Barrier barriers[MAX_BARRIERS];
IMAGE imgBarrier;
IMAGE imgPit;

void initBarrier()
{
	loadimage(&imgBarrier, "res/barrier.png");
	loadimage(&imgPit, "res/pit.png");

	for (int i = 0; i < MAX_BARRIERS; i++) 
	{
		barriers[i].isActive = false;
	}

}

void updateBarrier()
{
	static int spawnTimer = 0;
	spawnTimer++;


	if (spawnTimer >= 100)
	{//刷新

		// bug1:随机刷新容易刷出必死地形
		// 留出 safeDist 的安全起跳空间
		bool isSafe = true;

		for (int j = 0; j < MAX_ENEMIES; j++)
		{
			if (enemies[j].isActive && (enemies[j].x + enemies[j].width) > win_width - safeDist)
			{
				isSafe = false; break;
			}

		}

		for (int j = 0; j < MAX_BARRIERS; j++)
		{

			if (barriers[j].isActive && (barriers[j].x + barriers[j].width) > win_width - safeDist)
			{
				isSafe = false; break;
			}

		}
			
		if (isSafe)
		{
			spawnTimer = 0;
			for (int i = 0; i < MAX_BARRIERS; i++) 
			{
				if (!barriers[i].isActive) 
				{
					barriers[i].isActive = true;
					barriers[i].type = (BarrierType)(rand() % 3);//生成一个很大的随机数取余，余下来状态
					barriers[i].x = win_width;

					if (barriers[i].type == barGround) 
					{
						barriers[i].width = 60;
						barriers[i].height = 80;
						barriers[i].y = GroundY - barriers[i].height;
					}

					else if (barriers[i].type == barCeiling) 
					{
						barriers[i].width = 100;	
						barriers[i].y = 0;

						barriers[i].height = GroundY - 100;
					}

					else if (barriers[i].type == barPit) 
					{
						barriers[i].width = 300;
						barriers[i].height = 200;
						barriers[i].y = GroundY;
					}

					break;
				}
			}
		}

		else 
		{
			spawnTimer = 100;
		}

	}

	// 障碍物移动与回收
	for (int i = 0; i < MAX_BARRIERS; i++) 
	{
		if (barriers[i].isActive)
		{
			barriers[i].x -= gameSpeed;

			if (barriers[i].x + barriers[i].width < 0)
			{
				barriers[i].isActive = false;
			}

		}
	}
}

void drawBarrier()
{
	for (int i = 0; i < MAX_BARRIERS; i++)
	{
		if (barriers[i].isActive)
		{

			// 渲染坑
			if (barriers[i].type == barPit)
			{
				//居中裁剪算法
				int origW = imgPit.getwidth();
				int origH = imgPit.getheight();

				int pitW = barriers[i].width;
				int pitH = win_height - GroundY;

				double scaleW = (double)pitW / origW;//放大倍数
				double scaleH = (double)pitH / origH;

				double finalScale = (scaleW > scaleH) ? scaleW : scaleH;

				int srcW = (int)(pitW / finalScale);
				int srcH = (int)(pitH / finalScale);

				int srcX = (origW - srcW) / 2;
				int srcY = (origH - srcH) / 2;

				drawAlphaCrop(barriers[i].x, GroundY, pitW, pitH, &imgPit, srcX, srcY, srcW, srcH);
			}

			//渲染地面和挡板

			else
			{
				int origW = imgBarrier.getwidth();
				int origH = imgBarrier.getheight();

				drawAlphaCrop(barriers[i].x, barriers[i].y, barriers[i].width, barriers[i].height, &imgBarrier, 0, 0, origW, origH);
			}
		}
	}
}