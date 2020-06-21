#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "config.h"

char server[] = "api.openweathermap.org";

String host = "api.openweathermap.org";
String lat = "34.8405";
String lon = "-82.289544";

// OneCallAPI - Forecast
String OneCallForecast = "onecall";
String exclude = "daily,minutely";
String getForecast = ("GET /data/2.5/" + OneCallForecast + "?lat=" + lat + "&lon=" + lon + "&exclude=" + exclude + "&units=imperial&appid=" + APIkey);

//OneCallAPI - Historical
String OneCallHistorical = "onecall/timemachine";
int currentDT = 1592639056;
String getHistorical = ("GET /data/2.5/" + OneCallHistorical + "?lat=" + lat + "&lon=" + lon + "&units=imperial&dt=" + currentDT + "&appid=" + APIkey);

// Weather Parsing
int historicalHours = 10;
int forecastHours = 10;
float rainPast[10];
float rainPastSum = 0;
float rainLater[10];
float rainLaterSum = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println();

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
}

void loop()
{

  WiFiClient client;
  // if there's a successful connection:
  
  //Get Forecast Weather Data
  if (client.connect(host, 80))
  {
    Serial.println("Connecting to OpenWeatherMap server...");
    // send the HTTP PUT request:
    client.println(getForecast);
    client.println("Host: " + host);
    client.println("Connection: close");
    client.println();
    Serial.println("Getting Forecast...");

    // Parse response
    const size_t capacity = 49 * JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(48) + 9 * JSON_OBJECT_SIZE(1) + 49 * JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(6) + 40 * JSON_OBJECT_SIZE(10) + 8 * JSON_OBJECT_SIZE(11) + JSON_OBJECT_SIZE(15) + 7140;
    DynamicJsonDocument forecast(capacity);
    DeserializationError error = deserializeJson(forecast, client);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }

    //Past rain
    int i;
    String currentDT = forecast["current"]["dt"];
    String currentWeather = forecast["current"]["weather"][0]["main"];
    rainLaterSum = 0;
    for (i = 0; i > forecastHours; i++)                                       //forecasted rain in inches for next [forecastHours]
    {
      rainLater[i] = forecast["hourly"][i]["rain"]["1h"];
    }
    for (i = 0; i > forecastHours; i++)
    {
      rainLaterSum = rainLaterSum + rainLater[i];
    }

    //print values
    Serial.println(currentDT);
    Serial.println(currentWeather);
    Serial.println(rainLaterSum);

    //client.stop();
    Serial.println("Disconnecting");
    Serial.println("");
  }
  else
  {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
  delay(2000);

  //Get Historical Weather Data
  if (client.connect(host, 80))
  {
    Serial.println("Connecting to OpenWeatherMap server...");
    // send the HTTP PUT request:
    client.println(getHistorical);
    client.println("Host: " + host);
    client.println("Connection: close");
    client.println();
    Serial.println("Getting Historical...");

    // Parse response
    const size_t capacity = 25 * JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(24) + JSON_OBJECT_SIZE(1) + 25 * JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(6) + 23 * JSON_OBJECT_SIZE(11) + JSON_OBJECT_SIZE(12) + JSON_OBJECT_SIZE(14) + 6960;
    DynamicJsonDocument historical(capacity);
    DeserializationError error = deserializeJson(historical, client);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }

    //Past rain
    int i;
    String histLat = historical["lat"];
    String historicalDT = historical["current"]["dt"];
    String historicalWeather = historical["current"]["weather"][0]["main"];
    for (i = 0; i > historicalHours; i++)                                       //rain in inches for past [historicalHours]
    {
      rainPast[i] = historical["hourly"][i]["rain"];
    }
    for (i = 0; i > historicalHours; i++)
    {
      rainPastSum = rainPastSum + rainPast[i];
    }

    //print values
    Serial.println(historicalDT);
    Serial.println(historicalWeather);
    Serial.println(rainPastSum);

    client.stop();
    Serial.println("Disconnecting");
    Serial.println("");
  }
  else
  {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
  delay(20000);
}