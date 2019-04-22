#ifndef SYSTEM_H_
#define SYSTEM_H_

#define BUF_SIZE 10
#define SAMPLE_RATE_HZ 40
#define MAX_STR_LEN 16
#define UART_USB_BASE           UART0_BASE
#define SYSTICK_RATE_HZ    100

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "buttons4.h"
#include "circBufT.h"


static uint32_t g_ulSampCnt;    // Counter for the interrupts

void
SysTickIntHandler(void);

void
initSysTick (void);

void
initClock (void);

#endif /*SYSTEM_H_*/
