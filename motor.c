 /**********************************************************

 This is comment so please fucking register
 
 **********************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h" //Needed for pin configure
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "buttons4.h"

#include "inc/hw_ints.h"
#include "utils/ustdlib.h"
#include "stdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"

/**********************************************************
 * Generates a single PWM signal on Tiva board pin J4-05 =
 * PC5 (M0PWM7).  This is the same PWM output as the
 * helicopter main rotor.
 **********************************************************/

/**********************************************************
 * Constants
 **********************************************************/
// Systick configuration
#define SYSTICK_RATE_HZ    500

// PWM configuration
#define PWM_START_RATE_HZ  300
#define PWM_RATE_STEP_HZ   50
#define PWM_RATE_MIN_HZ    50
#define PWM_RATE_MAX_HZ    400
#define PWM_DUTY_MAX       95
#define PWM_DUTY_MIN       5
#define PWM_DUTY_RATE_STEP 5
#define PWM_START_DUTY     50
#define PWM_DIVIDER_CODE   SYSCTL_PWMDIV_4
#define PWM_DIVIDER        4

//Second PWM Config
#define PWM2_RATE_HZ       200
#define PWM2_DUTY          10

//  PWM Hardware Details M0PWM7 (gen 3)
//  ---Main Rotor PWM: PC5, J4-05
#define PWM_MAIN_BASE        PWM0_BASE
#define PWM_MAIN_GEN         PWM_GEN_3
#define PWM_MAIN_OUTNUM      PWM_OUT_7
#define PWM_MAIN_OUTBIT      PWM_OUT_7_BIT
#define PWM_MAIN_PERIPH_PWM  SYSCTL_PERIPH_PWM0
#define PWM_MAIN_PERIPH_GPIO SYSCTL_PERIPH_GPIOC
#define PWM_MAIN_GPIO_BASE   GPIO_PORTC_BASE
#define PWM_MAIN_GPIO_CONFIG GPIO_PC5_M0PWM7
#define PWM_MAIN_GPIO_PIN    GPIO_PIN_5


//PWM Hardware Details M1PWM5 (gen 2)
#define PWM_SEC_BASE        PWM1_BASE
#define PWM_SEC_GEN         PWM_GEN_2
#define PWM_SEC_OUTNUM      PWM_OUT_5
#define PWM_SEC_OUTBIT      PWM_OUT_5_BIT
#define PWM_SEC_PERIPH_PWM  SYSCTL_PERIPH_PWM1
#define PWM_SEC_PERIPH_GPIO SYSCTL_PERIPH_GPIOF
#define PWM_SEC_GPIO_BASE   GPIO_PORTF_BASE
#define PWM_SEC_GPIO_CONFIG GPIO_PF1_M1PWM5
#define PWM_SEC_GPIO_PIN    GPIO_PIN_1


/*******************************************
 *      Local prototypes
 *******************************************/
void initialisePWM (void);
void setPWM (uint32_t u32Freq, uint32_t u32Duty);
void initialisePWM2 (void);
void setPWM2 (uint32_t u32Freq2, uint32_t u32Duty2);

static uint32_t ui32Freq = PWM_START_RATE_HZ;
static uint32_t ui32Duty = PWM_START_DUTY;
static uint32_t ui32Freq2 = PWM_START_RATE_HZ;
static uint32_t ui32Duty2 = PWM2_DUTY;



/*********************************************************
 * initialisePWM
 * M0PWM7 (J4-05, PC5) is used for the main rotor motor
 *********************************************************/
void
initialisePWM (void)
{
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_GPIO);

    GPIOPinConfigure(PWM_MAIN_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_MAIN_GPIO_BASE, PWM_MAIN_GPIO_PIN);

    PWMGenConfigure(PWM_MAIN_BASE, PWM_MAIN_GEN,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    // Set the initial PWM parameters
    setPWM (PWM_START_RATE_HZ, PWM_START_DUTY);

    PWMGenEnable(PWM_MAIN_BASE, PWM_MAIN_GEN);

    // Disable the output.  Repeat this call with 'true' to turn O/P on.
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, false);
}

/********************************************************
 * Function to set the freq, duty cycle of M0PWM7
 ********************************************************/
void
setPWM (uint32_t ui32Freq, uint32_t ui32Duty)
{
    // Calculate the PWM period corresponding to the freq.
    uint32_t ui32Period =
        SysCtlClockGet() / PWM_DIVIDER / ui32Freq;

    PWMGenPeriodSet(PWM_MAIN_BASE, PWM_MAIN_GEN, ui32Period);
    PWMPulseWidthSet(PWM_MAIN_BASE, PWM_MAIN_OUTNUM,
        ui32Period * ui32Duty / 100);
}


/********************************************************
 * Function to set the freq, duty cycle of M1PWM5
 ********************************************************/
void
setPWM2 (uint32_t ui32Freq2, uint32_t ui32Duty2)
{
    // Calculate the PWM period corresponding to the freq.
    uint32_t ui32Period2 =
        SysCtlClockGet() / PWM_DIVIDER / ui32Freq2;

    PWMGenPeriodSet(PWM_SEC_BASE, PWM_SEC_GEN, ui32Period2);
    PWMPulseWidthSet(PWM_SEC_BASE, PWM_SEC_OUTNUM,
        ui32Period2 * ui32Duty2 / 100);
}

/*********************************************************
 * initialise2PWM
 * M1PWM5 (J3-10, PF1) is used for the secondary rotor motor
 *********************************************************/
void
initialisePWM2 (void)
{
    SysCtlPeripheralEnable(PWM_SEC_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_SEC_PERIPH_GPIO);

    GPIOPinConfigure(PWM_SEC_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_SEC_GPIO_BASE, PWM_SEC_GPIO_PIN);

    PWMGenConfigure(PWM_SEC_BASE, PWM_SEC_GEN,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    // Set the initial PWM parameters
    setPWM2 (PWM2_RATE_HZ, PWM2_DUTY);

    PWMGenEnable(PWM_SEC_BASE, PWM_SEC_GEN);

    // Disable the output.  Repeat this call with 'true' to turn O/P on.
    PWMOutputState(PWM_SEC_BASE, PWM_SEC_OUTBIT, false);
}

void
initmotor(void)
{
    // As a precaution, make sure that the peripherals used are reset
    SysCtlPeripheralReset (PWM_MAIN_PERIPH_GPIO); // Used for PWM output
    SysCtlPeripheralReset (PWM_MAIN_PERIPH_PWM);  // Main Rotor PWM
    SysCtlPeripheralReset (PWM_SEC_PERIPH_GPIO); // Used for PWM output
    SysCtlPeripheralReset (PWM_SEC_PERIPH_PWM);  // Main Rotor PWM
    initialisePWM ();
    initialisePWM2();

    // Initialisation is complete, so turn on the output.
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, true);
    PWMOutputState(PWM_SEC_BASE, PWM_SEC_OUTBIT, true);
}

void
updatePWM(void)
{
    //Will change a lot
    if ((checkButton (UP) == PUSHED) && (ui32Freq < PWM_RATE_MAX_HZ))
    {
        ui32Duty += PWM_DUTY_RATE_STEP;
        setPWM (ui32Freq, ui32Duty);
    }
    if ((checkButton (DOWN) == PUSHED) && (ui32Freq > PWM_RATE_MIN_HZ))
    {
        ui32Duty -= PWM_DUTY_RATE_STEP;
        setPWM (ui32Freq, ui32Duty);
    }
    if ((checkButton (LEFT) == PUSHED) && (ui32Duty > PWM_DUTY_MIN))
    {
        ui32Duty -= PWM_DUTY_RATE_STEP;
        setPWM (ui32Freq, ui32Duty);
    }
    if ((checkButton (RIGHT) == PUSHED) && (ui32Duty < PWM_DUTY_MAX))
    {
        ui32Duty += PWM_DUTY_RATE_STEP;
        setPWM (ui32Freq, ui32Duty);
    }
}
