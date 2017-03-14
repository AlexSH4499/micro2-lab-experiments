///*
// * main.c
// * Main program for TIVA MCU - Experiment 3
// * Read a Key Using Interrupts
// * Exercise 3.2.1
// *
// * Lab Team:
// * Emmanuel Ramos
// * Reynaldo Belfort
// *
// * TODO Pin setup
// *PORT A:
// * 2 - Pull up push button P2_OUT
// * 3 - Pull down push button
// * 5 - LCD: Enable
// * 6 - LCD: R/W
// * 7 - LCD: RS
// *
// *PORT B:
// *	ENTIRE_PORT pins - LCD data pins
// *
// *
// *	//Implement initial approach
// *	//Make arrangements or optimize
// */
//
//#include <stdint.h>
//#include <stdbool.h>
//#include "inc/hw_types.h"
//#include "inc/hw_memmap.h"
//#include "driverlib/sysctl.h"
//#include "driverlib/gpio.h"
////Include our LCDlibrary
//#include "customLibs/MIL_LCD_lib.h"
//#include "customLibs/tivaUtils.h"
//
////Global variables
//uint8_t pushCounter = 0;
//uint32_t pinStatus = 0;
//uint8_t countStr[10] = "Counter = ";
//bool pushFlag = false;
//
////Function declarations
////uint32_t computeDelayCount(uint8_t, uint8_t);
//void pushButtonPD_handler();
//
//int main(void) {
//
//	//****MCU Initialization****
//
//		SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); //Set-up the clocking of the MCU to 40MHz
//		//--Prepare ports for peripherals--
//		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
//		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
//		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); // HARDWARE debouncing push button
//
//		//Set port output pins
//		GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5); //LCD: Control pins
//		GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, ENTIRE_PORT); // LCD: Data pins
//		//Set port input pins
//		//TODO Bounce time of push button: 5ms max. (from datasheet)
//		GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, (GPIO_PIN_2)); //Push button with pull up rest. PB1_OUT
//		//GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_5);// HARDWARE debouncing push button
//
//		//--Interrupt configuration--
//		//--------For SOFTWARE debouncing--------
//		//Establish interrupt handler for port A
//		GPIOIntRegister(GPIO_PORTA_BASE, pushButtonPD_handler);
//		//Establish the type of interrupt trigger for the pushbutton pin
//		GPIOIntTypeSet(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_FALLING_EDGE);
//		//Enable interrupt capability for pull-up push button
//		GPIOIntEnable(GPIO_PORTA_BASE, GPIO_PIN_2);
//
//		//--------For HARDWARE debouncing--------
//	//	//Establish interrupt handler for port E
//	//	GPIOIntRegister(GPIO_PORTE_BASE, pushButtonPD_handler);
//	//	//Establish the type of interrupt trigger for the pushbutton pin
//	//	GPIOIntTypeSet(GPIO_PORTE_BASE, GPIO_PIN_5, GPIO_FALLING_EDGE);
//	//	//Enable interrupt capability for pull-up push button
//	//	GPIOIntEnable(GPIO_PORTE_BASE, GPIO_PIN_5);
//
//	//****END of MCU Initialization****
//
//	//****LCD setup****
//		initializeLCD();
//
//		//Title message
//		uint8_t initMsg[16] = "  Push counter! ";
//		writeMessage(initMsg, 16);
//
//		//Jump to second line and display intial count value
//		setCursorPosition(0x40);
//		writeMessage(countStr, 10);
//		writeChar(pushCounter + 48);
//	//****END LCD setup****
//
//	//Main loop
//	while(1){
//		if(pushFlag){
//			//SOFTWARE debouncing delay
//			//SysCtlDelay(computeDelayCount(30, CLOCK_FREQ));
//			//Lower flag
//			pushFlag = false;
//
//			//Increment counter or reset to zero
//			if(pushCounter + 1 > 9){
//				pushCounter = 0;
//			}
//			else{
//				pushCounter++;
//			}
//
//			//----Write to LCD: Set cursor to second line and write to screen----
//			setCursorPosition(0x40);
//			writeMessage(countStr, 10); //"Counter = " string
//			writeChar(pushCounter + 48); //0-9 characters are 48-57 in decimal
//		}
//	}
//}
//
//void pushButtonPD_handler(){
//
//	//----SOFTWARE debouncing push button----
//	//Clear interrupt flag
//	GPIOIntClear(GPIO_PORTA_BASE, GPIO_PIN_2);
//
//	//----HARDWARE debouncing push button----
//	//Clear interrupt flag
//	//GPIOIntClear(GPIO_PORTE_BASE, GPIO_PIN_5);
//
//				//Increment counter
//			//	if(pushCounter + 1 > 9){
//			//		pushCounter = 0;
//			//	}
//			//	else{
//			//		pushCounter++;
//			//	}
//			//	pushFlag = true; //Signal main to write on LCD
//
//	//Signal main that an interrupt has been served
//	pushFlag = true;
//}
//
////-----------------------OLD CODE-----------------------
//
////void pushButtonPD_handler(){
////
////	//Clear interrupt flag for pushbutton pin
////	//----SOFTWARE debouncing push button----
////	//Retrieve port pin status
////	pinStatus = GPIOIntStatus(GPIO_PORTA_BASE, true); //TODO Why argument is set to true?
////	SysCtlDelay(computeDelayCount(30, CLOCK_FREQ));
////	GPIOIntClear(GPIO_PORTA_BASE, GPIO_PIN_2);
////
////	//----HARDWARE debouncing push button----
////	//pinStatus = GPIOIntStatus(GPIO_PORTE_BASE, true); //TODO Why argument is set to true?
////	//GPIOIntClear(GPIO_PORTE_BASE, GPIO_PIN_5);
////
////	//TODO remove if not needed
////	//Disable interrupt on pull-up push button
////	//GPIOIntDisable(GPIO_PORTA_BASE, GPIO_PIN_2);
////
////	//Confirm that the interrput flag on pin was set
////	if((pinStatus & GPIO_INT_PIN_2) == GPIO_INT_PIN_2){ //SOFTWARE
////	//if((pinStatus & GPIO_INT_PIN_5) == GPIO_INT_PIN_5){ //HARDWARE
////
////		//Proceed to increment counter
////		if(pushCounter + 1 > 9){
////			pushCounter = 0;
////		}
////		else{
////			pushCounter++;
////		}
////		pushFlag = true; //Signal main to write on LCD
////	}
////
////	//TODO remove if not needed
////	//Enable interrupt on pull-up push button
////	//GPIOIntEnable(GPIO_PORTA_BASE, GPIO_PIN_2);
////}
//
//
