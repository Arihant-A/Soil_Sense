#include <ESP8266WiFi.h>
#include <espnow.h>

void setup() {
  Serial.begin(9600);  // Start serial communication for debugging
  pinMode(5, OUTPUT);  // Set D1 as output to control LED/relay
  
  // Set ESP8266 to Station mode (for ESP-NOW communication)
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();  // Disconnect from any existing Wi-Fi network
  
  // Initialize ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW initialization failed");
    return;
  }
  
  // Register the callback function to handle incoming data
  esp_now_register_recv_cb(OnDataReceived);
}

// Callback function to handle incoming data
void OnDataReceived(uint8_t *mac_addr, uint8_t *data, uint8_t len) {
  int receivedState = data[0];  // Extract the received state from data
  
  // Use the received state to control an LED/relay on pin D1
  digitalWrite(5, receivedState);  // HIGH = ON, LOW = OFF
}
void loop() {
  // The actual work is done in the callback, so the loop is empty
  delay(10);  // Just a small delay to prevent CPU overload
}
