/*
 * main.c
 * Main program for TIVA MCU
 * Exercise 8.2.1 Generating voltages Using a DAC
 *
 * Station 13
 * Lab Team:
 * Emmanuel Ramos
 * Reynaldo Belfort
 */

//Pin configuration
//7  6  5  4  3  2  1  0
//D7,D6,D5,D4,D3,D2,D1,D0 	- port B GPIOs - Data Port
//RS,R/W,E,PUSHB       		- port A GPIOs - Control Port + LCD Module Push Button
//-,-,-,-,D7,D6,D5,D4		- port D - Second Half of DAC data pins MSB
//-,-,-,-,D3,D2,D1,D0		- port F - First Half of DAC data pins LSB
//-,-,-,-,-,PUSHB,Pot. (ADC),- port E - LCD Module Pull-up Push Button 1 + potentiometer ADC

//MCU objective #1: Change the current DAC value each 1 second, following the DAC table.
//				 The current DAC Value must be displayed on LCD
//MCU objective #2: Produce a sinusoidal wave with freq: 500Hz adn peak-to-peak voltage of 3.3V

//MCU objective #1 Pseudocode - Main program
//Initialize DAC Value table (array, hex values)
//<<Initialize LCD>>
//<<Initialize Timer, register ISR>>
//<<Initialize DAC pins>>
//<<Display LCD welcome message>>

//Main loop
	//<<Send dacTable[dac_counter] to DAC>>
	//<<Display current DAC Value>>
	//<<If dac_counter == 12>>
		//Reset dac_counter to 0

//<<Send dacTable[dac_counter]>>
	//Set dacTable[dac_counter][0] (4 LSB bits) on port F
	//Set dacTable[dac_counter][1] (4 MSB bits) on port D

//Pseudocode - Timer ISR
//Clear timer interrupt flag
//Increment dac_counter

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"

//Timer functionality
#include "inc/tm4c123gh6pm.h"
#include "driverlib/timer.h"

//Custom libraries
#include "MIL_LCD_lib.h"
#include "tivaUtils.h"

int dacTable[12][2] = { //TODO WORKING VERSION
		{0,0}, //0
		{1,7}, //23
		{2,14}, //46
		{4,5}, //69
		{5,12}, //92
		{7,3}, //115
		{8,10}, //138
		{10,1}, //161 //TODO Why doen't 0xA0 work??????
		{11,8}, //184
		{12,15}, //207
		{14,6}, //230
		{15,15}, //255
};

//Constant definition for LCD positions

//Constant definitions
//Timer
#define DESIRED_FRECUENCY 1  //1Hz - Desired timer count frequency
//GPIO
//#define MSB_DAC_PINS GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_4	//Remove if not needed
#define MSB_DAC_PINS GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0
#define LSB_DAC_PINS GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0

//Global variables
uint32_t timerPeriod;
uint32_t dac_counter;
int displayDecimalNumber;
//char lcdOutput[100];


int main(void) {
	//Set MCU to 40MHz
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	//--------LCD Setup--------
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);  // Enable, RS and R/W port for LCD Display
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);  // Data port for LCD display
	//Set LCD pins as outputs
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, ENTIRE_PORT);
	//-------------------------

	//--------Timer initialization--------

		SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
		//Configure Timer 0 as a 32-bit timer in periodic mode
		TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

		// Calculate number of clock cycles required at desired freq. Hz period by getting clock Hz and dividing it by our freq
		// then divide by two, since we want a count that is 1/2 of tthat for the interrupt
		timerPeriod = (SysCtlClockGet() / DESIRED_FRECUENCY); // Internal clock divided by desired frequency, in this case 1 ms
		// The calculated period is then loaded into the timer's Interval Load register(subtract 1, timer starts at 0)
		TimerLoadSet(TIMER0_BASE, TIMER_A, timerPeriod - 1);

		//Enable specific vector associated with interrupt for timer A
		IntEnable(INT_TIMER0A);
		//Enables specific event within the to generate an interrupt.  In this case generated on a timeout of Timer 0A
		TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
		//Enable master interrupt enable api for all interrupts
		IntMasterEnable();

//		//--------Initialize DAC pins--------
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
		//Important steps to be able to use pins PF0, PD3, PD2
		HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
		HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
		HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
		GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_OUT);
		GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
		GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LSB_DAC_PINS);	//LSBits DAC

		//Unlocking pin PD3
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
//		HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
//		HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= 0x08;
//		HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 8;
//		GPIODirModeSet(GPIO_PORTD_BASE, GPIO_PIN_3, GPIO_DIR_MODE_OUT);
//		GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
		//TODO Pins 1 & 0 are not working...!
		GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1); 	//MSBits DAC

	//--------------------------------------------------------

	//----Variable Initialization----
	dac_counter = 0;
	displayDecimalNumber = 0;

	//----LCD screen initialization----
	initializeLCD();

	//Start timer
	TimerEnable(TIMER0_BASE, TIMER_A);

	setCursorPosition(0x00);

	writeMessage("--- DAC ---", 11);
	setCursorPosition(0x40); //Jump to second line
	writeMessage("-- Interfacer --", 16);
	setDelay(2000); //2s delay
	clearLCD();


	while(true){

	}
} //End main


//Program functions
void TIMER_INTERRUPT_ISR(void){
	//Clear timer interrupt flag
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
//
//	//printf("LOG - displayDecimalNumber: %d \n", displayDecimalNumber);
//
	//Increment coutner
	if(dac_counter + 1 >= 12){ //Reset counter if needed
		dac_counter = 0;
	}
	else{
		dac_counter++;
	}
}
