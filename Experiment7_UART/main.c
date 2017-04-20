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
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"

#include "tivaUtils.h"
#include "MIL_LCD_lib.h"


//Main subroutine

//Initialize Clock
//Intitialize MCU UART Pins
//---Configue MCU UART---
	//Set baud rate
	//Configure UART

//LOOP:
	//if UART empty
		//Set character 'A' (ASCII) transmission buffer

//Global variables
uint32_t cursorPosition = 0x40;
uint8_t receivedChars[16];
uint8_t counter = 0;

//uint32_t computeDelayCount(uint32_t waitTime, uint8_t clockFreq);
void sendMessage(uint8_t strVal[], uint8_t arrSize);


int main (void){
	//----MCU Initialization----
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); //Set-up the clocking of the MCU to 40MHz

	//----LCD Setup----
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);  // Enable, RS and R/W port for LCD Display
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);  // Data port for LCD display
	//Set LCD pins as outputs
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, ENTIRE_PORT);
	//-----------------

	//----UART Initialization----
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

	//Configure pins to be used as UART Rx & TX: Pins 6 & 7 from port c
	GPIOPinConfigure(GPIO_PC6_U3RX);
	GPIOPinConfigure(GPIO_PC7_U3TX);
	GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);

	UARTConfigSetExpClk(UART3_BASE, SysCtlClockGet(), 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE ));

	//Enabling interrupts for UART
	IntMasterEnable();
	IntEnable(INT_UART3);
	UARTIntEnable(UART3_BASE, UART_INT_RX | UART_INT_RT);

	//---------------------------

	//Variable initialization
	uint8_t i = 0;

	for(;i < 16; i++){
		receivedChars[i] = 0;
	}

	initializeLCD();
	//Write initial messages
	writeMessage("---UART Link----", 16);
	setCursorPosition(0x40); //Jump to second line
	writeMessage("Ready to receive", 16);

	setDelay(1000); //1sec
	clearLCD();
	setCursorPosition(0); //Jump to second line
	writeMessage("---Message---", 13);
	setCursorPosition(0x40); //Jump to second line

	//sendMessage("Hello Word!", 11);

	while(1){
		//stand by
		//EXERCISE #1 UART via pollingH
		//UARTCharPut(UART3_BASE, 'A');
		//setDelay(500); //0.5s
	//	SysCtlDelay(computeDelayCount(500, 40)); //500ms

		//TODO Sample code
		//if (UARTCharsAvail(UART0_BASE))
			//UARTCharPut(UART0_BASE, UARTCharGet(UART0_BASE));
	}

}


void UARTIntHandler(void)
{
	uint32_t status;
	status = UARTIntStatus(UART3_BASE, true); //GEt interrupt status
	//Clear interrupt flags
	UARTIntClear(UART3_BASE, status);
//	//Clear screen in order to display new message
//	clearLCD();
//	setCursorPosition(0x40);

	while(UARTCharsAvail(UART3_BASE)) //Keep looping while there are characters
	{
		//UARTCharPutNonBlocking (UART3_BASE, UARTCharGetNonBlocking(UART3_BASE));
		//writeChar(UARTCharGetNonBlocking(UART3_BASE));

		//Verify if we have to reset the cursor on second line LCD.
		//Note: 4F = 79 decimal
//		if(cursorPosition + 1 > 0x4F){
//			cursorPosition = 0x40;
//			setCursorPosition(cursorPosition);
//		}
//		else{
//			cursorPosition++;
//		}


		//Store receuved char on buffer
		//If char buffer is 16
			//display on lcd
			//respond to PC
		//else
			//Store char on bufferVAr under counte index

		receivedChars[counter] = UARTCharGetNonBlocking(UART3_BASE);
		setCursorPosition(0);
		writeMessage("Chars recv: ", 12);
		writeChar(counter + 1 + 48); //Add 1 considering that counter starts from 0


		//Verify if we have to reset the cursor on second line LCD.
		//Note: 4F = 79 decimal
		if(counter == 15){
			counter = 0;
			setCursorPosition(0x40);
			writeMessage(receivedChars, 16);
			//---------Respond back to PC with message in upper case----------

			//Conver to uppercase by toggling 6th bit for each char
			uint8_t i = 0;
			for(;i < 16; i++){
				receivedChars[i] = receivedChars[i] ^ 0x20; //20 hex = 000100000b
			}

			i = 0;
			for(;i < 16; i++){
				UARTCharPutNonBlocking(UART3_BASE, receivedChars[i]);
			}

		}
		else{
			counter++;
		}
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
//uint32_t computeDelayCount(uint32_t waitTime, uint8_t clockFreq){
//	return 0.3333333 * (clockFreq*1000000) * (waitTime * 0.001);
//}
