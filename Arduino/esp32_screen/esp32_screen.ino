#include "Config.h"
#include "Display.h"
#include "Weather.h"
#include "Clock.h"
#include "Monitor.h"

#define BTN_A 7
#define BTN_B 6
#define BTN_C 5

#define NUM_LEDS 1
#define DATA_PIN 48
#define LED_TYPE WS2812
#define BRIGHTNESS 64
#define COLOR_ORDER GRB

// TFT 物件
TFT_eSPI tft = TFT_eSPI();

// WiFi 憑證
const char* ssid = "";
const char* password = "";
const char *host = "";

// NTP 設置
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 28800, 60000); // UTC+8 (台灣時區)，每60秒更新

// OpenWeatherMap API 設定
const char* apiKey = "";
const char* city = "Taipei,TW";

CRGB leds[NUM_LEDS];

bool haveNewData = false;
bool needUpdateWeather = false;
bool needUpdateDisplay = false;

WiFiClient client;
bool isHostConnect = false;

// 輪播控制
int currentDisplay = 0;  // 當前顯示的數據索引（0-4）
unsigned long lastSwitchTime = 0;
const unsigned long switchInterval = 3000;  // 切換間隔（3秒）
bool needSwitchDisplay = false;

// 計時相關變數
unsigned long previousMillis = 0; // 上次執行時間
const long interval = 1000;      // 間隔時間（毫秒，1秒）
int counter = 0;                 // 計數器

int show_status = 0;

volatile unsigned long lastInterruptTimeA = 0; // 上次中斷時間
volatile unsigned long lastInterruptTimeB = 0; // 上次中斷時間
volatile unsigned long lastInterruptTimeC = 0; // 上次中斷時間
const unsigned long debounceDelay = 500; // 防彈跳延遲 (毫秒)

void setup() {
  // 初始化序列埠
  Serial.begin(115200);

  pinMode(BTN_A, INPUT_PULLDOWN);
  pinMode(BTN_B, INPUT_PULLDOWN);
  pinMode(BTN_C, INPUT_PULLDOWN);

  // 初始化 LittleFS
  if (!LittleFS.begin()) {
    Serial.println("LittleFS 初始化失敗");
    return;
  }
  Serial.println("LittleFS 初始化成功");

  // 初始化 TFT
  tft.begin();
  tft.setRotation(2); // 調整顯示方向
  tft.fillScreen(TFT_BLACK);

  initScreen();


  // 連接到 WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi 已連接");
  Serial.println("");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  // 初始化 NTP
  timeClient.begin();
  
  attachInterrupt(digitalPinToInterrupt(BTN_A), btnHandlerA, RISING);
  attachInterrupt(digitalPinToInterrupt(BTN_B), btnHandlerB, RISING);
  attachInterrupt(digitalPinToInterrupt(BTN_C), btnHandlerC, RISING);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  getWeather();

  connectHost();

  tft.fillScreen(TFT_BLACK); //clear screen

}

void loop() {

  // 獲取當前時間
  unsigned long currentMillis = millis();

  // 檢查是否達到 1 秒間隔
  if (currentMillis - previousMillis >= interval) {
    timeClient.update();
    if(show_status==0){
      leds[0] = CRGB::Green;
      updateClockScreen();
    }else if(show_status==1){
      leds[0] = CRGB::Red;
      updateWeatherScreen();
    }else if(show_status==2){
      leds[0] = CRGB::Blue;
      updateMonitorScreen();
      
    }
    FastLED.show();

    // 更新上次執行時間
    previousMillis = currentMillis;
  }
}

void initScreen(){
  // 顯示標題
  tft.fillScreen(TFT_BLACK); //clear screen
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setFreeFont(&FreeSansBold12pt7b);  // 使用免費字型
  tft.setTextColor(rgb24to16("FFFFFF"), TFT_BLACK);
  tft.drawString("ESP32 Screen", 120, 120);
  delay(3000);
  
  // 顯示連接WiFi
  tft.fillScreen(TFT_BLACK); //clear screen
  drawIcon("/wifi.bmp", 120-25, 80-25);
  tft.setTextDatum(MC_DATUM);
  tft.setFreeFont(&FreeMonoBold12pt7b);  // 使用免費字型
  tft.setTextColor(rgb24to16("615AFF"), TFT_BLACK);
  tft.drawString("Wi-Fi", 120, 120);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Connecting...", 120, 150);
}

void btnHandlerA(){
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTimeA > debounceDelay) {
    Serial.println("btn a");
    show_status++;
    if(show_status > 2){
      show_status=0;
    }
    if(show_status==1){
      needUpdateDisplay = true;
    }
    lastInterruptTimeA = currentTime; // 更新上次中斷時間
  }
}
void btnHandlerB(){
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTimeB > debounceDelay) {
    Serial.println("btn b");
    if(show_status==1){
      needUpdateWeather = true;
    }
    if(show_status==2){
      needSwitchDisplay = true;
    }
    lastInterruptTimeB = currentTime; // 更新上次中斷時間
  }
}
void btnHandlerC(){
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTimeC > debounceDelay) {
    Serial.println("btn c");
    show_status--;
    if(show_status < 0){
      show_status=2;
    }
    if(show_status==1){
      needUpdateDisplay = true;
    }
    lastInterruptTimeC = currentTime; // 更新上次中斷時間
  }
}


