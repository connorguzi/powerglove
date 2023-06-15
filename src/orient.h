//-----------------------------------------------------------------------------
// orient.h
// Header file for the orientation module.
//-----------------------------------------------------------------------------

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Arduino.h>



#ifndef ORIENT_H
#define	ORIENT_H

#define X_RANGE 45
#define Y_RANGE 30
#define LED_BUILTIN 2


// orientInit():
    // Initializes orientation module
void orientInit();

// printOrientation():
    // Prints the current yaw and pitch to the serial monitor
void printOrientation();

// getYaw():
    // Returns: current yaw orientation
float getYaw();

// getPitch():
    // Returns: current pitch orientation
float getPitch();

// inRangeX():
    // Determines if orientation is within x bounds of bounding box
    // Params:  start - initial yaw
    //          current - current yaw
    //          range - range of bounding box, defaults to X_RANGE
    // Returns: True if in box, false if not
bool inRangeX(float start, float current, int range=X_RANGE);

// inRangeY():
    // Determines if orientation is within y bounds of bounding box
    // Params:  start - initial pitch
    //          current - current pitch
    //          range - range of bounding box, defaults to Y_RANGE
    // Returns: True if in box, false if not
bool inRangeY(float start, float current, int range=Y_RANGE);

// vertJolt():
    // Determines if there is a vertical jolt in the system
    // Returns: 1 for upwards movement, -1 for downwards, 0 for no movement
int vertJolt();

#endif