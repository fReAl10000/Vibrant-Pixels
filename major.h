#pragma once
#include <graphics.h>

// 状态
enum majorState { RUN, JUMP, SLIDE };

typedef struct {
	int x;
	int y;
	int width;
	int height;

	majorState state;

	int frameIndex;
	int vy;//重力
	int jumpCount;

	bool superMode;    
	int superModeTimer;  
	
	bool dashMode;
	bool dashRecover;
	int dashModeTimer;
	
	} Major;


extern Major major;

void initMajor();
void drawMajor();
void updateMajor();
void drawAlphaCrop(int dstX, int dstY, int dstW, int dstH, IMAGE* srcImg, int srcX, int srcY, int srcW, int srcH);