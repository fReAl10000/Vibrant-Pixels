#define _CRT_SECURE_NO_WARNINGS
#include "init.h"
#include "major.h"
#include "enemy.h" 
#include <stdio.h>
#include"barrier.h"
#include "item.h"

bool isGameOver = false;
int gameScore = 0;
int gameDistance = 0;
int gameSpeed = 6;


bool enemyCollision() 
{

	int enemy_hX = major.x + major.width * 0.35;
	int enemy_hW = major.width * 0.3;
	int enemy_hY = major.y + major.height * 0.1;
	int enemy_hH = major.height * 0.8;


	if (major.dashMode)
	{
		enemy_hX = major.x - 30;
		enemy_hW = major.width + 60;
		enemy_hY = 0;      
		enemy_hH = win_height;
	}

	else if (major.superMode) 
	{
		enemy_hX = major.x - 30;
		enemy_hW = major.width + 60;
		enemy_hY = major.y - 30;
		enemy_hH = major.height + 60;
	}

	else if (major.state == JUMP)
	{
		enemy_hY = major.y + major.height * 0.2;
		enemy_hH = major.height * 0.5;
	}

	else if (major.state == SLIDE)
	{
		enemy_hH = major.height * 0.4;
		enemy_hY = (major.y + major.height) - enemy_hH - major.height * 0.05;
	}

	for (int i = 0; i < MAX_ENEMIES; i++) 
	{
		if (enemies[i].isActive)
		{
			int enemyBoxX = enemies[i].x + enemies[i].width * 0.25;
			int enemyBoxW = enemies[i].width * 0.5;
			int enemyBoxY = enemies[i].y + enemies[i].height * 0.25;
			int enemyBoxH = enemies[i].height * 0.5;

			if (enemy_hX + enemy_hW > enemyBoxX && enemy_hX < enemyBoxX + enemyBoxW &&
				enemy_hY + enemy_hH > enemyBoxY && enemy_hY < enemyBoxY + enemyBoxH)
			{


				if (major.dashMode || major.superMode) 
				{

					enemies[i].isActive = false;
					gameScore += 100;
				}

				else if (major.dashRecover || major.dashModeTimer > 0)
				{//落地无敌
					continue;
				}

				else
				{
					return true;
				}

			}
		}
	}
	return false;
}

void barrierCollision()
{
	//障碍物碰撞
	if (major.dashMode)
	{
		int dash_hX = major.x - 30;
		int dash_hW = major.width + 60;

		for (int i = 0; i < MAX_BARRIERS; i++) 
		{
			if (barriers[i].isActive)
			{
				int bx = barriers[i].x;
				int bw = barriers[i].width;

				if (dash_hX + dash_hW > bx && dash_hX < bx + bw) 
				{
					barriers[i].isActive = false;
					gameScore += 50; 
				}

			}
		}
		return; 
	}

	int bar_offsetX = major.width * 0.3;
	int bar_bodyW = major.width * 0.4;
	int bar_hX = major.x + bar_offsetX;
	int bar_hW = bar_bodyW;

	int bar_hY = major.y + major.height * 0.1;
	int bar_hH = major.height * 0.85; 

	if (major.state == SLIDE)
	{
		bar_hH = major.height * 0.45;
		bar_hY = (major.y + major.height) - bar_hH;
	}

	bool majorBlock = false;

	for (int i = 0; i < MAX_BARRIERS; i++) 
	{
		if (barriers[i].isActive && barriers[i].type != barPit)
		{
			int bx = barriers[i].x;
			int by = barriers[i].y;
			int bw = barriers[i].width;
			int bh = barriers[i].height;

			// 跨栏容错
		
			int footTolerance = 10;

			if (bar_hX + bar_hW > bx && bar_hX < bx + bw && (bar_hY + bar_hH - footTolerance) > by && bar_hY < by + bh) {
				major.x = bx - bar_bodyW - bar_offsetX;//卡死代码5
				majorBlock = true;
			}
		}
	}

	if (!majorBlock && major.x < 100) 
	{
		major.x += 2;
		if (major.x > 100) 
		{
			major.x = 100;
		}
	}

	if (major.x + major.width / 2 <= 0)
	{
		isGameOver = true;
	}

	if (major.y + major.height >= win_height) 
	{
		major.y = win_height - major.height;
		isGameOver = true;
	}
}

int main(void)
{
	initGame();
	initEnemy();
	initBarrier();
	BeginBatchDraw();
	initItem();
	

	while (1)
	{
		cleardevice();

		if (!isGameOver)
		{
			int baseSpeed = gameSpeed;

			if (major.dashMode)
			{
				gameSpeed = baseSpeed * 2;
			}

			if (!isGameOver)//刷新
			{
				flyBg();
				updateMajor();
				updateEnemy();
				updateBarrier();
				updateItem();
				barrierCollision();

				gameSpeed = baseSpeed;

				gameDistance++;


				// 加速
				if (gameDistance > 0 && gameDistance % 140 == 0 && gameSpeed < 10)
				{
					gameSpeed++;
				}
				if (enemyCollision())
				{
					isGameOver = true;
				}
			}
		}

			updateBg();
			drawItem();
			drawBarrier();
			drawEnemy();
			drawMajor();

			//得分栏与距离
			char strScore[64];
			char strDistance[64];
			sprintf(strScore, "SCORE: %05d", gameScore);
			sprintf(strDistance, "DIST : %d m", gameDistance / 6); // 帧折算米

			settextcolor(YELLOW);
			settextstyle(24, 0, "Consolas");
			setbkmode(TRANSPARENT);

			outtextxy(20, 20, strScore);
			settextcolor(BLACK);
			outtextxy(20, 50, strDistance); 

			drawItemUI();

			// 死亡动画
			if (isGameOver) 
			{
		
				settextcolor(RGB(220, 50, 50));
				settextstyle(100, 0, "Impact", 0, 0, FW_BOLD, false, false, false);
				setbkmode(TRANSPARENT);
				outtextxy(win_width / 2 - 230, win_height / 2 - 160, "GAME OVER");

			// 结算得分距离
				char finalScore[64];
				char finalDist[64];
				sprintf(finalScore, "FINAL SCORE : %d", gameScore);
				sprintf(finalDist, "TOTAL DIST  : %d m", gameDistance / 6);

				settextcolor(WHITE);

				settextstyle(36, 0, "Consolas", 0, 0, FW_BOLD, false, false, false);
				outtextxy(win_width / 2 - 170, win_height / 2 - 10, finalScore);
				outtextxy(win_width / 2 - 170, win_height / 2 + 40, finalDist);


				settextcolor(YELLOW);
				settextstyle(24, 0, "黑体", 0, 0, FW_NORMAL, false, false, false);
				outtextxy(win_width / 2 - 250, win_height / 2 + 120, " [空格键] 再来一局 |  [ESC] 下次一定");

			//重开
				if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
				{
					break; 
				}

				if (GetAsyncKeyState(VK_SPACE) & 0x8000)
				{

					isGameOver = false;
					gameScore = 0;
					gameDistance = 0;
					gameSpeed = 6;  

					initMajor();
					initEnemy();
					initBarrier();
					initItem();

					Sleep(200);
				}
			}
			FlushBatchDraw();
			Sleep(10);

		}
	
	EndBatchDraw();
	return 0;
	}	