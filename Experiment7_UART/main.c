/*
 * main.c
 * Main program for TIVA MCU - Experiment 7.2.1
 * Asynchronous Serial Communication (UART)
 *
 * Station 13
 * Lab Team:
 * Emmanuel Ramos
 * Reynaldo Belfort
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"

//#include "tivaUtils.h"


//Main subroutine

//Initialize Clock
//Intitialize MCU UART Pins
//---Configue MCU UART---
	//Set baud rate
	//Configure UART

//LOOP:
	//if UART empty
		//Set character 'A' (ASCII) transmission buffer

uint32_t computeDelayCount(uint32_t waitTime, uint8_t clockFreq);


int main (void){
	//----MCU Initialization----
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); //Set-up the clocking of the MCU to 40MHz

	//----UART Initialization----
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

	//Configure pins to be used as UART Rx & TX: Pins 6 & 7 from port c
	GPIOPinConfigure(GPIO_PC6_U3RX);
	GPIOPinConfigure(GPIO_PC7_U3TX);
	GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);

	UARTConfigSetExpClk(UART3_BASE, SysCtlClockGet(), 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE ));
	//---------------------------

	sendMessage("Hello Word!", 11);

	while(1){
		//stand by
		//UARTCharPut(UART3_BASE, 'A');
		//setDelay(500); //0.5s
	//	SysCtlDelay(computeDelayCount(500, 40)); //500ms
	}

}

void sendMessage(uint8_t strVal[], uint8_t arrSize){
	//Validation
	if(arrSize <= 0) { return; }

	uint8_t i = 0;
	for(; i < arrSize; i++){
		UARTCharPut(UART3_BASE,strVal[i]);
	}
}
uint32_t computeDelayCount(uint32_t waitTime, uint8_t clockFreq){
	return 0.3333333 * (clockFreq*1000000) * (waitTime * 0.001);
}

