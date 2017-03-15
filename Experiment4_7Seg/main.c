#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
//Our libraries
#include "createdLibs/7seg-utils.h"
#include "tivaUtils.h"

uint8_t counter = 0;

int main(void){

	//---------------MCU Initialization---------------

	//Set clock to 40MHz
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	//Enable Port A
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	//Port as output
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, ENTIRE_PORT);

	//Set pins to 2mA
	GPIOPadConfigSet(GPIO_PORTB_BASE, ENTIRE_PORT, GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD);

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


	//Main loop
	while(1){

		//Display current value in LCD
		GPIOPinWrite(GPIO_PORTB_BASE, ENTIRE_PORT, _sevenSegLookup[counter]); //Display #4
		if(counter + 1 == 16) {counter = 0; } else { counter++; }
		SysCtlDelay(computeDelayCount(800, CLOCK_FREQ)); //Stand-by
	}
}
