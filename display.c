//*****************************************************************************
//
//  Display - Functions used to output values on Display and UART
//
//  Author:  N. James
//           L. Trenberth
//           M. Arunchayanon
//     Last modified:   20.4.2019
//*****************************************************************************


#include "display.h"
#include "system.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "OrbitOLED/lib_OrbitOled/OrbitOled.h"
#include "altitude.h"
#include "yaw.h"
#include "control.h"
#include "uart.h"


//  *****************************************************************************
//  initDisplay:        Initialises Display using OrbitLED functions
void initDisplay (void)
{
    // Initialise the Orbit OLED display
    OLEDInitialise ();
}


//  *****************************************************************************
//  introLine:          Prints the intro line on the OLED Display
void introLine (void)
{
    OLEDStringDraw ("Heli Project", 0, 0);
}


//  *****************************************************************************
//  printString:        Prints the input format and line contents on the given line number on OLED Display
//  TAKES:              line_format - The format to print the string in, including a integer placeholder
//                      line_contents - The integer to print on the line
//                      line_number - The line number integer to print the string on.
void printString(char* restrict line_format, int32_t line_contents, uint8_t line_number)
{
    char string[MAX_STR_LEN + 1];
    usnprintf (string, sizeof(string), line_format, line_contents);
    OLEDStringDraw (string, 0, line_number); // Update line on display.
}


//*****************************************************************************
//  initButtonCheck:    Initialises left and up buttons on the micro-controller
void initButtonCheck (void) {
    SysCtlPeripheralReset (LEFT_BUT_PERIPH);//setting up the LEFT button GPIO
    SysCtlPeripheralReset (UP_BUT_PERIPH);//setting the UP button GPIO
    SysCtlPeripheralReset (DOWN_BUT_PERIPH);//setting the DOWN button GPIO
    SysCtlPeripheralReset (RIGHT_BUT_PERIPH);//setting the RIGHT button GPIO
}


//  *****************************************************************************
//  OutputToDisplay:    Displays the helicopter altitude, height and references
//  NOTE:               This function is not currently implemented, though is included for testing
void OutputToDisplay (void)
{
    printString("Altitude = %4d%%", percentAltitude(), 0);
    printString("Yaw Angle = %4d", getYaw(), 1);
    printString("Alt Ref = %4d", GetAltRef(), 2);
    printString("Yaw Ref = %4d", GetYawRef(), 3);
}


//*****************************************************************************
//  OutputToDisplay:    Uses UART to send yaw and altitude references,
//                      duty cycles and the current mode
void OutputToUART (void)
{
    char statusStr[MAX_STR_LEN + 1];
    if (slowTick)
    {
        slowTick = false;
        usprintf (statusStr, "YawRef=%2d Yaw=%2d | \r\n", GetYawRef(), getYaw()); // Form status message
        UARTSend (statusStr); // Send to the console
        usprintf (statusStr, "AltRef=%2d Alt=%2d | \r\n", GetAltRef(), percentAltitude());
        UARTSend (statusStr);
        usprintf (statusStr, "MDut=%2d TDuty=%2d | \r\n", getMainDuty(), getTailDuty());
        UARTSend (statusStr);
        usprintf (statusStr, "Mode=%s | \r\n", getMode());
        UARTSend (statusStr);
    }
}

