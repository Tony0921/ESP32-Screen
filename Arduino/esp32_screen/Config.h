#ifndef CONFIG_H
#define CONFIG_H

#include <TFT_eSPI.h>
#include <FastLED.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <Arduino.h>
#include <SPI.h>

extern TFT_eSPI tft;
extern NTPClient timeClient;
extern CRGB leds[];
extern const char* ssid;
extern const char* password;
extern const char* apiKey;
extern const char* city;
extern WiFiClient client;
extern const char* host;

extern bool haveNewData;
extern bool needUpdateWeather;
extern bool needUpdateDisplay;


extern bool isHostConnect;

// 輪播控制
extern int currentDisplay;  // 當前顯示的數據索引（0-4）
extern unsigned long lastSwitchTime;
extern const unsigned long switchInterval;  // 切換間隔（3秒）
extern bool needSwitchDisplay;

#endif
