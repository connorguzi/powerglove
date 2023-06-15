//-----------------------------------------------------------------------------
// orient.cpp
// Application file for the orientation module.
//-----------------------------------------------------------------------------

#include "orient.h"

// TEST DEFINITIONS
#define PRINT_TEST 0
#define RANGE_TEST 0
#define JOLT_TEST 0


Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);
sensors_event_t event;

void orientInit(){
  // Hardware setup
  Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);
  bno.begin();
  pinMode(LED_BUILTIN, OUTPUT);

  // Callibration Setup
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);
  // Dirty Blocking code for callibration
  while(system < 2){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    bno.getCalibration(&system, &gyro, &accel, &mag);
  }
  Serial.println();
  Serial.println("Orientation Callibration Complete");
  Serial.println();

}


void printOrientation(){
    bno.getEvent(&event);
    Serial.printf("x:%lf\ty:%lf\n", event.orientation.x, event.orientation.y);
}


float getYaw(){
  bno.getEvent(&event);
  return event.orientation.x;
}

float getPitch(){
  bno.getEvent(&event);
  return event.orientation.y;
}


bool inRangeX(float start, float current, int range){
    // float diff = start - current;
    // if(diff < -180){
    //     diff += 360;
    // }
    // else if (diff > 180){
    //     diff = 360 - diff;
    // }
    // if(diff > range){
    //     return false;
    // }
    // return true;
    float diff = fabs(start - current);
    if (diff > 180){
        diff = 360 - diff;
    }
    if(diff > range){
        return false;
    }
    return true;
}

bool inRangeY(float start, float current, int range){
    float diff = fabs(start - current);
    if(diff > range){
        return false;
    }
    return true;
}

int vertJolt(){
  static unsigned long t = 0;
  int jolt = 0;
  imu::Vector<3> gyro = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  if(gyro.y() > 40 && (millis() - t > 500)){
    jolt = -1;
    t = millis();

  }
  else if(gyro.y() < -40 && (millis() - t > 500)){
    jolt = 1;
    t = millis();
  }
  return jolt;
}

#if PRINT_TEST
void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    orientInit();
}

void loop() {
  // put your main code here, to run repeatedly:
    printOrientation();
    delay(100);
}
#endif


#if RANGE_TEST

float initial_x;
float initial_y;
bool prevBoundsX = true;
bool prevBoundsY = true;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  orientInit();
  delay(2000);
  bno.getEvent(&event);
  initial_x = event.orientation.x;
  initial_y = event.orientation.y;
  Serial.printf("Inital Yaw: %lf\tInital Pitch:%lf\n", initial_x, initial_y);

}

void loop() {
  // put your main code here, to run repeatedly:
  float curr_x = getYaw();
  float curr_y = getPitch();
  bool inBoundsX = inRangeX(initial_x, curr_x, X_RANGE);
  bool inBoundsY = inRangeY(initial_y, curr_y, Y_RANGE);
  if(inBoundsX != prevBoundsX){
    Serial.println((inBoundsX == false) ? "X out of bounds" : "X in bounds");
    Serial.printf("Current yaw: %lf\n\n", curr_x);
    prevBoundsX = inBoundsX;
  }
  if(inBoundsY != prevBoundsY){
    Serial.println((inBoundsY == false) ? "Y out of bounds" : "Y in bounds");
    Serial.printf("Current pitch: %lf\n\n", curr_y);

    prevBoundsY = inBoundsY;
  }
}

#endif

#if JOLT_TEST
void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    orientInit();
}

void loop() {
  // put your main code here, to run repeatedly:
  int isJolt = vertJolt();
  if(isJolt == 1){
    Serial.println("UP");
  }
  else if(isJolt == -1){
    Serial.println("DOWN");
  }
  delay(100);
}
#endif