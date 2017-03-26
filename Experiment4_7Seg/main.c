#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

//For timer functionality
#include "inc/tm4c123gh6pm.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"

//Our libraries
#include "createdLibs/7seg-utils.h"
#include "tivaUtils.h"

//--------------Pseudocode exercise 4.2.3--------------

// Set MCU clock to 40MHz
// Enable Port B
// Set Port B pins as output with 2mA strength
// Setup timer
// Initialize counter and lookUpTable

//MAIN_LOOP:
	//Write to 7seg value: lookUpTable[counter]
	//GOTO MAIN_LOOP

//Timer ISR
//Clear Timer flag
//IF counter + 1 = 16
	  //counter = 0
//ELSE
	  //counter = counter + 1

//--------------Pseudocode exercise 4.2.4--------------

// Set MCU clock to 40MHz
// Enable Port B
// Set Port B pins as output with 2mA strength
// Setup timer
// Initialize counter and lookUpTable

//MAIN_LOOP:
	//Turn off MSB_SEG & LSB_SEG
	//Write to MSB_SEG value: lookUpTable[msbCounter]
	//Delay TODO set secods
	//Turn off MSB_SEG
	//Write to MSB_SEG value: lookUpTable[lsbCounter]
	//Delay TODO set secods

	//GOTO MAIN_LOOP

//Timer ISR
//Clear Timer flag
//IF lsbCounter + 1 == 16
	  //lsbCounter = 0
	  //IF msbCounter + 1 == 16
			//msbCounter = 0
	  //ELSE
			//msbCounter = msbCounter + 1
//ELSE
	  //lsbCounter = lsbCounter + 1

#define DESIRED_FRECUENCY 1 //Desired timer count frequency

//Turning segments with a logic 0 according to the module's datasheet
#define DISPLAY_OFF 0xFF
#define MSB_SEGMENT_ON 0xBF
#define LSB_SEGMENT_ON 0x7F

//#define DISPLAY_OFF 0x00
//#define MSB_SEGMENT_ON 0x40
//#define LSB_SEGMENT_ON 0x80

#define DISPLAY_REFRESH_RATE 9 //In miliseconds  Tperiod - 17 or halfperiod - 9ms

uint8_t digitCounter = 0;
uint32_t timerPeriod = 0;
uint8_t msbCounter = 0;
uint8_t lsbCounter = 0;

int main(void){

	//---------------MCU Initialization---------------

	//Set clock to 40MHz
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	//Enable Port B
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	//Port as output
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_6|GPIO_PIN_7); //Segment enable pins
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, ENTIRE_PORT); //Segment pins

	//Set pins to 2mA
	//GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_6|GPIO_PIN_7, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
	GPIOPadConfigSet(GPIO_PORTB_BASE, ENTIRE_PORT, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);

	//-----------Setup Timer-----------
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	//Setup timer period:
	//--Desired frecuency for 7seg counter: 2Hz
	timerPeriod = (SysCtlClockGet() / DESIRED_FRECUENCY); //50% Duty cycle
	TimerLoadSet(TIMER0_BASE, TIMER_A, timerPeriod);

	//Enable timer interrupt TODO Add comments to individual code
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();

	//Start timer
	TimerEnable(TIMER0_BASE, TIMER_A);

	//---------------END of MCU Initialization-------------

	uint8_t _sevenSegLookup[16] = {
			ZERO,
			ONE,
			TWO,
			THREE,
			FOUR,
			FIVE,
			SIX,
			SEVEN,
			EIGHT,
			NINE,
			LETTER_A,
			LETTER_B,
			LETTER_C,
			LETTER_D,
			LETTER_E,
			LETTER_F
	};

	//-------------------------SINGLE SEGMENT EXERCISE-------------------------
//	//Main loop
//	while(1){
//		//Display current value in the 7 segment
//		GPIOPinWrite(GPIO_PORTB_BASE, ENTIRE_PORT, _sevenSegLookup[digitCounter]);
//	}

	//-------------------------DOUBLE SEGMENT EXERCISE-------------------------

	//Main loop
	while(1){

		//Turn off MSB_SEG & LSB_SEG
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6|GPIO_PIN_7, DISPLAY_OFF);

		//Obtain most significant bit value
		GPIOPinWrite(GPIO_PORTB_BASE, ENTIRE_PORT, _sevenSegLookup[msbCounter]);

		//-------Show value on screen-------
		//Turn on MSB_SEG
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6|GPIO_PIN_7, MSB_SEGMENT_ON);
		SysCtlDelay(computeDelayCount(DISPLAY_REFRESH_RATE, CLOCK_FREQ)); //Stand-by

		//Turn off MSB_SEG & LSB_SEG
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6|GPIO_PIN_7, DISPLAY_OFF);

		//Obtain least significant bit value
		GPIOPinWrite(GPIO_PORTB_BASE, ENTIRE_PORT, _sevenSegLookup[lsbCounter]);

		//-------Show value on screen-------
		//Turn on LSB_SEG
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6|GPIO_PIN_7, LSB_SEGMENT_ON);
		SysCtlDelay(computeDelayCount(DISPLAY_REFRESH_RATE, CLOCK_FREQ)); //Stand-by
	}
}

//SINGLE SEGMENT EXERCISE
//Pleas refer to the tm4c123gh6pm_startup_ccs.c to find the registration of this function in the MCU vector table
//void Timer0_handler(){
//	//Clear timer interrupt flag
//	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
//	//Increment counter
//	if(digitCounter + 1 == 10) {digitCounter = 0; } else { digitCounter++; } //0 to 9 count.
//}

//DUAL SEGMENT EXERCISE
//Pleas refer to the tm4c123gh6pm_startup_ccs.c to find the registration of this function in the MCU vector table
void Timer0_handler(){
	//Clear timer interrupt flag
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	//Increment digit counters
	if(lsbCounter + 1 == 16) //0 to 15 count.
	{
		lsbCounter = 0;
		if(msbCounter + 1 == 16) { msbCounter = 0; } else { msbCounter++; } //Increment msbCoutner
	}
	else //Increment lsbCoutner
	{
		lsbCounter++;
	}
}
