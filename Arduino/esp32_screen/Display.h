#ifndef DISPLAY_H
#define DISPLAY_H

#include "Config.h"

void drawIcon(String filePath, int x, int y);
void drawBmp(String filename, int16_t x, int16_t y);
uint16_t rgb24to16(const char* hexColor);


#endif
