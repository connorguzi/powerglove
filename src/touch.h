//-----------------------------------------------------------------------------
// touch.h
// Header file for the touch module. This is a simple module but is kept 
//      separate for clarity sake
//-----------------------------------------------------------------------------

#include <Arduino.h>

// #define TOUCH_1 15
// #define TOUCH_2 4
// #define TOUCH_3 13
// #define TOUCH_4 14

typedef enum touchEvents{
    NO_EVENT,
    TOUCH_1_SINGLE,
    TOUCH_1_DOUBLE,
    TOUCH_2_SINGLE,
    TOUCH_2_DOUBLE,
}touchEvents;

#define TOUCH_1 33
#define TOUCH_2 32
#define TOUCH_THRESHOLD 32
#define LENGTH 8

#ifndef TOUCH_H
#define	TOUCH_H
touchEvents checkTouch();

void printTouch(touchEvents val);

#endif