#include <Arduino.h>

#define LAPTOP_TRANS 0
#include <esp_now.h>
#include "WiFi.h"
#include <ESP32Servo.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>


#if LAPTOP_TRANS 

// IR LED
const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

void setup(){
    Serial.begin(115200);
    irsend.begin();
}

void loop(){
  irsend.sendNEC(0x720);
  delay(100);
}

#endif