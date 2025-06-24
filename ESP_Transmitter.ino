#include <ESP8266WiFi.h>
#include <espnow.h>

// Replace with receiver's MAC Address
uint8_t receiverAddress[] = {0xC8, 0xC9, 0xA3, 0x56, 0x6D, 0xF1};  // Replace with actual MAC   C8:C9:A3:56:6D:F1

const int soilMoisturePin = A0;  // Analog pin connected to soil moisture sensor
const int moistureThreshold = 600;  // Threshold for dry soil, adjust based on testing

void setup() {
  Serial.begin(115200);
  
  // Initialize WiFi and ESP-NOW
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW Initialization Failed");
    return;
  }
  
  // Register the peer
  if (esp_now_add_peer(receiverAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0) != 0) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // Read soil moisture level
  int moistureLevel = analogRead(soilMoisturePin);
  Serial.println(moistureLevel);

  // Determine soil status based on threshold
  int soilStatus = (moistureLevel > moistureThreshold) ? HIGH : LOW;

  // Send the soil status to receiver
  if (esp_now_send(receiverAddress, (uint8_t *)&soilStatus, sizeof(soilStatus)) != 0) {
    Serial.println("Error sending data");
  } else {
    Serial.println(soilStatus == HIGH ? "Soil is dry" : "Soil is wet");
  }

  delay(1000);  // Adjust delay as needed
}
