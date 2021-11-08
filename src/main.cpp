#include <Arduino.h>
#include "WiFi.h"
#include "HTTPClient.h"
#include "Arduino_JSON.h"
#include <U8g2lib.h>

#include "./config.h"

#define NYSSE_URL "http://api.digitransit.fi/routing/v1/routers/waltti/index/graphql"
#define PAYLOAD "{\"query\":\"{\\n  stop(id: \\\"tampere:0566\\\") {\\n    name\\n    id\\n    stoptimesWithoutPatterns(numberOfDepartures:10){\\n      realtimeArrival\\n      headsign\\n    }\\n  }\\n}\",\"variables\":null}"
#define BUS_LINE "Linnainmaa"

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);
int update_second_count = 0;
String arrivals = "Odota";

// https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/#3
void init_wifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PWD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println("Connected");
}

// https://randomnerdtutorials.com/esp32-http-get-post-arduino/#http-post
String query_data()
{
  WiFiClient client;
  HTTPClient http;

  http.begin(client, NYSSE_URL);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(PAYLOAD);

  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  String body = http.getString();
  Serial.println(body);
  if (httpResponseCode == 200)
  {
    return body;
  }
  else
    return "";
}

String parse_arrivals(String data)
{
  JSONVar json = JSON.parse(data);
  String arrivals_text = "";
  int count = 0;

  Serial.print("Arrivals: ");

  if (data.length() > 0)
  {
    JSONVar arrivals_list = json["data"]["stop"]["stoptimesWithoutPatterns"];
    for (int i = 0; i < arrivals_list.length() && count < 2; i++)
    {
      if (strcmp(arrivals_list[i]["headsign"], BUS_LINE) == 0)
      {
        int hours = (int)arrivals_list[i]["realtimeArrival"] / 60 / 60;
        hours = hours > 24 ? hours - 24 : hours;
        int minutes = (int)arrivals_list[i]["realtimeArrival"] / 60 % 60;
        arrivals_text += String(hours) + ":" + String(minutes) + " ";
        count++;
      }
    }
    arrivals_text = arrivals_text.length() == 0 ? "Ei vuoroa :|" : arrivals_text;
    Serial.println(arrivals_text);
    return arrivals_text;
  }
  else
  {
    Serial.println(arrivals);
    return "Ei vuoroja :|";
  }
}

void update_display(String data, float seconds_since_update, int update_interval)
{
  int text_length = data.length() * 9;
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_10x20_mf);
  u8g2.setFontDirection(2);
  u8g2.drawStr(text_length + (128 - text_length) / 2, 11, data.c_str());
  u8g2.drawLine(0, 1, 128 - seconds_since_update / update_interval * 128, 1);
  u8g2.sendBuffer();
}

void setup()
{
  Serial.begin(115200);
  u8g2.begin();

  Serial.println("Connecting to Wifi");
  init_wifi();

  Serial.println("Getting arrivals");
  String data = query_data();

  Serial.println("Parsing arrivals");
  arrivals = parse_arrivals(data);
}

void loop()
{
  if (update_second_count == 60)
  {
    update_second_count = 0;
    Serial.println("Getting arrivals");
    String data = query_data();

    Serial.println("Parsing arrivals");
    arrivals = parse_arrivals(data);
  }

  update_display(arrivals, update_second_count, 60);
  delay(1000);
  update_second_count++;
}
