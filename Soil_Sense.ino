#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const int soilPin = A0;  // Analog pin for soil sensor
#define RELAY_PIN 14     // GPIO14 = D5 on NodeMCU
#define SWITCH 12     // GPIO12 = D6 on NodeMCU

LiquidCrystal_I2C lcd(0x27,16,2);

const char* ssid = "User";
const char* password = "1234";

WiFiClient client;

unsigned long myChannelNumber = *******; //channel ID
const char * myWriteAPIKey = "******************"; //write api key
float nextHourRain = 0.0;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Starting...");
  
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  pinMode(SWITCH, INPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  ThingSpeak.begin(client);
}

void loop() {
  int soilValue = analogRead(soilPin);
  int moisturePercent = map(soilValue, 1023, 300, 0, 100);

  Serial.print("Soil Value: ");
  Serial.print(soilValue);
  Serial.print(" | Moisture: ");
  Serial.print(moisturePercent);
  Serial.println("%");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Moist: ");
  lcd.print(moisturePercent);
  lcd.print("%");

  // Relay logic

    if (nextHourRain<10){
      if (moisturePercent < 50) {
        digitalWrite(RELAY_PIN, HIGH); // Water
      } else {
        digitalWrite(RELAY_PIN, LOW);  // Stop
      }
    }
  
  // Upload to ThingSpeak
  ThingSpeak.writeField(myChannelNumber, 1, moisturePercent, myWriteAPIKey);

  // Call weather API
  getWeatherForecast();

  delay(5000); // Run every 60 seconds
}

// 🌦️ WEATHER FUNCTION
void getWeatherForecast() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    // Replace lat/lon with your location (e.g., Delhi)
    String url = "http://api.open-meteo.com/v1/forecast?latitude=26.9302&longitude=75.9250&hourly=precipitation,precipitation_probability&forecast_days=1&timezone=auto";
    http.begin(client, url);

    int httpCode = http.GET();
    if (httpCode == 200) {
      String payload = http.getString();
      DynamicJsonDocument doc(4096);
      deserializeJson(doc, payload);

    nextHourRain = doc["hourly"]["precipitation"][1]; // Rain in mm
    int rainProbability = doc["hourly"]["precipitation_probability"][1];

      Serial.print("Next Hour rain: ");
      Serial.print(nextHourRain);
      Serial.println(" mm");

      Serial.print("Rain Probability: ");
      Serial.print(rainProbability);
      Serial.println(" %");


      lcd.setCursor(0, 1);

      if (rainProbability > 50) {
        lcd.print("Rain Likely!");
      } else if (nextHourRain > 0.0) {
        lcd.print("Rain: ");
        lcd.print(nextHourRain);
        lcd.print("mm");
      } else {
        lcd.print("No Rain");
      }

    } else {
      Serial.println("Failed to fetch weather!");
      lcd.setCursor(0, 1);
      lcd.print("Weather Error");
    }
    http.end();
  }
}
