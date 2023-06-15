#include <Arduino.h>

#define SERVO 0
#include <esp_now.h>
#include "WiFi.h"
#include <ESP32Servo.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#if SERVO

#define LED 2

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int out;
} struct_message;

// Struct message to store incoming data
struct_message myData;
// ESP32 servo object
Servo myservo;  

int servoPin = 5;

int currPos = 90;

// IR LED
const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.


// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));

  currPos += (myData.out == 1) ? 15 : (myData.out == 2) ? 30 : (myData.out == 3) ? -15 : (myData.out == 4) ? -30 : 0;

  myservo.write(currPos);
  // Flash on board LED for debugging purposes
  for(int i = 0; i < 4; i++){
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
  }
}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Setup led on board
  pinMode(LED, OUTPUT);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	myservo.setPeriodHertz(50);    // standard 50 hz servo
	myservo.attach(servoPin, 500, 2400); // attaches the servo on pin 18 to the servo object
  myservo.write(currPos);
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
  irsend.begin();

}
 
void loop() {
  Serial.println("NEC");
  irsend.sendNEC(0x02);
  delay(100);
}
#endif
