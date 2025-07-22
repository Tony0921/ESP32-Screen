#ifndef WEATHER_H
#define WEATHER_H

#include "Display.h"
#include "Config.h"

void getWeather();
void updateWeatherScreen();
void drawWeatherIcon(String iconCode, int x, int y);
void drawWindDirection(float windDeg, int x, int y);
void drawTime(int x, int y);
void drawWeatherDisplay(String city_name, float temp, int humidity, String weather, String iconCode, float windSpeed, float windDeg, float rain);

#endif
