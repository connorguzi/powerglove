#include <Arduino.h>

#define LED 0
#define LED_TEST 0
#include <esp_now.h>
#include "WiFi.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>

#define LED_BUILTIN 2

#define R 21
#define G 22
#define B 23

int prev_red = 255;
int prev_blue = 255;
int prev_green = 255;

void setColor(int red, int green, int blue);
void advanceColorWheel();
void decreaseColorWheel();
void randColor();

void setColor(int red, int green, int blue){
    analogWrite(R, red);
    analogWrite(G, green);
    analogWrite(B, blue);
}

void advanceColorWheel(){
    if(prev_red == 255 && prev_green == 0 && prev_blue == 0){
        setColor(255, 255, 0); // 1
        prev_green = 255;
    }
    else if(prev_red == 255 && prev_green == 255 && prev_blue == 0){
        setColor(255, 255, 255); // 2
        prev_blue = 255;
    }
    else if(prev_red == 255 && prev_green == 255 && prev_blue == 255){
        setColor(0, 255, 255); // 3
        prev_red = 0;
    }
    else if(prev_red == 0 && prev_green == 255 && prev_blue == 255){
        setColor(0, 0, 255); // 4
        prev_green = 0;
    }
    else if(prev_red == 0 && prev_green == 0 && prev_blue == 255){
        setColor(255, 0, 255); // 5
        prev_red = 255;
    }
    else if(prev_red == 255 && prev_green == 0 && prev_blue == 255){
        setColor(255, 0, 0); // 6
        prev_blue = 0;
    }
}

void decreaseColorWheel(){
    if(prev_red == 255 && prev_green == 0 && prev_blue == 0){
        setColor(255, 0, 255); // 5
        prev_blue = 255;
    }
    else if(prev_red == 255 && prev_green == 0 && prev_blue == 255){
        setColor(0, 0, 255); // 4
        prev_red = 0;
    }
    else if(prev_red == 0 && prev_green == 0 && prev_blue == 255){
        setColor(0, 255, 255); // 3
        prev_green = 255;
    }
    else if(prev_red == 0 && prev_green == 255 && prev_blue == 255){
        setColor(255, 255, 255); // 2
        prev_red = 255;
    }
    else if(prev_red == 255 && prev_green == 255 && prev_blue == 255){
        setColor(255, 255, 0); // 1
        prev_blue = 255;
    }
    else if(prev_red == 255 && prev_green == 0 && prev_blue == 255){
        setColor(255, 0, 0); // 6
        prev_green = 0;
    }
}

void randColor(){
    long rand = random(1, 7);
    if(rand == 1){
        setColor(255, 255, 0);
        prev_red = 255;
        prev_green = 255;
        prev_blue = 0;

    }
    else if(rand == 2){
        setColor(255, 255, 255); // 2
        prev_red = 255;
        prev_green = 255;
        prev_blue = 255;
    }
    else if(rand == 3){
        setColor(0, 255, 255); // 3
        prev_red = 0;
        prev_green = 255;
        prev_blue = 255;
    }
    else if(rand == 4){
        setColor(0, 0, 255); // 4
        prev_red = 0;
        prev_green = 0;
        prev_blue = 255;
    }
    else if(rand == 5){
        setColor(255, 0, 255); // 5
        prev_red = 255;
        prev_green = 0;
        prev_blue = 255;
    }
    else if(rand == 6){
        setColor(255, 0, 0); // 6
        prev_red = 255;
        prev_green = 0;
        prev_blue = 0;
    }
    
}

#if LED


// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int out;
} struct_message;

// Struct message to store incoming data
struct_message myData;



bool isOn = false;

// IR LED
const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.


// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  if(myData.out == 1){
    isOn = !isOn;
    if(isOn){
        setColor(prev_red, prev_green, prev_blue);
    }
    else{
        setColor(0,0,0);
    }
  }

  else{
    if(isOn){
        if(myData.out == 2){
            randColor();
        }
        else if(myData.out == 3){
            advanceColorWheel();
        }
        else if(myData.out == 4){
            decreaseColorWheel();
        }
    }
  }
}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
    randomSeed(1);
  // Setup led on board
  pinMode(LED_BUILTIN, OUTPUT);
    pinMode(R, OUTPUT);
    pinMode(G, OUTPUT);
    pinMode(B, OUTPUT);
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
  Serial.println("NEC");
  irsend.sendNEC(0x00FFE01FUL);
  delay(100);
}
#endif

#if LED_TEST
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Setup led on board
  pinMode(LED_BUILTIN, OUTPUT);
    pinMode(R, OUTPUT);
    pinMode(G, OUTPUT);
    pinMode(B, OUTPUT);


}
 
void loop() {
    setColor(255, 0, 0);
  delay(250);
      setColor(255, 255, 0);
  delay(250);
      setColor(255, 255, 255);
  delay(250);
      setColor(175, 175, 175);
  delay(250);
      setColor(50, 50, 50);
  delay(250);
  
      setColor(0, 255, 255);
  delay(250);
      setColor(0, 0, 255);
  delay(250);
  
}
#endif