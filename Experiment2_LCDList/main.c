/*
 * main.c
 * Main program for TIVA MCU - Experiment 2
 * LCD Module Interfacing
 * Complementary Task Excercise
 *
 * Lab Team:
 * Emmanuel Ramos
 * Reynaldo Belfort
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
//Include our LCDlibrary
#include "customLibs/MIL_LCD_lib.h"

//Function definitions
uint8_t incCounter(uint8_t, uint8_t);
uint8_t decCounter(uint8_t, uint8_t);

int main(void) {

	//****MCU Initialization****
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); //Set-up the clocking of the MCU to 40MHz
	//--Prepare peripherals--
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	//Set port output pins
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, ENTIRE_PORT);
	//Set port input pins
	GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, (GPIO_PIN_2|GPIO_PIN_3));

	// Set Pin 3 to pull up
	GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_STRENGTH_8MA,GPIO_PIN_TYPE_STD_WPD);
	// Set pin 4 to pull down
	GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_STRENGTH_8MA,GPIO_PIN_TYPE_STD_WPU);

	//****END of MCU Initialization****

	//****Variable initialization****
	// Array Containing all the messeges to be displayed and their size
	uint8_t messages[16][2][16] = {
			{ {"List of Chords:"}, {15} },       // Message #1
			{ {"Gmaj7: G B D F#"}, {15} },				  // Message #2
			{ {"Dmin9: D F A C E"}, {16} },		  // Message #3
			{ {"Fmajor: F A C"}, {13} },				  // Message #4
			{ {"Bb7: Bb D F"}, {11} },	      // Message #5
			{ {"List of Scales:"}, {15} }, 			  // Message #6
			{ {"C major Scale:"}, {14} },		  // Message #7
			{ {"C D E F G A B"}, {13} },				  // Message #8
			{ {"A minor Scale:"}, {14} },	      // Message #9
			{ {"A B C D E F G"}, {13} },				  // Message #10
			{ {"C Blues Scale:"}, {14} },	  	  // Message #11
			{ {"C Eb F Gb G Bb C"}, {16} },				  // Message #12
			{ {"C Arabic Scale:"}, {15} },	      // Message #13
			{ {"C Db E F G Ab B"}, {15} },			      // Message #14
			{ {"F Mixolydian:"}, {13} },		  // Message #15
			{ {"F G A Bb C D Eb"}, {15} }				  // Message #16
	};
	// Counts current position in message array that has been written in display
	uint8_t msgCounterTop = 0;
	uint8_t msgCounterBot = 1;

	//Setup the LCD
	initializeLCD(); //Contains the writeCommand function
	clearLCD();

	//Show the first two lines
	writeMessage(messages[0][0], messages[0][1][0]);
	setCursorPosition(0x40); //Jump to second line
	writeMessage(messages[1][0], messages[1][1][0]);

	//Main loop
	while(1){
		// Pressed Pin 3 = PB 2 (Increment) ?
		if(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_3)){
			// Counter reached size of array limit?
			msgCounterTop = incCounter(msgCounterTop,16);
			//msgCounterBot = incCounter(msgCounterBot,16);
			// Clear LCD & write messages
			clearLCD();
			writeMessage(messages[msgCounterTop][0], messages[msgCounterTop][1][0]);
			setCursorPosition(0x40); //Jump to second line
			writeMessage(messages[msgCounterTop+1][0], messages[msgCounterTop+1][1][0]);

		}
		// Pressed Pin4 = PB 1 (Decrement) ?
		if(!(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_2))){
			msgCounterTop = decCounter(msgCounterTop,16);
			//msgCounterBot = decCounter(msgCounterBot,16);

			// Clear LCD & write messages
			clearLCD();
			writeMessage(messages[msgCounterTop][0], messages[msgCounterTop][1][0]);
			setCursorPosition(0x40); //Jump to second line
			writeMessage(messages[msgCounterTop+1][0], messages[msgCounterTop+1][1][0]);
		}

		SysCtlDelay(60000); //Delay
	} //End main loopC:\Users\s402120683\Downloads\Micro 2 - Docs & Files
}

// increases counter depending on array size limit
uint8_t incCounter(uint8_t counter, uint8_t size){
	if(counter == size-2){
		counter = 0;
	}else {
		counter = counter + 2;
	}
	return counter;
}
// decrements counter depending on array size limit
uint8_t decCounter(uint8_t counter, uint8_t size){
	if(counter == 0){
		counter = size -2;
	} else{
		counter = counter - 2;
	}
	return counter;
}
