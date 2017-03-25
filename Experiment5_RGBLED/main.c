/*
 * main.c
 * Main program for TIVA MCU - Experiment 5
 * RGB LED
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

#define DESIRED_PWM_FRECUENCY 1000 //In Hz
#define DUTY_CYCLE 0.01  //% Duty Cycle (decimal value)

//Note Volatile guarantees that the comoiler will not eliminate the variables with that keywoerd

volatile uint32_t pwmClockFreq = 0;
volatile uint32_t pwmLoadValue = 0;
uint8_t colorCounter = 7;
bool pushFlag = false;

//Function declarations
void pushbutton_handler();

int main(void) {
	//----MCU Initialization----

	//TODO NOTE: ROM version of the API functions is used to reduce code size

	//---------System clock configuration---------
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN ); //Set-up the clocking of the MCU to 40MHz
	//PWM Module is clocked by the System Clock
	SysCtlPWMClockSet(SYSCTL_PWMDIV_32); //Divide system clock by 32 to run the PWM at 1.25MHz

	//---------Enabling preripherals---------
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1); //Enable PWM Module 1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); //Port where the PWM pin will be selected
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); //Port where the PWM pin will be selected
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //For switch button

	//---------GPIO Pin configuration---------
	GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0|GPIO_PIN_1); //Set Port D pin 0 as output //TODO Checkout which ports can be used for PWM functionallity
	GPIOPinTypePWM(GPIO_PORTA_BASE, GPIO_PIN_6); //Set Port D pin 0 as output //TODO Checkout which ports can be used for PWM functionallity
	GPIOPinConfigure(GPIO_PD0_M1PWM0); //Select PWM Output 0 from PWM Module 1  RED
	GPIOPinConfigure(GPIO_PD1_M1PWM1); //Select PWM Output 1 from PWM Module 1  GREEN
	GPIOPinConfigure(GPIO_PA6_M1PWM2); //Select PWM Output 2 from PWM Module 1  BLUE
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4); //Using SW1 button

	//---------Load value determination---------
	pwmClockFreq = SysCtlClockGet() / 32; //TODO as Isnt the same as ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_64);? Is something being repeated?
	pwmLoadValue = (pwmClockFreq / DESIRED_PWM_FRECUENCY) - 1; //Load Value = (PWMGeneratorClock * DesiredPWMPeriod) - 1

	//Module 1 - PWM Generator 0
	PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN|PWM_GEN_MODE_NO_SYNC); //Set a count-down generator type.
																				   //We also set the generator to produce independent signals for its two pwm outputs
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, pwmLoadValue); //Set PWM period determined by the load value
	//Module 1 - PWM Generator 1
	PWMGenConfigure(PWM1_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN); //Set a count-down generator type
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_1, pwmLoadValue); //Set PWM period determined by the load value

	//-----------Duty Cycle Set-----------
	//Enable PWM Outputs on PWM Module 1
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, pwmLoadValue * DUTY_CYCLE);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_1, pwmLoadValue * DUTY_CYCLE);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_2, pwmLoadValue * DUTY_CYCLE);
	PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT | PWM_OUT_1_BIT | PWM_OUT_2_BIT, true);

	//-----------PWM Generator enable-----------
	PWMGenEnable(PWM1_BASE, PWM_GEN_0); //Enable PWM Generator 0
	PWMGenEnable(PWM1_BASE, PWM_GEN_1); //Enable PWM Generator 2

	//-----------Preparing the switch buttons-----------
	//Pins 0 and 4 have to be unlocked first in the GPIO commit control register before they can be used
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
	//Configure as input pins
	GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	//-----------Interrupt initialization-----------
	GPIOIntRegister(GPIO_PORTF_BASE, pushbutton_handler); //Register SW1 interrupt handles
	GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE); //Setup interrupt for the individual pin
	GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_4); //Enable pin interrupt

	//----END MCU Initialization----

	//------Variable initialization------
	//Columns: R | G | B
	uint8_t rgb_LUT[8][3] = {
			{0, 0, 255},
			{0, 255, 0},
			{255, 0, 0},
			{255, 30, 217},
			{30, 222, 252},
			{240, 200, 40},
			{255, 123, 33},
			{255, 255, 255},
	};

	//Setup has been finished. Put MCU in sleep mode
	SysCtlSleep();

	//Main loop
	while (1) {
		if(pushFlag)
		{
			pushFlag = false; //Lower flag
			float dutyCycle = 0.0;

			//RED
			dutyCycle = (float) rgb_LUT[colorCounter][0] / 255;
			if(dutyCycle == 0)
				PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, false);
			else
				PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);

			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, pwmLoadValue * dutyCycle ); //RED //Requires extra config to work with output 1

			//GREEN
			dutyCycle = (float) rgb_LUT[colorCounter][1] / 255;
			if(dutyCycle == 0)
				PWMOutputState(PWM1_BASE, PWM_OUT_1_BIT, false);
			else
				PWMOutputState(PWM1_BASE, PWM_OUT_1_BIT, true);

			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_1, pwmLoadValue * dutyCycle ); //GREEN //Requires extra config to work with output 0

			//BLUE
			dutyCycle = (float) rgb_LUT[colorCounter][2] / 255;
			if(dutyCycle == 0)
				PWMOutputState(PWM1_BASE, PWM_OUT_2_BIT, false);
			else
				PWMOutputState(PWM1_BASE, PWM_OUT_2_BIT, true);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_2, pwmLoadValue * dutyCycle ); //BLUE

			//All LED configuration have been set. Put MCU in sleep mode.
			SysCtlSleep();
		}
	}
}

void pushbutton_handler(){
	GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);
	//Change color
	if( colorCounter + 1 == 8 ){
		colorCounter = 0;
	}
	else{
		colorCounter++;
	}
	//Signal main a push was made
	pushFlag = true;
}
