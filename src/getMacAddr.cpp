#include <Arduino.h>

#define MAC_ADDRESS 0

#include "WiFi.h"
 
#if MAC_ADDRESS
void setup(){
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  Serial.println(WiFi.macAddress());
}
 
void loop(){

}
#endif
