#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "system.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "OrbitOLED/lib_OrbitOled/OrbitOled.h"
#include "altitude.h"

void
initDisplay (void);

void
introLine (void);

void
printString(char line_format, int32_t line_contents, uint8_t line_number);

void
OutputToDisplay (void);

#endif /*DISPLAY_H_*/
