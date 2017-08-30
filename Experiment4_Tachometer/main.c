/*
 * main.c
 * Main program for TIVA MCU - Experiment 4
 * Digital Tachometer
 * Complementary Task Excercise
 *
 * Lab Team:
 * Emmanuel Ramos
 * Reynaldo Belfort
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "customLibs/MIL_LCD_lib.h"

/*
 *
 * The code assumes the following pin configuration (7-0 bits)
 * 7  6  5  4  3  2  1  0
 * D7,D6,D5,D4,D3,D2,D1,D0 - port B GPIOs - Data Port
 * RS,R/W,E 		           - port A GPIOs - Control Port
 */

 /* TODO Falta calcular:
    - Los signal delays (regla de tres)
    - Comentar todas las funciones
    - Entender bien COMMAND_ENTRYMODE_INCCUR_NODISP_SHIFT, sobre todo EL NODISPLAY option, que hace.
 */

int msgCounterTop; // Current index for top line on LCD
int msgCounterBot; // current index for bottom line on LCD
uint32_t signal = 0; //tells us witch pin interrupted the program
//uint32_t index; // Index for holding the
int LUT[16] = {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0};  //Look up table for knowing disk direction sequence
int indexLUT; // index that points to the lookup table after the 4bit combination of oldA oldB newA newB
int oldA, oldB, newA, newB = 0;  // will hold the states of each sensor for calculating the 4-bit comibination index
								 //oldA (MSb), oldB, newA, newB(LSb)
int intStep = 0; // 0 is no interrupt, if 1 is first interrupt step , if 2  is second interrupt step
int millisCounterInit = 0; // holds milliseconds passed
int millisCounterFinal = 0; // temporary for holding actual millis that passed

uint32_t ui32Period;  // freq

//Function definitions
void PB1_IntHandler();  // interrupt handler
int incCounter(int counter, int size);
int decCounter(int counter, int size);

int main(void) {

	//----MCU Initialization----
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); //Set-up the clocking of the MCU to 40MHz
	//--Prepare peripherals--
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); // Port for RS,R/W, E pins of LCD
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); // PORT for LCD data pins
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); // Input Port for columns


	//Set port pins as outputs
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5); // RS,R/W, E pins of LCD
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, ENTIRE_PORT); // Data pins of LCD
	// Set port pins as inputsPIN 3 = Inner Sensor, PIN 2 = Outer Sensor
	GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_3|GPIO_PIN_2);
	// Set Pin 2 for outer sensorto pull down
	GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_2, GPIO_STRENGTH_4MA,GPIO_PIN_TYPE_STD_WPD);
	// Set pin 3 for inner sensor to pull down
	GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_3, GPIO_STRENGTH_4MA,GPIO_PIN_TYPE_STD_WPD);

	// timer Inicialization
	//Enable the clock to the timer
		SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
		//Configure Timer 0 as a 32-bit timer in periodic mode
		TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

		// calculate number of clock cycles required at desired freq. Hz period by getting clock Hz and dividing it by our freq
		// then divide by two, since we want a count that is 1/2 of tthat for the interrupt
		ui32Period = (SysCtlClockGet () / 1000); // Internal clock divided by desired frequency, in this case 1 ms
		// the calculated period is then loaded into the timer's Interval Load register(subtract 1, timer starts at 0)
		TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);

		//Enable specific vector associated with interrupt for timer A
		IntEnable(INT_TIMER0A);
		// Enables specific event within the to generate an interrupt.  In this case generated on a timeout of Timer 0A
		TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
		//enable master interrupt enable api for all interrupts
		IntMasterEnable();


	//----END of MCU Initialization----

	//Setup the LCD
	initializeLCD();


	// Array Containing all the messeges to be displayed and their size
	uint8_t msgDisplayTop[3][2][16] = {
			{ {"Speed="}, {6} },
			{ {"0000"}, {4} },
			{ {"RPM"}, {3} }
	};
	//
	uint8_t msgDisplayBot[3][2][16] = {
			{ {"Direction:----"}, {14} },
			{ {"Direction: CW "}, {14} },
			{ {"Direction: CCW"}, {14} }

	};

    // Counts current position in message array that has been written in display
    int msgCounterTop = 0;
    int msgCounterBot = 1;
    clearLCD();
    // write first & second message
    writeMessage(msgDisplayTop[0][0], msgDisplayTop[0][1][0]);
    writeMessage(msgDisplayTop[1][0], msgDisplayTop[1][1][0]);
    writeMessage(msgDisplayTop[2][0], msgDisplayTop[2][1][0]);
    setCursorPosition(0x40); //Jump to second line
    writeMessage(msgDisplayBot[0][0], msgDisplayBot[0][1][0]);
	GPIOIntRegister(GPIO_PORTE_BASE, PB1_IntHandler);
	//Sets the interrupt type for the specified pin(s). GPIO_BOTH_EDGES to get both changes CW and CCW
	GPIOIntTypeSet(GPIO_PORTE_BASE, GPIO_PIN_3|GPIO_PIN_2, GPIO_BOTH_EDGES);
	 // Enable interrupt for PE.2 & PE.3
	GPIOIntEnable(GPIO_PORTE_BASE, GPIO_PIN_3|GPIO_PIN_2);
	//Main loop
	while(1){

		// Interrupted by disk?
			if(signal !=0){
				if(intStep == 1  ){
					//Enable the timer.  Start the timer and interrupts will begin triggering on the timeouts
					TimerEnable(TIMER0_BASE, TIMER_A);
					//while(intStep == 1){};
				}else if(intStep == 2){

					//Disable the timer.
					TimerDisable(TIMER0_BASE, TIMER_A);
					//time /1000/60 = ms/1000ms = 1s/ 60s = 1min
					int rpms = 60000/((millisCounterFinal) * 24);  // obtain rpm by multipling tedge by rotational wheel steps
					intStep = 0; // reset Step int flag
					uint8_t strRPMS[4];
					// calculate string rpms
					strRPMS[0] = rpms/1000;
					strRPMS[1] = rpms/100 - (strRPMS[0] * 10);
					strRPMS[2] = rpms/10 - (strRPMS[0] *100) - (strRPMS[1] * 10);
					strRPMS[3] = rpms - (strRPMS[0] *1000) - (strRPMS[1] * 100) - (strRPMS[2]* 10);

					//clearLCD();

					//writeMessage(msgDisplayTop[0][0], msgDisplayTop[0][1][0]); // writes Speed=
					setCursorPosition(0x06);
					writeChar(strRPMS[0] + 48);  // Writes Speed=
					writeChar(strRPMS[1] + 48);  // Writes Speed=
					writeChar(strRPMS[2] + 48);  // Writes Speed=
					writeChar(strRPMS[3] + 48);  // Writes Speed=
					//writeMessage(msgDisplayTop[1][0], msgDisplayTop[1][1][0]);	// Writes Amount

					//writeMessage(strRPMS, 1);	// Writes Amount


					//writeMessage(msgDisplayTop[2][0], msgDisplayTop[2][1][0]);   //Writes RPM
					SysCtlDelay(500);
					//setCursorPosition(0x40); //Jump to second line
				//	writeMessage(msgDisplayBot[0][0], msgDisplayBot[0][1][0]);

				}

			// interrupeted by outer sensor?
			if(signal == 4 ){
				//verify A previous state and XOR it
				if(oldA == 2){
					newA = 0;
				}else{
					newA = 2;
				}

			}
			//interrupted by inner sensor?
			else if(signal == 8){
				//verify B previous state and XOR it
				if(oldB == 1){
					newB = 0;
				}else{
					newB = 1;
				}
			}
			indexLUT = ((oldA+oldB)*4) + (newA + newB);  // mapping to Lookup table to get wheel direction
			oldA = newA;  // newA is now oldA
			oldB = newB;  // newB is now oldB
			signal = 0;  // unset interrupt flag
			if(LUT[indexLUT] == 1){	//Increment list
				//msgCounterTop = incCounter(msgCounterTop,16);
				//msgCounterBot = incCounter(msgCounterBot,16);
				// clearLCD();
				//writeMessage(messages[msgCounterTop][0], messages[msgCounterTop][1][0]);
				setCursorPosition(0x40); //Jump to second line
				writeMessage(msgDisplayBot[1][0], msgDisplayBot[1][1][0]);  // Counter Clock Wise

			}
			if(LUT[indexLUT] == -1){//Decrement List
				msgCounterTop = decCounter(msgCounterTop,16);
				msgCounterBot = decCounter(msgCounterBot,16);
				//clearLCD();
				//writeMessage(messages[msgCounterTop][0], messages[msgCounterTop][1][0]);
				setCursorPosition(0x40); //Jump to second line

				writeMessage(msgDisplayBot[2][0], msgDisplayBot[2][1][0]);  //Clock Wise
			}

				//}
			}

		}
}

// increases counter depending on array size limit
int incCounter(int counter, int size){
	if(counter == size-1){
		counter = 0;
	}else {
		counter++;
	}
	return counter;
}
// decrements counter depending on array size limit
int decCounter(int counter, int size){
	if(counter == 0){
		counter = size -1;
	} else{
		counter--;
	}
	return counter;
}

// interrupt handler for the timer
void Timer0IntHandler(void){
	//Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	millisCounterInit = millisCounterInit+1;
//	// Read the current state of the GPIO(buzzer) pin and
//	// write back the opposite state
//	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1)){
//		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
//	}else{
//		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 2);
//	}
}

void PB1_IntHandler(void)
{
	signal = GPIOIntStatus(GPIO_PORTE_BASE,true);            // set interrupt flag and wihch pin(sensor) interrupted
	GPIOIntClear (GPIO_PORTE_BASE, GPIO_PIN_3| GPIO_PIN_2);   // Pin 2 Outer sensor, Pin 3 Inner Sensor
	if(intStep == 1){
		millisCounterFinal = millisCounterInit;
		millisCounterInit = 0;
	}
	intStep++;
	if(intStep >2){
		intStep = 0;
	}

}
