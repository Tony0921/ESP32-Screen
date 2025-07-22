#include "Weather.h"

struct Weather {
  String city_name;
  float temp;
  int humidity;
  String weather;
  String iconCode;
  float windSpeed; // 風速 (m/s)
  float windDeg;     // 風向 (度)
  float rain;
};
struct Weather w;

void updateWeatherScreen(){
  // 每十分更新
  if(timeClient.getMinutes() % 10 == 0 && timeClient.getSeconds() == 0 || needUpdateWeather){
    getWeather();
    needUpdateWeather = false;
  }
  if(haveNewData || needUpdateDisplay){
    drawWeatherDisplay(w.city_name, w.temp, w.humidity, w.weather, w.iconCode, w.windSpeed, w.windDeg, w.rain);
    haveNewData = false;
    needUpdateDisplay = false;
  }
  // 每秒更新時間
  drawTime(120, 50);
}

void getWeather() {
  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/weather?q=" + String(city) + "&appid=" + String(apiKey) + "&units=metric";
  http.begin(url);
  
  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);

    w.city_name = doc["name"].as<String>();
    w.temp = doc["main"]["temp"];
    w.humidity = doc["main"]["humidity"];
    w.weather = doc["weather"][0]["description"].as<String>();
    w.iconCode = doc["weather"][0]["icon"].as<String>();
    w.windSpeed = doc["wind"]["speed"]; // 風速 (m/s)
    w.windDeg = doc["wind"]["deg"];     // 風向 (度)
    w.rain = doc["rain"]["1h"];

    Serial.println("HTTP Get: " + url);
    haveNewData = true;
  } else {
    Serial.println("HTTP 請求失敗");
  }
  http.end();
}

void drawWeatherDisplay(String city_name, float temp, int humidity, String weather, String iconCode, float windSpeed, float windDeg, float rain) {
  tft.fillScreen(TFT_BLACK);

  // 城市名稱
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.setTextFont(1);
  tft.drawString(city_name, 120, 20);

  // 時間
  // drawTime(120, 50);

  // 溫度圖標
  drawIcon("/temparature.bmp", 25, 60);

  // 溫度
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setTextSize(3);
  tft.setTextDatum(MR_DATUM);
  tft.drawString(String(temp, 1), 155, 85);
  tft.drawCircle(165, 75, 3, TFT_RED);  // 手動繪製 °
  tft.drawString("C", 190, 85);

  // 天氣圖標
  drawWeatherIcon(iconCode, 160, 100);

  // 降雨量
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextDatum(MR_DATUM);
  if(rain == 0){
    tft.drawString("-- mm/hr", 210, 160);
  }else{
    tft.drawString(String(rain) + " mm/hr", 210, 160);
  }

  // 濕度圖標
  drawIcon("/humidity.bmp", 20, 110);

  // 濕度
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextDatum(MR_DATUM);
  tft.drawString(String(humidity)+" %", 135, 135);

  // 天氣描述
  // tft.setTextColor(TFT_WHITE, TFT_BLACK);
  // tft.setTextSize(2);
  // tft.setCursor(20, 160);
  // tft.println(weather);

  // 風向
  drawWindDirection(windDeg, 50, 170); // 風向圖標
  
  // 風速
  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextDatum(MR_DATUM);
  tft.drawString(String(windSpeed) + " m/s", 215, 180);

  // 風向角
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextDatum(MR_DATUM);
  tft.drawString(String((int)windDeg), 170, 210);
  tft.drawCircle(175, 202, 2, TFT_WHITE);  // 手動繪製 °
}

void drawWeatherIcon(String iconCode, int x, int y){
  String iconFile;
  if (iconCode == "01d") {
    iconFile = "/clear-day.bmp";
  } else if (iconCode == "01n") {
    iconFile = "/clear-night.bmp";
  } else if (iconCode == "02d") {
    iconFile = "/partly-cloudy-day.bmp";
  } else if (iconCode == "02n") {
    iconFile = "/partly-cloudy-night.bmp";
  } else if (iconCode == "03d" || iconCode == "03n" || iconCode == "04d" || iconCode == "04n") {
    iconFile = "/cloudy.bmp";
  } else if (iconCode == "09d" || iconCode == "09n") {
    iconFile = "/drizzle.bmp";
  } else if (iconCode == "10d" || iconCode == "10n") {
    iconFile = "/rain.bmp";
  } else if (iconCode == "11d" || iconCode == "11n") {
    iconFile = "/thunderstorm.bmp";
  } else if (iconCode == "13d" || iconCode == "13n") {
    iconFile = "/snow.bmp";
  } else if (iconCode == "50d" || iconCode == "50n") {
    iconFile = "/fog.bmp";
  } else {
    iconFile = "/unknown.bmp";
  }

  String path = "/icon50" + iconFile;
  drawIcon(path, x, y);
}

void drawWindDirection(float windDeg, int x, int y) {
    // 顯示風向文字（例如 N, NE, E 等）
  String direction;
  if (windDeg >= 337.5 || windDeg < 22.5) direction = "N";
  else if (windDeg >= 22.5 && windDeg < 67.5) direction = "NE";
  else if (windDeg >= 67.5 && windDeg < 112.5) direction = "E";
  else if (windDeg >= 112.5 && windDeg < 157.5) direction = "SE";
  else if (windDeg >= 157.5 && windDeg < 202.5) direction = "S";
  else if (windDeg >= 202.5 && windDeg < 247.5) direction = "SW";
  else if (windDeg >= 247.5 && windDeg < 292.5) direction = "W";
  else if (windDeg >= 292.5 && windDeg < 337.5) direction = "NW";

  // 繪製風向箭頭
  String path = "/wind/" + direction + ".bmp";
  drawIcon(path, x, y);
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(direction, x + 27, y - 10);
}

void drawTime(int x, int y){
  // timeClient.update();
  String formattedTime = timeClient.getFormattedTime();
  tft.fillRect(x-60, y, 120, 20, TFT_BLACK); // 清除舊時間
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.setTextFont(1);
  tft.drawString(formattedTime, x, y);
}