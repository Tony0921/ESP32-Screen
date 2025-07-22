#include "Display.h"

void drawIcon(String filePath, int x, int y) {
  fs::File file = LittleFS.open(filePath, "r");
  if (file) {
    drawBmp(filePath, x, y);
    file.close();
  } else {
    Serial.println("無法打開圖標檔案: " + filePath);
  }
}

void drawBmp(String filename, int16_t x, int16_t y) {
  if ((x >= tft.width()) || (y >= tft.height())) return;

  fs::File bmpFile = LittleFS.open(filename, "r");
  if (!bmpFile) {
    Serial.print("File not found: ");
    Serial.println(filename);
    return;
  }

  uint8_t header[54];
  bmpFile.read(header, 54);

  if (header[0] != 'B' || header[1] != 'M') {
    Serial.println("Not a valid BMP file");
    bmpFile.close();
    return;
  }

  uint32_t bmpWidth  = *(int32_t*)&header[18];
  uint32_t bmpHeight = *(int32_t*)&header[22];
  uint16_t bitDepth  = *(int16_t*)&header[28];

  if (bitDepth != 24) {
    Serial.println("Only 24-bit BMP files are supported");
    bmpFile.close();
    return;
  }

  uint32_t rowSize = (bmpWidth * 3 + 3) & ~3;
  bool flip = bmpHeight > 0;
  bmpHeight = abs((int32_t)bmpHeight); // 修正這裡

  tft.startWrite();
  tft.setAddrWindow(x, y, bmpWidth, bmpHeight);

  uint8_t sdbuffer[3 * 20];
  uint8_t r, g, b;

  for (uint32_t row = 0; row < bmpHeight; row++) {
    uint32_t pos = 54 + (flip ? (bmpHeight - 1 - row) * rowSize : row * rowSize);
    bmpFile.seek(pos);

    for (uint16_t col = 0; col < bmpWidth; col++) {
      bmpFile.read(sdbuffer, 3);
      b = sdbuffer[0];
      g = sdbuffer[1];
      r = sdbuffer[2];

      uint16_t color = tft.color565(r, g, b);
      tft.writeColor(color, 1);
    }
  }
  tft.endWrite();
  bmpFile.close();
  // Serial.println("BMP Image Displayed Successfully!");
}

uint16_t rgb24to16(const char* hexColor) {
  // 檢查輸入是否有效
  if (!hexColor) return 0; // 空指針返回黑色

  // 檢查字串長度和格式
  int offset = 0;
  if (hexColor[0] == '#') offset = 1; // 跳過 # 符號
  int len = strlen(hexColor);
  if (len != (6 + offset)) return 0; // 長度不正確返回黑色

  // 提取 R、G、B 值
  char r_str[3] = {hexColor[offset], hexColor[offset + 1], '\0'};
  char g_str[3] = {hexColor[offset + 2], hexColor[offset + 3], '\0'};
  char b_str[3] = {hexColor[offset + 4], hexColor[offset + 5], '\0'};

  // 將 16 進位字串轉為整數 (0-255)
  uint8_t r = strtol(r_str, NULL, 16);
  uint8_t g = strtol(g_str, NULL, 16);
  uint8_t b = strtol(b_str, NULL, 16);

  // 將 8 位 RGB 值縮放到 RGB565 格式
  uint8_t r_5bit = (r * 31) / 255; // 縮放到 5 位 (0-31)
  uint8_t g_6bit = (g * 63) / 255; // 縮放到 6 位 (0-63)
  uint8_t b_5bit = (b * 31) / 255; // 縮放到 5 位 (0-31)

  // 組合為 16 位色號
  return (r_5bit << 11) | (g_6bit << 5) | b_5bit;
}