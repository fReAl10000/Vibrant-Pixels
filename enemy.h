#pragma once
#include <graphics.h>

typedef struct {
	int x;
	int y;
	int width;
	int height;
	bool isActive;
	int frameIndex;
} Enemy;

#define MAX_ENEMIES 5
extern Enemy enemies[MAX_ENEMIES];

void initEnemy();
void updateEnemy();
void drawEnemy();	