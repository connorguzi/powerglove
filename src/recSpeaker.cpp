#include <Arduino.h>

#define SPEAKER 0
#include <esp_now.h>
#include "WiFi.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>

#define LED_BUILTIN 2

#define SPEAKER_PIN 4

#if SPEAKER


// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int out;
} struct_message;

// Struct message to store incoming data
struct_message myData;



bool isOn = false;

// IR LED
const uint16_t kIrLed = 23;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.


// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  tone(SPEAKER_PIN, 220 * myData.out);
  delay(500);
  noTone(SPEAKER_PIN);

}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
    randomSeed(1);
  // Setup led on board
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SPEAKER_PIN, OUTPUT);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
 
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
  irsend.begin();

}
 
void loop() {
  irsend.sendNEC(0x01134);
  delay(100);
}
#endif
