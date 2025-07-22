#include "Clock.h"

// 月份名稱陣列
const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

void updateClockScreen(){
  // timeClient.update();

  int hours = timeClient.getHours() % 12; // 12小時制
  int minutes = timeClient.getMinutes();
  int seconds = timeClient.getSeconds();

  // 使用 NTP 時間戳計算年、月、日
  time_t rawTime = timeClient.getEpochTime();
  struct tm* timeInfo = localtime(&rawTime);
  int year = timeInfo->tm_year + 1900; // 年份從 1900 開始
  int month = timeInfo->tm_mon;        // 0-11
  int day = timeInfo->tm_mday;         // 1-31

  // 渲染整個時鐘畫面
  renderClock(hours, minutes, seconds, year, month, day);
}

// 渲染整個時鐘畫面，包括年月日
void renderClock(int hours, int minutes, int seconds, int year, int month, int day) {
  // 重新繪製錶盤
  drawClockFace();

  // 在圓心上半部顯示年份，下半部顯示月份和日期（在指針後面）
  tft.setTextColor(rgb24to16("E4C9A0"), TFT_BLACK);
  tft.setFreeFont(&FreeMonoBold12pt7b);
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM); // 置中對齊

  // 年份（上半部）
  tft.drawString(String(year), CENTER_X, CENTER_Y - 20);

  // 月份和日期（下半部）
   tft.setTextColor(rgb24to16("F9B48E"), TFT_BLACK);
  String dateStr = String(months[month]) + " " + String(day);
  tft.drawString(dateStr, CENTER_X, CENTER_Y + 20);

  // 繪製所有指針（在文字前面）
  drawHand(hours * 5 + minutes / 12, HOUR_LEN, rgb24to16("FFD97D"), 5); // 時針
  drawHand(minutes, MINUTE_LEN, rgb24to16("F7EFD2"), 3);              // 分針
  drawHand(seconds, SECOND_LEN, rgb24to16("FF6F00"), 1);                // 秒針
  tft.fillCircle(CENTER_X, CENTER_Y, 5, rgb24to16("B26430"));
}

// 繪製時鐘錶盤（無外框，半徑120，含數字）
void drawClockFace() {
  tft.fillScreen(TFT_BLACK);

  // 繪製12個小時刻度和數字
  for (int i = 0; i < 12; i++) {
    float angle = i * 30 * PI / 180; // 每30度一個刻度
    // 刻度線
    int x1 = CENTER_X + 110 * sin(angle);
    int y1 = CENTER_Y - 110 * cos(angle);
    int x2 = CENTER_X + 120 * sin(angle);
    int y2 = CENTER_Y - 120 * cos(angle);
    tft.drawLine(x1, y1, x2, y2, TFT_WHITE);

    // 數字位置（稍微內縮）
    int numX = CENTER_X + 95 * sin(angle);
    int numY = CENTER_Y - 95 * cos(angle);
    tft.setTextColor(rgb24to16("FCEED4"), TFT_BLACK);
    tft.setFreeFont(&FreeMonoBold9pt7b);
    tft.setTextSize(1);
    tft.setTextDatum(MC_DATUM); // 置中對齊
    // tft.setFreeFont(&Picopixel);
    
    tft.drawString(String(i == 0 ? 12 : i), numX, numY);
  }
}

// 繪製單個指針
void drawHand(int value, int length, uint16_t color, int thickness) {
  float angle = value * 6 * PI / 180 - PI / 2; // 每刻度6度，偏移90度
  int x = CENTER_X + length * cos(angle);
  int y = CENTER_Y + length * sin(angle);
  tft.drawWideLine(CENTER_X, CENTER_Y, x, y, thickness, color);
}