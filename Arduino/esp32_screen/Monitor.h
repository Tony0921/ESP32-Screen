#ifndef MONITOR_H
#define MONITOR_H

#include "Display.h"
#include "Config.h"

void connectHost();
void showHostDisconnect();
void updateMonitorScreen();
void switchMonitorDisplay();
void moniterGetState();
void drawProgressRing(int x, int y, int radius, float value, float minValue, float maxValue, String label, String valueUnit, String iconPath);

#endif