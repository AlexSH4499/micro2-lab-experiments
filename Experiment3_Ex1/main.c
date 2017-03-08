/*
 * main.c
 * Main program for TIVA MCU - Experiment 3
 * Read a Key Using Interrupts, Hardware Debouncing and Software Debouncing exercises
 *
 * Lab Team:
 * Emmanuel Ramos
 * Reynaldo Belfort
 *
 * Pin setup
	 *PORT A:
	 * 2 - Pull up push button P2_OUT
	 * 5 - LCD: Enable
	 * 6 - LCD: R/W
	 * 7 - LCD: RS
	 *
	 *PORT B:
	 *	ENTIRE_PORT pins - LCD data pins
	 *
	 *PORT E:
	 * 5 - Push button with HARDWARE debouncing
	 *
* Notes:
* 	- Bounce time of push button: 5ms max. (from datasheet)
 */
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
//Include our libraries
#include "customLibs/MIL_LCD_lib.h"
#include "customLibs/tivaUtils.h"

//Global variables
uint8_t pushCounter = 0;
uint32_t pinStatus = 0;
uint8_t countStr[10] = "Counter = ";
bool pushFlag = false;

//Function declarations
void pushButtonPD_handler();

//----Software pseudocode----

//Set MCU clock to 40MHz (maybe not needed)

//Enable ports A, B, E
//Set PortA and Port B pins for LCD as output
//Set PB1 or PB3 pins as input (port A or E)
//Register ISR and enable interrupt for PB1 or PB3
//Initialize LCD with initial counter value
//LOOP: (main loop)
//if pushFlag = true
	//SetDelay(30ms)
	//Lower pushFlag
	//Increment counter or set to zero
	//Display counter value to LCD

//Interrupt handler ISR
//Retrieve interrupt flag status from PB1 or PB3
//Clear interrupt flag for PB1 or PB3
	//If PB1 or PB3 interrupt flag enabled
		//pushFlag = true

//TODO REMEMBER to write what are the global variables for this flowchart
//TODO Como mostarias numeros con mas de un digito???


int main(void) {
	
	//****MCU Initialization****
		//Set-up the clocking of the MCU to 40MHz
		SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

		//--Prepare ports for peripherals--
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
		//SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); //For HARDWARE debouncing push button

		//Set port output pins
		GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5); //LCD: Control pins
		GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, ENTIRE_PORT); // LCD: Data pins
		//Set port input pins
		GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_2); //Push button with pull up rest. PB1_OUT
		//GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_5);// HARDWARE debouncing push button

		//--Interrupt configuration--

		//--------For SOFTWARE debouncing--------
		//Establish interrupt handler for port A
		GPIOIntRegister(GPIO_PORTA_BASE, pushButtonPD_handler);
		//Establish the type of interrupt trigger for the pushbutton pin
		GPIOIntTypeSet(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_FALLING_EDGE);
		//Enable interrupt capability for pull-up push button
		GPIOIntEnable(GPIO_PORTA_BASE, GPIO_PIN_2);

		//--------For HARDWARE debouncing--------
//		//Establish interrupt handler for port E
//		GPIOIntRegister(GPIO_PORTE_BASE, pushButtonPD_handler);
//		//Establish the type of interrupt trigger for the pushbutton pin
//		GPIOIntTypeSet(GPIO_PORTE_BASE, GPIO_PIN_5, GPIO_FALLING_EDGE);
//		//Enable interrupt capability for pull-up push button
//		GPIOIntEnable(GPIO_PORTE_BASE, GPIO_PIN_5);

	//****END of MCU Initialization****

	//****LCD setup****
		initializeLCD();

		//Title message
		uint8_t initMsg[16] = "  Push counter! ";
		writeMessage(initMsg, 16);

		//Jump to second line and display intial count value
		setCursorPosition(0x40);
		writeMessage(countStr, 10);
		writeChar(pushCounter + 48);
	//****END LCD setup****

	//Main loop
	while(1){
		if(pushFlag){
			//SOFTWARE debouncing delay
			//SysCtlDelay(computeDelayCount(30, CLOCK_FREQ));
			//Lower flag
			pushFlag = false;

			//Increment counter or reset to zero
			if(pushCounter + 1 > 9){
				pushCounter = 0;
			}
			else{
				pushCounter++;
			}

			//----Write to LCD: Set cursor to second line and write to screen----
			setCursorPosition(0x40);
			writeMessage(countStr, 10); //"Counter = " string
			writeChar(pushCounter + 48); //0-9 characters are 48-57 in decimal
		}
	}
}

void pushButtonPD_handler(){

	//----SOFTWARE debouncing push button----
	//Clear interrupt flag
	pinStatus = GPIOIntStatus(GPIO_PORTA_BASE, true);
	GPIOIntClear(GPIO_PORTA_BASE, GPIO_PIN_2);

	//----HARDWARE debouncing push button----
	//Clear interrupt flag
//	pinStatus = GPIOIntStatus(GPIO_PORTE_BASE, true);
//	GPIOIntClear(GPIO_PORTE_BASE, GPIO_PIN_5);

	//Confirm that the interrput flag on pin was set
	if((pinStatus & GPIO_INT_PIN_2) == GPIO_INT_PIN_2){ //SOFTWARE
	//if((pinStatus & GPIO_INT_PIN_5) == GPIO_INT_PIN_5){ //HARDWARE
		//Signal main funciton that an interrupt has been served
		pushFlag = true; //Signal main to write on LCD
	}
}
