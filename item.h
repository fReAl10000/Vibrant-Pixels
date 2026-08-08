#pragma once
#include <graphics.h>

typedef struct {
	int x;
	int y;
	int width;
	int height;
	bool isActive;
} Item;

extern Item attackItem;//红色
extern Item dashItem;//蓝色





void initItem();
void updateItem();
void drawItem();