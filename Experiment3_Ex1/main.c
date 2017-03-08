/*
 * main.c
 * Main program for TIVA MCU - Experiment 3
 * Read a Key Using Interrupts
 * Exercise 3.2.1
 *
 * Lab Team:
 * Emmanuel Ramos
 * Reynaldo Belfort
 *
 * TODO Pin setup
 *PORT A:
 * 2 - Pull up push button P2_OUT
 * 3 - Pull down push button
 * 5 - LCD: Enable
 * 6 - LCD: R/W
 * 7 - LCD: RS
 *
 *PORT B:
 *	ENTIRE_PORT pins - LCD data pins
 *
 *
 *	//Implement initial approach
 *	//Make arrangements or optimize
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
//Include our LCDlibrary
#include "customLibs/MIL_LCD_lib.h"
#include "customLibs/tivaUtils.h"

//Global variables
uint8_t pushCounter = 0;
uint32_t pinStatus = 0;
uint8_t outputStr[10] = "Counter = ";
bool countUpdated = false;

//Function declarations
//uint32_t computeDelayCount(uint8_t, uint8_t);
void pushButtonPD_handler();

int main(void) {
	
	//****MCU Initialization****

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); //Set-up the clocking of the MCU to 40MHz
	//--Prepare ports for peripherals--
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); //For switch with hardware debouncing

	//Set port output pins
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5); //LCD: Control pins
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, ENTIRE_PORT); // LCD: Data pins
	//Set port input pins
	GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, (GPIO_PIN_2|GPIO_PIN_3)); //Push buttons 2 - pull up PB1_OUT, 3 - pull down

	// Set Pin 2 current to pull up push button
	GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_STRENGTH_4MA,GPIO_PIN_TYPE_STD); //TODO If push button is pull up (comments) why we set a pull down resistance here??
	// Set Pin 3 current to pull down push button
	GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_STRENGTH_4MA,GPIO_PIN_TYPE_STD);

	//--Interrupt configuration--
	//Establish interrupt handler for port A
	GPIOIntRegister(GPIO_PORTA_BASE, pushButtonPD_handler);
	//Establish the type of interrupt trigger for the pushbutton pin
	GPIOIntTypeSet(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_FALLING_EDGE);
	//Enable interrupt capability for pull-up push button
	GPIOIntEnable(GPIO_PORTA_BASE, GPIO_PIN_2);

	//****END of MCU Initialization****

	//----LCD setup----
	initializeLCD();

	//Temporary message
	uint8_t initMsg[16] = "LCD initialized!";
	writeMessage(initMsg, 16);

	//Jump to second line and display intial count value
	setCursorPosition(0x40);
	writeMessage(outputStr, 10);
	writeChar(pushCounter + 48);

	while(1){
		if(countUpdated){
			countUpdated = false;
			//Set cursor to second line
			setCursorPosition(0x40);
			//Write to LCD
			writeMessage(outputStr, 10);
			writeChar(pushCounter + 48); //0-9 characters are 48-57
		}
		//Wait before we start reading countDelay again
		//SysCtlDelay(computeDelayCount(20, CLOCK_FREQ)); //Bounce time of push button: 5ms max. (from datasheet)
	}
}

void pushButtonPD_handler(){
	//In this program, we only have one interrupt enabled, so we don't need to check the flag, just clear it
	//Retrieve port pin status
	//pinStatus = GPIOIntStatus(GPIO_PORTA_BASE, true); //TODO Why argument is set to true?

	//Clear interrupt flag for pushbutton pin
	GPIOIntClear(GPIO_PORTA_BASE, GPIO_PIN_2);

	//Disable interrupt on pull-up push button
	GPIOIntDisable(GPIO_PORTA_BASE, GPIO_PIN_2);

	//if((pinStatus & GPIO_INT_PIN_2) == GPIO_INT_PIN_2){
		//Push button was pressed. Proceed to increment counter

		if(pushCounter + 1 > 9){
			pushCounter = 0;
		}
		else{
			pushCounter++;
		}
		//Signal to write on LCD
		countUpdated = true;
	//}

	//Doing our best to set a delay longer than the bounce time to avoid interrupt issues
	//At the same time should not be too large to mantain good UI behavior
	SysCtlDelay(computeDelayCount(20, CLOCK_FREQ));

	//Enable interrupt on pull-up push button
	GPIOIntEnable(GPIO_PORTA_BASE, GPIO_PIN_2);
}


