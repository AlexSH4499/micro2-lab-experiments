/*
 * main.c
 * Main program for TIVA MCU - Experiment 7.2.3
 * Inter-Integrated Circuit
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


/*
 * main.c
 */
int main(void) {
	//----MCU Initialization----
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); //Set-up the clocking of the MCU to 40MHz
	
	//I2C Initialization
	//Init Procedure
		//Set bus speed and enable master module - I2CMAsterInitExpClk();
	//After succesful init, data can be transmited


}
