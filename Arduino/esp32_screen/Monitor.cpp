#include "Monitor.h"

int16_t cpu_temp;
int16_t cpu_usage;
int16_t ram_usage;
int16_t gpu_usage;
int16_t gpu_temp;
// 每個數據的最小值和最大值
const float cpuTempMin = 24;   // CPU 溫度最小值（°C）
const float cpuTempMax = 100;  // CPU 溫度最大值（°C）
const float cpuUsageMin = 0;   // CPU 使用率最小值（%）
const float cpuUsageMax = 100; // CPU 使用率最大值（%）
const float ramUsageMin = 0;   // RAM 使用率最小值（%）
const float ramUsageMax = 100; // RAM 使用率最大值（%）
const float gpuTempMin = 24;   // GPU 溫度最小值（°C）
const float gpuTempMax = 100;  // GPU 溫度最大值（°C）
const float gpuUsageMin = 0;   // GPU 使用率最小值（%）
const float gpuUsageMax = 100; // GPU 使用率最大值（%）

void connectHost(){
  if (client.connect(host, 80))
  {
    isHostConnect = true;
    Serial.println("host Conected!");
    return;
  }
  isHostConnect = false;
  Serial.println("Connect host failed!");
  // client.stop();
}

void showHostDisconnect(){
  // 清除螢幕
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(1);
  tft.setFreeFont(&FreeSansBold12pt7b);  // 使用免費字型
  tft.drawString("Host Disconnect!", 120, 120);
}

void updateMonitorScreen(){
  unsigned long currentTime = millis();
  if(isHostConnect){
    moniterGetState();
    if (currentTime - lastSwitchTime >= switchInterval) {
      needSwitchDisplay = true;
    }
    if(needSwitchDisplay){
      switchMonitorDisplay();
    }
  }
  else{
    showHostDisconnect();
    connectHost();
  }
  
}

void switchMonitorDisplay(){
  currentDisplay = (currentDisplay + 1) % 5;  // 切換到下一個數據（0-4）
  lastSwitchTime = millis();

  // 根據當前顯示的數據繪製進度環
  switch (currentDisplay) {
    case 0:  // CPU 溫度
      drawProgressRing(120, 120, 70, cpu_temp, cpuTempMin, cpuTempMax, "CPU Temp", "C", "/cpu.bmp");
      break;
    case 1:  // CPU 使用率
      drawProgressRing(120, 120, 70, cpu_usage, cpuUsageMin, cpuUsageMax, "CPU Usage", "%", "/cpu.bmp");
      break;
    case 2:  // RAM 使用率
      drawProgressRing(120, 120, 70, ram_usage, ramUsageMin, ramUsageMax, "RAM Usage", "%", "/ram.bmp");
      break;
    case 3:  // GPU 溫度
      drawProgressRing(120, 120, 70, gpu_temp, gpuTempMin, gpuTempMax, "GPU Temp", "C", "/gpu.bmp");
      break;
    case 4:  // GPU 使用率
      drawProgressRing(120, 120, 70, gpu_usage, gpuUsageMin, gpuUsageMax, "GPU Usage", "%", "/gpu.bmp");
      break;
  }
  needSwitchDisplay = false;
}

void moniterGetState(){
  String getUrl = "/sse";
  client.print(String("GET ") + getUrl + " HTTP/1.1\r\n" + "Content-Type=application/json;charset=utf-8\r\n" + "Host: " + host + "\r\n" + "User-Agent=ESP32\r\n" + "Connection: close\r\n\r\n");
  // Serial.println("Get send");

  delay(10);

  char endOfHeaders[] = "\n\n";
  bool ok = client.find(endOfHeaders);
  if (!ok)
  {
    Serial.println("No response or invalid response!");
    isHostConnect = false;
    client.flush();
    client.stop();
  }
  // Serial.println("Skip headers");

  String line = "";

  line += client.readStringUntil('\n');

  // Serial.println("Content:");
  // Serial.println(line);

  int16_t dataStart = 0;
  int16_t dataEnd = 0;
  String dataStr;

  char cpuTemp[] = "CPU Temp";
  dataStart = line.indexOf(cpuTemp) + strlen(cpuTemp);
  dataEnd = line.indexOf("C", dataStart);
  dataStr = line.substring(dataStart, dataEnd);
  cpu_temp = dataStr.toInt();

  char cpuUsage[] = "CPU Usage";
  dataStart = line.indexOf(cpuUsage) + strlen(cpuUsage);
  dataEnd = line.indexOf("%", dataStart);
  dataStr = line.substring(dataStart, dataEnd);
  cpu_usage = dataStr.toInt();

  char ramUsage[] = "RAM Usage";
  dataStart = line.indexOf(ramUsage) + strlen(ramUsage);
  dataEnd = line.indexOf("%", dataStart);
  dataStr = line.substring(dataStart, dataEnd);
  ram_usage = dataStr.toInt();

  char gpuTemp[] = "GPU Temp";
  dataStart = line.indexOf(gpuTemp) + strlen(gpuTemp);
  dataEnd = line.indexOf("C", dataStart);
  dataStr = line.substring(dataStart, dataEnd);
  gpu_temp = dataStr.toInt();

  char gpuUsage[] = "GPU Usage";
  dataStart = line.indexOf(gpuUsage) + strlen(gpuUsage);
  dataEnd = line.indexOf("%", dataStart);
  dataStr = line.substring(dataStart, dataEnd);
  gpu_usage = dataStr.toInt();

  // Serial.print("CPU temp :");
  // Serial.println(cpu_temp);
  // Serial.print("CPU usage :");
  // Serial.println(cpu_usage);
  // Serial.print("RAM usage :");
  // Serial.println(ram_usage);
  // Serial.print("GPU temp :");
  // Serial.println(gpu_temp);
  // Serial.print("GPU usage :");
  // Serial.println(gpu_usage);
}

void drawProgressRing(int x, int y, int radius, float value, float minValue, float maxValue, String label, String valueUnit, String iconPath) {
  // 清除螢幕
  tft.fillScreen(TFT_BLACK);

  // 顯示數據名稱
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setFreeFont(&FreeSans9pt7b);
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);  // 文字居中
  tft.drawString(label, 120, 210);

  // 將數據值映射到 0-100 的百分比
  int percentage = map(value, minValue, maxValue, 0, 100);
  percentage = constrain(percentage, 0, 100);  // 限制百分比在 0-100 之間

  // 根據百分比選擇顏色
  uint16_t color;
  if (percentage <= 50) {
    color = TFT_BLUE;  // 小於等於 50：綠色
  } else if (percentage <= 90) {
    color = TFT_YELLOW;  // 51~90：黃色
  } else {
    color = TFT_RED;  // 大於 90：紅色
  }

  // 定義線條粗細（例如 3 像素）
  int thickness = 15;

  // 繪製背景環（灰色）
  int xC = 120; // Center x-coordinate of the arc
  int yC = 120; // Center y-coordinate of the arc
  int rad = 70; // Radius of the arc
  int irad = 85;
  int startAngle = 30; // Starting angle of the arc (in degrees)
  int endAngle = 330; // Ending angle of the arc (in degrees)
  uint32_t fg_color = TFT_DARKGREY; // Color of the arc
  uint32_t bg_color = TFT_BLACK;
  tft.drawArc(x, y, rad, irad, startAngle, endAngle, TFT_DARKGREY, TFT_BLACK); // Draw the arc

  // 繪製進度環
  int angle = map(percentage, 0, 100, 0, 300);  // 假設數據範圍為 0-100
  tft.drawArc(x, y, rad, irad, startAngle, startAngle + angle, color, TFT_BLACK); // Draw the arc

  drawIcon(iconPath, 120-25, 120-25);

  // 顯示數值並手動繪製 °C（僅對溫度數據）
  int value_y = 160;
  tft.setFreeFont(&FreeSans12pt7b);
  if (valueUnit == "C") {
    tft.drawString(String((int)value), 105, value_y);  // 顯示數值
    tft.drawCircle(125, value_y-5, 3, TFT_WHITE);  // 手動繪製 °
    tft.drawString("C", 140, value_y);  // 顯示 C
  } else {
    tft.drawString(String((int)value)+valueUnit, 120, value_y);  // 其他數據直接顯示
  }
}