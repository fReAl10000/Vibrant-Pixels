#include "major.h"
#include"init.h"
#pragma comment(lib, "msimg32.lib") // 引入透明渲染的系统多媒体库
#include"barrier.h"

Major major;
IMAGE imgMajorRun;
IMAGE imgMajorJump;
IMAGE imgMajorSlide;

// 透明抠图函数
void drawAlphaCrop(int dstX, int dstY, int dstW, int dstH, IMAGE* srcImg, int srcX, int srcY, int srcW, int srcH) 
{
	AlphaBlend(
		GetImageHDC(NULL), dstX, dstY, dstW, dstH,
		GetImageHDC(srcImg), srcX, srcY, srcW, srcH,
		{ AC_SRC_OVER, 0, 255, AC_SRC_ALPHA }
	);
}

// 主角自己的数据初始化
void initMajor()
{
	loadimage(&imgMajorRun, "res/Run.M.png");
	loadimage(&imgMajorJump, "res/Jump.M.png");
	loadimage(&imgMajorSlide, "res/Slide.M.png");


	int scale = major_size; 
	int origW = imgMajorRun.getwidth() / 4;
	int origH = imgMajorRun.getheight() / 2;

	major.width = origW * scale;
	major.height = origH * scale;

	major.x = 100;
	major.y = GroundY - major.height;
		
	major.state = RUN;
	major.frameIndex = 0;

	major.vy = 0;

	major.jumpCount = 0;

	major.dashMode = false;
	major.dashModeTimer = 0;

	major.superMode = false;   
	major.superModeTimer = 0;    
}

// 渲染主角动画
void drawMajor() 
{
	IMAGE* currentImg = NULL;

	if (major.state == RUN) 
	{
		currentImg = &imgMajorRun;
	}

	else if (major.state == JUMP) 
	{
		currentImg = &imgMajorJump;
	}

	else if (major.state == SLIDE)
	{
		currentImg = &imgMajorSlide;
	}

	double scale = major_size;


	int origW = currentImg->getwidth() / 4;
	int origH = currentImg->getheight() / 2;

	int dstW = (int)(origW * scale);
	int dstH = (int)(origH * scale);

	int srcX = (major.frameIndex % 4) * origW;
	int srcY = (major.frameIndex / 4) * origH;

	// 脚底板对齐
	int drawY = (major.y + major.height) - dstH;	

	drawAlphaCrop(major.x, drawY, dstW, dstH, currentImg, srcX, srcY, origW, origH);



	if (!isGameOver)
	{  
		static int loopCounter = 0;
		loopCounter++;
		if (loopCounter % 6 == 0)
		{
			if (major.state == RUN) 
			{
				major.frameIndex++;
				if (major.frameIndex >= 8)//循环播放
				{
					major.frameIndex = 0;
				}
			}

			else if (major.state == JUMP) 
			{
				if (major.frameIndex < 5) 
				{
					major.frameIndex++;
				}
			}

			else if (major.state == SLIDE)
			{
				if (major.frameIndex < 3) 
				{
					major.frameIndex++;
				}
			}
		}
	} 

	if (major.superMode)
	{
		major.superModeTimer--;
		if (major.superModeTimer <= 0) 
		{
			major.superMode = false; 
		}

	}

	if (major.dashMode)
	{
		major.dashModeTimer--;
		if (major.dashModeTimer <= 0)
		{
			major.dashMode = false;
		}

	}

}

void updateMajor()
{
	static bool spaceUp = true;
	bool spaceDown = (GetAsyncKeyState(VK_SPACE) & 0x8000);
	bool shiftDown = (GetAsyncKeyState(VK_SHIFT) & 0x8000);


	static int jumpBufferTimer = 0;
	if (spaceDown && spaceUp)
	{//跳跃缓冲		
		jumpBufferTimer = 8;
	}

	if (jumpBufferTimer > 0)
	{
		jumpBufferTimer--;
	}
	spaceUp = !spaceDown;


	if (major.state == JUMP)
	{
		if (!spaceDown && major.vy < -6) //短按跳
		{
			major.vy = -6;	
		}

		if (major.vy >= -3 && major.vy <= 3) //滞空
		{
			static int hangTimer = 0;
			hangTimer++;
			if (hangTimer % 2 == 0)
			{
				major.vy += 1;
			}

		}
		else
		{
			major.vy += 1;
		}

	}
	else
	{
		major.vy += 1;
	}

	major.y += major.vy;

	// 探坑与落地检测
	int floorY = GroundY - major.height;
	for (int i = 0; i < MAX_BARRIERS; i++)
	{
		if (barriers[i].isActive && barriers[i].type == barPit)
		{
			int majorCenterX = major.x + major.width / 2;
			if (majorCenterX > barriers[i].x + 30 && majorCenterX < barriers[i].x + barriers[i].width - 30)
			{
				floorY = 2000;//搭一个安全地面
				break;
			}

		}
	}

	if (major.y >= floorY) //落地切换run
	{
		major.y = floorY;
		major.vy = 0;
		major.jumpCount = 0;

		if (major.state == JUMP)
		{
			major.state = RUN;
			major.frameIndex = 0;
		}

	}

	// 下滑锁定
	bool underCeiling = false;
	for (int i = 0; i < MAX_BARRIERS; i++)
	{
		if (barriers[i].isActive && barriers[i].type == barCeiling)
		{

			int playerCenterX = major.x + major.width / 2;

			if (playerCenterX > barriers[i].x && major.x < barriers[i].x + barriers[i].width)
			{
				underCeiling = true;
				break;
			}

		}
	}


	if (jumpBufferTimer > 0 && major.jumpCount < 2 && !underCeiling)//起跳
	{
		major.vy = -18;
		major.state = JUMP;
		if (major.jumpCount == 0)
		{
			major.frameIndex = 0;

		}

		else
		{
			major.frameIndex = 2;
		}
		major.jumpCount++;

		jumpBufferTimer = 0;
	}

	if (major.y == floorY && major.jumpCount == 0 && major.state != JUMP)
	{
		if (shiftDown || underCeiling)
		{
			if (major.state != SLIDE)
			{
				major.state = SLIDE;
				major.frameIndex = 0;
			}

		}

		else
		{
			if (major.state != RUN)
			{
				major.state = RUN;
				major.frameIndex = 0;
			}

		}
	}
}