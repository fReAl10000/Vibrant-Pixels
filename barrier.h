#pragma once
#include <graphics.h>

enum BarrierType { barGround, barCeiling, barPit };

typedef struct{
	int x;
	int y;
	int width;
	int height;
	bool isActive; // 是否在屏幕内且有效
	BarrierType type; 
} Barrier;

#define MAX_BARRIERS 5 
	
extern Barrier barriers[MAX_BARRIERS];

void initBarrier();
void updateBarrier();
void drawBarrier();