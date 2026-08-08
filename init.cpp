#define _CRT_SECURE_NO_WARNINGS
#include "init.h"
#include "major.h" 
#include <stdio.h>


extern int gameSpeed;
IMAGE imgBg;

int bgX = 0;


// 初始化
void initGame()
{

	initgraph(win_width, win_height);
	loadimage(&imgBg, "res/bg.jpg", win_width, win_height, true);
	bgX = 0;
	initMajor();
}	

void updateBg()
{
	putimage(bgX, 0, &imgBg);
	putimage(bgX + win_width, 0, &imgBg);

//背景倒计时
	if (major.dashMode || major.superMode) 
	{
		char strCount[16];
		if (major.dashMode)
		{
			sprintf(strCount, "%d", (major.dashModeTimer + 59) / 60);//向上取整
		}

		else 
		{
			sprintf(strCount, "%d", (major.superModeTimer + 59) / 60);
		}

		settextcolor(RGB(220, 225, 230));

		settextstyle(200, 0, "Impact", 0, 0, FW_BOLD, false, false, false);

		setbkmode(TRANSPARENT);

		outtextxy(win_width / 2 - 50, win_height / 2 - 140, strCount);
	}
}

void flyBg() 
{
	bgX -= gameSpeed;
	if (bgX <= -win_width)
	{
		bgX = 0;
	}
}

void drawItemUI()
{

	if (major.superMode)
	{
		settextcolor(RGB(220, 20, 60)); 
		settextstyle(32, 0, "黑体", 0, 0, FW_BOLD, false, false, false);
		outtextxy(20, 85, "无敌状态");
	}
	else if (major.dashMode)
	{
		settextcolor(RGB(0, 191, 255)); 
		settextstyle(32, 0, "黑体", 0, 0, FW_BOLD, false, false, false);
		outtextxy(20, 85, "超级冲刺");
	}
}