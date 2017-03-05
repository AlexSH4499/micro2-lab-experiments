#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include <stdio.h>
#include <stdlib.h>

//Pin data for Pin 2
uint8_t ui8Pin0Data = 0;
//Pin data for Pin 1
uint8_t ui8Pin1Data = 2;

//   LAB2


int main(void){
	// Set System Clock to 40Mhz
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	// Enable Port B
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	// Enable Port F
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	// Set Port F Pin 1 & 2 for Output
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, (GPIO_PIN_2|GPIO_PIN_1)); //Pin 1: LED Pull-up, Pin 2: LED Pull-down
	// Set PORTB Pin 2 & 3 for Input
	GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, (GPIO_PIN_2|GPIO_PIN_3)); //Pin 2: Push Pull Down, Pin 3: Push Pull Up
	// Set PortB Pin 2 & 3
	GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_STRENGTH_8MA,GPIO_PIN_TYPE_STD_WPD);
	GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_STRENGTH_8MA,GPIO_PIN_TYPE_STD_WPU);

	while (1){
		// Set & Write Pin data for Pin 1 to OFF (Pullup High=Off Low=On)
		//ui8Pin1Data = 2;
		//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, ui8Pin1Data); //Turn LED OFF
		// while Pin 4 is pressed
		//while(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_2) == 0){
		if(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_2) == 0){

			// Turn Led in Pin 0 On
			//ui8Pin0Data = 4;
			ui8Pin0Data = ui8Pin0Data == 4 ? 0 : 4;
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, ui8Pin0Data);
			SysCtlDelay(0.3333333 * (40*1000000) * (150 * 0.001) );
		}
		//Turn Led in Pin 0 Off
		//ui8Pin0Data = 0;
		//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, ui8Pin0Data);
		// while Pin 3 is pressed
		//while(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_3) == 8){
		if(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_3) == 8){
			//SysCtlDelay(0.3333333 * (40*1000000) * (50 * 0.001) );
			// Turn Led in Pin 3 On (Pullup High=Off Low=On)
			//ui8Pin1Data = 0;
			ui8Pin1Data = ui8Pin1Data == 2 ? 0 : 2;
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, ui8Pin1Data);
			SysCtlDelay(0.3333333 * (40*1000000) * (150 * 0.001) );
		}



	}
}


