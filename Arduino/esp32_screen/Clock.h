#ifndef CLOCK_H
#define CLOCK_H

#include "Display.h"
#include "Config.h"

// 時鐘中心和指針長度
#define CENTER_X 120
#define CENTER_Y 120
#define HOUR_LEN 70
#define MINUTE_LEN 90
#define SECOND_LEN 110

void updateClockScreen();
void renderClock(int hours, int minutes, int seconds, int year, int month, int day);
void drawClockFace();
void drawHand(int value, int length, uint16_t color, int thickness);

#endif
