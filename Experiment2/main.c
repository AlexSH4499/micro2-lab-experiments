#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

/*
 * main.c
 * Main program for TIVA MCU - Experiment 2
 * Blinking LED
 *
 * Team:
 * Emmanuel Ramos
 * Reynaldo Belfort
 */
int main(void) {
	
	//--------Initialization--------
//	uint8_t toggleValue = 128; //1000000b value
	uint8_t toggleValue = 4;
	//Set-up the clocking of the MCU to 40MHz
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	//--------Prepare peripheral--------
	//Enable the peripheral port F
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	//Set pin 1 as output  of the port
//	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_7);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
	//Set pin 1 current
//	GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);

	//--------END of Initialization--------

	//Main loop
	while(1){
		//Write output to pin
//		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, toggleValue);
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, toggleValue);
		//Wait 100ms at 40MHz clock frecuency
		SysCtlDelay(1333333);
		//Alternate toggle value
		toggleValue = toggleValue == 4 ? 0 : 4;
	}
}


//int main(void) {
//
//	//Initialization
//	uint8_t toggleValue = 128; //1000000b value
//	//SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); //Set-up the clocking of the MCU to 40MHz
//	SysCtlClockSet(SYSCTL_SYSDIV_8|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); //Set-up the clocking of the MCU to 25MHz
//	//Prepare peripheral
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); //Enable the peripheral port F
//	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_7); //Set pin 1 as output  of the port
//
//	GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD); //Set pin 1 current
//	//Main loop
//	while(1){
//		//Write output to pin
//		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, toggleValue);
//
//		//SysCtlDelay(1333333); //Wait 100ms at 40MHz clock frecuency
//		SysCtlDelay(4166667); //Wait 500ms at 25MHz
//		//SysCtlDelay(833334); //Wait 100ms at 25MHz
//		//SysCtlDelay(2000000); //At 40MHz (0.000025ms/cycle), 0.000075ms per 3 instruction cycles on delay function. Total delay time: 150ms
//		//SysCtlDelay(6650000); //At 40MHz (0.000025ms/cycle), 0.000075ms per 3 instruction cycles on delay function. Total delay time: 498.75ms
//		//SysCtlDelay(13300000); //Total delay time: 997.5ms
//		//Alternate toggle value
//		toggleValue = toggleValue == 128 ? 0 : 128;
//	}
//}
