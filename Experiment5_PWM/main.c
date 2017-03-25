/*
 * main.c
 * Main program for TIVA MCU - Experiment 5
 * PWM Signal Generation
 *
 * Lab Team:
 * Emmanuel Ramos
 * Reynaldo Belfort
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"

#define DESIRED_PWM_FRECUENCY 3000 //In Hz
#define DUTY_CYCLE 0.35  //% Duty Cycle (decimal value)

//Note Volatile guarantees that the comoiler will not eliminate the variables with that keywoerd

//Note: Be sure to add TARGET_IS_BLIZZARD_RB1 for ROM configuration

volatile uint32_t pwmClockFreq = 0;
volatile uint32_t pwmLoadValue = 0;

int main(void) {

	//----MCU Initialization----

	//TODO NOTE: ROM version of the API functions is used to reduce code size

	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN); //Set-up the clocking of the MCU to 40MHz
	//Clock the PWM module by the system clock
	SysCtlPWMClockSet(SYSCTL_PWMDIV_32); //Divide system clock by 32 to run the PWM at 1.25MHz

	//Enabling PWM1 module and Port D
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); //Port where the PWM pin will be selected

	//Selecting PWM generator 0 and port D pin 0 (PD0) aa a PWM output pin for module one
	GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0); //Set Port D pin 0 as output //TODO Checkout which ports can be used for PWM functionallity
	GPIOPinConfigure(GPIO_PD0_M1PWM0); //Select PWM Generator 0 from PWM Module 1

	pwmClockFreq = SysCtlClockGet() / 32; //TODO as Isnt the same as ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_64);? Is something being repeated?
	pwmLoadValue = (pwmClockFreq / DESIRED_PWM_FRECUENCY) - 1; //Load Value = (PWMGeneratorClock * DesiredPWMPeriod) - 1

	//-----------PWM Generator initialization-----------
	PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN); //Set a count-down generator type
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, pwmLoadValue); //Set PWM period determined by the load value
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, pwmLoadValue * DUTY_CYCLE);
	PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);

	//Enable PWM Generator
	PWMGenEnable(PWM1_BASE, PWM_GEN_0);

	//Main loop
	while (1) {

	}
}
