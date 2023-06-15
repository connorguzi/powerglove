//-----------------------------------------------------------------------------
// touch.cpp
// Implementation file for the touch module. This is a simple module but is kept 
//      separate for clarity sake
//-----------------------------------------------------------------------------
#include "touch.h"


#define TOUCH_TEST 0
#define TOUCH_TEST2 0

#define NOISE_TEST 0


typedef enum touchState{
    WAIT,
    TOUCHED,
    RELEASED,
    RETURN_RELEASED,
    TOUCHED_AGAIN,
}touchState;


touchState state1 = WAIT;
touchState state2 = WAIT;

unsigned long lastTouchTime;

// Global variables
bool touched1 = false; 
bool touched2 = false;
bool touched3 = false;
bool touched4 = false;

bool touch1_bools[LENGTH] = {0};
bool touch2_bools[LENGTH] = {0};
bool touch3_bools[LENGTH] = {0};
bool touch4_bools[LENGTH] = {0};

int touch_it = 0;


// checkTouch():
//      Returns which capacitive touch sensor is currently being touched
// int checkTouch(){

//     bool exists1 = false;
//     bool exists2 = false;
//     bool exists3 = false;
//     bool exists4 = false;
//     Serial.println(touchRead(TOUCH_2));
//     touch1_bools[touch_it] = (touchRead(TOUCH_1) < TOUCH_THRESHOLD) ? true: false;
//     touch2_bools[touch_it] = (touchRead(TOUCH_2) < TOUCH_THRESHOLD) ? true: false;
//     touch3_bools[touch_it] = (touchRead(TOUCH_3) < TOUCH_THRESHOLD) ? true: false;
//     touch4_bools[touch_it++] = (touchRead(TOUCH_4) < TOUCH_THRESHOLD) ? true: false;

//     touch_it = (touch_it == LENGTH-1) ? 0 : touch_it;
    
//     // For our sake we are only looking for one sensor at a time
//     for(int i = 0; i < LENGTH; i++){
//         if(touch1_bools[i]){
//             return 1;
//         }
//         if(touch2_bools[i]){
//             return 2;
//         }     
//         if(touch3_bools[i]){
//             return 3;
//         }      
//         if(touch4_bools[i]){
//             return 4;
//         }
//         // exists1 = (touch1_bools[i] == true) ? true: exists1;
//         // exists2 = (touch2_bools[i] == true) ? true: exists2;
//         // exists3 = (touch3_bools[i] == true) ? true: exists3;
//         // exists4 = (touch4_bools[i] == true) ? true: exists4;
//     }
    
//     // touched1 = exists1;
//     // touched2 = exists2;
//     // touched3 = exists3;
//     // touched4 = exists4;
//     return 0;
// }

unsigned long lastTime;

touchEvents checkTouch(){
    static bool prevExists1 = false;
    static bool prevExists2 = false;
    bool exists1 = false;
    bool exists2 = false;
    // Serial.println(touchRead(TOUCH_1));
    touch1_bools[touch_it] = (touchRead(TOUCH_1) < TOUCH_THRESHOLD) ? true: false;
    touch2_bools[touch_it++] = (touchRead(TOUCH_2) < TOUCH_THRESHOLD) ? true: false;

    touch_it = (touch_it == LENGTH-1) ? 0 : touch_it;
    
    // For our sake we are only looking for one sensor at a time
    for(int i = 0; i < LENGTH; i++){
        if(touch1_bools[i]){
            exists1 = true;
            break;
        }
        if(touch2_bools[i]){
            exists2 = true;
            break;
        }
    }

    // Check to see if the touch sensor has just been touched or released
    if(prevExists1 != exists1 || state1 == RELEASED){
        prevExists1 = exists1;
        // Run touch sensor state machine
        switch (state1)
            {
            case WAIT:
                if(exists1){
                    state1 = TOUCHED;
                }
                return NO_EVENT;
            case TOUCHED:
                if(!exists1){
                    state1 = RELEASED;
                    lastTouchTime = millis();
                }
                return NO_EVENT;

            case RELEASED:
                if(exists1){
                    state1 = TOUCHED_AGAIN;
                    return NO_EVENT;
                }
                else if (millis() - lastTouchTime > 400){
                    state1 = WAIT;
                    return TOUCH_1_SINGLE;
                }
                return NO_EVENT;
            case TOUCHED_AGAIN:
                if(!exists1){
                    state1 = WAIT;
                    return TOUCH_1_DOUBLE;
                }
                return NO_EVENT;
            }
        }

    if(prevExists2 != exists2 || state2 == RELEASED){
        prevExists2 = exists2;
        // Run touch sensor state machine
        switch (state2)
            {
            case WAIT:
                if(exists2){
                    state2 = TOUCHED;
                }
                return NO_EVENT;
            case TOUCHED:
                if(!exists2){
                    state2 = RELEASED;
                    lastTouchTime = millis();
                }
                return NO_EVENT;

            case RELEASED:
                if(exists2){
                    state2 = TOUCHED_AGAIN;
                    return NO_EVENT;
                }
                else if (millis() - lastTouchTime > 400){
                    state2 = WAIT;
                    return TOUCH_2_SINGLE;
                }
                return NO_EVENT;
            case TOUCHED_AGAIN:
                if(!exists2){
                    state2 = WAIT;
                    return TOUCH_2_DOUBLE;
                }
                return NO_EVENT;
            }
        }


    return NO_EVENT;
}

// printTouch():
    // Function used for debugging and showing what the given touch value was
void printTouch(touchEvents val){
    static touchEvents prev = NO_EVENT;
    if(val != prev){
        prev = val;
        if(val == TOUCH_1_SINGLE){
            Serial.println("Touch 1 Single");
        }
        else if(val == TOUCH_1_DOUBLE){
            Serial.println("Touch 1 Double");
        }
        else if(val == TOUCH_2_SINGLE){
            Serial.println("Touch 2 Single");
        }
        else if(val == TOUCH_2_DOUBLE){
            Serial.println("Touch 2 Double");
        }
    }
}


#if TOUCH_TEST
bool prevtouched1 = false; 
bool prevtouched2 = false;
bool prevtouched3 = false;
bool prevtouched4 = false;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000); // give me time to bring up serial monitor
  Serial.println("");
  Serial.println("Touch Test");
}

void loop() {
  int tou = checkTouch();
  switch (tou)
  {
    case 1:
        touched1 = true;
        touched2 = false;
        touched3 = false;
        touched4 = false;
        break;
    case 2: 
        touched1 = false;
        touched2 = true;
        touched3 = false;
        touched4 = false;
        break;
    case 3:
        touched1 = false;
        touched2 = false;
        touched3 = true;
        touched4 = false;
        break;
    case 4:
        touched1 = false;
        touched2 = false;
        touched3 = false;
        touched4 = true;
        break;    
    default:
        touched1 = false;
        touched2 = false;
        touched3 = false;
        touched4 = false;
        break;
  }
  if(touched1 != prevtouched1){
    Serial.println((touched1 == true) ? "Touch1: Touched" : "Touch1: Untouched");
    prevtouched1 = touched1;
  }
  if(touched2 != prevtouched2){
    Serial.println((touched2 == true) ? "Touch2: Touched" : "Touch2: Untouched");
    prevtouched2 = touched2;
  }
  if(touched3 != prevtouched3){
    Serial.println((touched3 == true) ? "Touch3: Touched" : "Touch3: Untouched");
    prevtouched3 = touched3;
  }
  if(touched4 != prevtouched4){
    Serial.println((touched4 == true) ? "Touch4: Touched" : "Touch4: Untouched");
    prevtouched4 = touched4;
  }
}
#endif
#if TOUCH_TEST2

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000); // give me time to bring up serial monitor
  Serial.println("");
  Serial.println("Touch Test");
}

void loop() {
  touchEvents tou = checkTouch();
  if(tou == TOUCH_1_SINGLE){
    Serial.println("Touch 1 Single");
  }
   else if(tou == TOUCH_1_DOUBLE){
    Serial.println("Touch 1 Double");
  }
   else if(tou == TOUCH_2_SINGLE){
    Serial.println("Touch 2 Single");
  }
   else if(tou == TOUCH_2_DOUBLE){
    Serial.println("Touch 2 Double");
  }
  
}
#endif


#if NOISE_TEST

void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    delay(1000); // give me time to bring up serial monitor
    Serial.println("");

    Serial.println("Noise Touch Test");
    int tou = checkTouch();
        while(tou == 0){
            tou = checkTouch();
        }
    unsigned long t;
    if(tou == 1){
        t = millis();
        while(millis() - t < 2000){
            if(millis() % 100 == 0){
                Serial.println(touchRead(TOUCH_1));

            }
        }
    }
}

void loop() {

}
#endif