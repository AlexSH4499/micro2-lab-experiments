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
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"

//Main subroutine

//Initialize Clock
//Intitialize MCU UART Pins
//---Configue MCU UART---
	//Set baud rate
	//Configure UART

//LOOP:
	//if UART empty
		//Set character 'A' (ASCII) transmission buffer


int main (void){
	//----MCU Initialization----
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); //Set-up the clocking of the MCU to 40MHz

}
