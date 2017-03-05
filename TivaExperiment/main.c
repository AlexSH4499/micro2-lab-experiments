#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"


int main(void)
{
	uint8_t ui8LED = 2;
	//---Boot sequence---

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); //Set-up the clocking of the MCU (40MHz)
	//Peripheral configuration
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //Enable the peripheral port to use.
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); //Configure the GPIO pins to be used as GPIO outputs. Args: target_port, pin numbers
	//TODO Set the pin current strength (default is 2mA)

	while(1)
	{
		// Turn on the LED
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED); //Write a value (given by the third parameter) to the specified pins given the base port (or GPIO_PORT_BASE).
																				 //The third parameter value should also be 8-bit. The 8-bits of ui8LED will be set to the port_base given the
																				//selected GPIO pins
		// Delay for a bit
		SysCtlDelay(2000000); //Run a 3-instruction cycle a given number of times (2000000 times in this case). @40MHz (0.000025ms/cycle), 0.000075ms per 3 instructions cycles. Total delay time:
		// Cycle through Red, Green and Blue LEDs
		if (ui8LED == 8) {ui8LED = 2;} else {ui8LED = ui8LED*2;} //Multiplying by 2 essentially shifting the MSB to the left
//		ui8LED = ui8LED == 2 ? 4 : 2; //Alternate between red and blue.
	}
}
