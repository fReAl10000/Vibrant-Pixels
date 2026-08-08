#pragma once
//窗口大小
#define win_width 980
#define win_height 640
#define major_size 2.5
//水平基线
#define GroundY 500

#define safeDist 280


// 环境与总初始化函数声明
void initGame();  
void updateBg();  
void flyBg();     

void drawItemUI();

extern bool isGameOver; 