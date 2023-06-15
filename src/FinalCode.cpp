#include "orient.h"
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <esp_now.h>
#include <WiFi.h>
#include <BleMouse.h>
#include "touch.h"
#define MAIN 1
#define IR_TEST 0

#define NODE_LED1 23
#define NODE_LED2 17
#define NODE_LED3 18
#define NODE_LED4 19


// Range is 1-4 for all Nodes
typedef struct struct_message {
  int out;
} struct_message;

struct_message sendMsg;

uint8_t ledAdd[] = {0x94, 0x3C, 0xC6, 0x38, 0x02, 0x4C};

// SPEAKER MAC Address - 94:3C:C6:34:66:74
uint8_t speakerAdd[] = {0x94, 0x3C, 0xC6, 0x34, 0x66, 0x74};

// Servo MAC Address - A0:B7:65:4C:4F:C0
uint8_t servoAdd[] = {0xA0, 0xB7, 0x65, 0x4C, 0x4F, 0xC0};

esp_now_peer_info_t ledInfo;
esp_now_peer_info_t speakerInfo;
esp_now_peer_info_t servoInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

typedef enum Nodes{
  NONE,
  LED,
  LAPTOP,
  SPEAKER,
  SERVO,
}Nodes;

// Global Variables
Nodes curNode = NONE;  

const uint16_t kRecvPin = 16;
IRrecv irrecv(kRecvPin);
decode_results results;

bool isFacing = false;
bool isPressed = false;


float init_yaw;
float init_pitch;
float last_yaw;
float last_pitch;
float current_yaw;
float current_pitch;

int jolt = 0;
touchEvents touch_val = NO_EVENT;
int prev_touch = 0;

BleMouse bleMouse;

#if MAIN

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(NODE_LED1, OUTPUT);
  pinMode(NODE_LED2, OUTPUT);
  pinMode(NODE_LED3, OUTPUT);
  pinMode(NODE_LED4, OUTPUT);
  WiFi.mode(WIFI_STA);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  irrecv.enableIRIn();  // Start the receiver

  esp_now_register_send_cb(OnDataSent);

  // Register peers
  memcpy(ledInfo.peer_addr, ledAdd, 6);
  ledInfo.channel = 0;  
  ledInfo.encrypt = false;
  
  memcpy(speakerInfo.peer_addr, speakerAdd, 6);
  speakerInfo.channel = 0;  
  speakerInfo.encrypt = false;

  memcpy(servoInfo.peer_addr, servoAdd, 6);
  servoInfo.channel = 0;  
  servoInfo.encrypt = false;
  // Add peers       
  if (esp_now_add_peer(&ledInfo) != ESP_OK){
    Serial.println("Failed to add led");
    return;
  }
  if (esp_now_add_peer(&speakerInfo) != ESP_OK){
    Serial.println("Failed to add speaker");
    return;
  }
  // if (esp_now_add_peer(&servoInfo) != ESP_OK){
  //   Serial.println("Failed to add servo");
  //   return;
  // }
  bleMouse.begin();

  orientInit();

}

void loop() {
  if(curNode == NONE){
    if (irrecv.decode(&results)) {
      if(results.value == 0x00FFE01FUL){
        curNode = LED;
        Serial.println("Facing LED");
        digitalWrite(NODE_LED1, HIGH);
        init_yaw = getYaw();
        init_pitch = getPitch();
        Serial.printf("InitYaw: %lf\tInitPitch: %lf\n", init_yaw, init_pitch);

      }
      else if(results.value == 0x02){
        curNode = SERVO;
        Serial.println("Facing SERVO");
        digitalWrite(NODE_LED2, HIGH);
        init_yaw = getYaw();
        init_pitch = getPitch();
        Serial.printf("InitYaw: %lf\tInitPitch: %lf\n", init_yaw, init_pitch);
      }
      else if(results.value == 0x01134){
        curNode = SPEAKER;
        Serial.println("Facing SPEAKER");
        digitalWrite(NODE_LED3, HIGH);
        init_yaw = getYaw();
        init_pitch = getPitch();
        Serial.printf("InitYaw: %lf\tInitPitch: %lf\n", init_yaw, init_pitch);
      }
      else if(results.value == 0x720){
        curNode = LAPTOP;
        Serial.println("Facing Laptop");
        digitalWrite(NODE_LED4, HIGH);
        init_yaw = getYaw();
        init_pitch = getPitch();
        last_yaw = init_yaw;
        last_pitch = init_pitch;
        Serial.printf("InitYaw: %lf\tInitPitch: %lf\n", init_yaw, init_pitch);
      }
      
      else{
        irrecv.resume();  // Receive the next value
      }
    }
  }
  else{
    curNode = (inRangeX(init_yaw, getYaw()) && inRangeY(init_pitch, getPitch())) ? curNode : NONE;
    touch_val = checkTouch();
    switch (curNode){
      // Servo Code
      case SERVO:
      switch(touch_val){
        case TOUCH_1_SINGLE:
          sendMsg.out = 1;
          esp_now_send(servoAdd, (uint8_t *) &sendMsg, sizeof(sendMsg));
          break;
        case TOUCH_1_DOUBLE:
          sendMsg.out = 2;
          esp_now_send(servoAdd, (uint8_t *) &sendMsg, sizeof(sendMsg));
          break;
        case TOUCH_2_SINGLE:
          sendMsg.out = 3;
          esp_now_send(servoAdd, (uint8_t *) &sendMsg, sizeof(sendMsg));
          break;
        case TOUCH_2_DOUBLE:
          sendMsg.out = 4;
          esp_now_send(servoAdd, (uint8_t *) &sendMsg, sizeof(sendMsg));
          break;
        }
      break;
      // LED code
      case LED:
        switch(touch_val){
          case TOUCH_1_SINGLE:
            sendMsg.out = 1;
            esp_now_send(ledAdd, (uint8_t *) &sendMsg, sizeof(sendMsg));
            break;
          case TOUCH_1_DOUBLE:
            sendMsg.out = 2;
            esp_now_send(ledAdd, (uint8_t *) &sendMsg, sizeof(sendMsg));
            break;
          case TOUCH_2_SINGLE:
            sendMsg.out = 3;
            esp_now_send(ledAdd, (uint8_t *) &sendMsg, sizeof(sendMsg));
            break;
          case TOUCH_2_DOUBLE:
            sendMsg.out = 4;
            esp_now_send(ledAdd, (uint8_t *) &sendMsg, sizeof(sendMsg));
            break;
        }
        break;
      // Laptop Code
      case LAPTOP:
      {
        switch(touch_val){
          case TOUCH_1_SINGLE:
            bleMouse.click(MOUSE_LEFT);
            break;
          case TOUCH_1_DOUBLE:
            bleMouse.click(MOUSE_RIGHT);
            break;
          case TOUCH_2_SINGLE:
            jolt = vertJolt();
            // Blocking code is evil but sometimes evil is necessary
            while(!jolt) jolt = vertJolt();
            if(jolt == 1){
              bleMouse.move(0,0,5);
            }
            else{
              bleMouse.move(0,0,-5);
            }
            break;
          case TOUCH_2_DOUBLE:
            if(isPressed){
              bleMouse.release();
              isPressed = false;
            }
            else{
              bleMouse.press();
              isPressed = true;
            }
            break;
        }
        current_yaw = getYaw();
        current_pitch = getPitch();
        int x = -int((last_yaw - current_yaw) * 35);
        int y = int((last_pitch - current_pitch) * 25);
        bleMouse.move(x, y);
        last_yaw = current_yaw;
        last_pitch = current_pitch;
        break;
      }
              // Speaker code
      case SPEAKER:
        switch(touch_val){
          case TOUCH_1_SINGLE:
            sendMsg.out = 1;
            esp_now_send(speakerAdd, (uint8_t *) &sendMsg, sizeof(sendMsg));
            break;
          case TOUCH_1_DOUBLE:
            sendMsg.out = 2;
            esp_now_send(speakerAdd, (uint8_t *) &sendMsg, sizeof(sendMsg));
            break;
          case TOUCH_2_SINGLE:
            sendMsg.out = 3;
            esp_now_send(speakerAdd, (uint8_t *) &sendMsg, sizeof(sendMsg));
            break;
          case TOUCH_2_DOUBLE:
            sendMsg.out = 4;
            esp_now_send(speakerAdd, (uint8_t *) &sendMsg, sizeof(sendMsg));
            break;
        }
        break;

        
    }
    // Turn off LEDS if not facing the corresponding nodes
    if(!curNode){
        digitalWrite(NODE_LED1, LOW);
        digitalWrite(NODE_LED2, LOW);
        digitalWrite(NODE_LED3, LOW);
        digitalWrite(NODE_LED4, LOW);
        bleMouse.release();
        irrecv.resume();  // Receive the next value

    }
  }
  
}

#endif


#if IR_TEST

void setup() {
  pinMode(NODE_LED1, OUTPUT);
  pinMode(NODE_LED2, OUTPUT);

  Serial.begin(115200);
  irrecv.enableIRIn();  // Start the receiver
  while (!Serial)  // Wait for the serial connection to be establised.
    delay(50);
  orientInit();
  Serial.println();
  Serial.print("IRrecvDemo is now running and waiting for IR message on Pin ");
  Serial.println(kRecvPin);
}

void loop() {
  if(!isFacing){
    if (irrecv.decode(&results)) {
      
      if(results.value == 0x00FFE01FUL){
        isFacing = true;
        Serial.println("Facing Node1");
        digitalWrite(NODE_LED1, HIGH);
        init_yaw = getYaw();
        init_pitch = getPitch();
        Serial.printf("InitYaw: %lf\tInitPitch: %lf\n", init_yaw, init_pitch);

      }
      else if(results.value == 0x02){
        isFacing = true;
        Serial.println("Facing servo");
        digitalWrite(NODE_LED2, HIGH);
        init_yaw = getYaw();
        init_pitch = getPitch();
        Serial.printf("InitYaw: %lf\tInitPitch: %lf\n", init_yaw, init_pitch);
      }
      else{
        irrecv.resume();  // Receive the next value
      }
    }
  }
  else{
    isFacing = inRangeX(init_yaw, getYaw()) && inRangeY(init_pitch, getPitch());
    if(!isFacing){
        digitalWrite(NODE_LED1, LOW);

        digitalWrite(NODE_LED2, LOW);
        irrecv.resume();  // Receive the next value

    }
  }
  
}

#endif


