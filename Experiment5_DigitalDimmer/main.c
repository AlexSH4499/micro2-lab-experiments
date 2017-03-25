/*
 * main.c
 * Main program for TIVA MCU - Experiment 5
 * Digital Dimer
 * Complementary Task Exercise
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
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"
#include "customLibs/MIL_LCD_lib.h"

#define DESIRED_PWM_FRECUENCY 8000 //In Hz
#define DUTY_CYCLE 0.50  //% Duty Cycle (decimal value)

 /*
 * The code assumes the following pin configuration (7-0 bits)
 * 7  6  5  4  3  2  1  0
 * D7,D6,D5,D4,D3,D2,D1,D0 - port B GPIOs - Data Port
 * RS,R/W,E                    - port A GPIOs - Control Port
 */

//Note Volatile guarantees that the comoiler will not eliminate the variables with that keywoerd
//Note: Be sure to add TARGET_IS_BLIZZARD_RB1 for ROM configuration

volatile uint32_t pwmClockFreq = 0;
volatile uint32_t pwmLoadValue = 0;
int linePos; // Current line position on LCD, 0 = Top, 1 = Bot
uint8_t Lookup[12][1] = {"1","2","3","4","5","6","7","8","9","*","0","#"}; // lookup array for decoding keys
uint8_t bLvlStr[12][4] = {"010%","020%","030%","040%","050%","060%","070%","080%","090%","*","000%","100%" }; // brigthnes levels for displaying on LCD
float bLvlAmnt[12] = {0.1, 0.20, 0.30, 0.40,0.50,0.60,0.70,0.80,0.90,0.01,0.01,0.99};  // TODO calculate diferent brigthnes level amounts
int index = 0; // Index for LOOKUP table to decode keys
int ScanCode = 2;    // Flag for current row activated  // Points to current row thats high,
int ReturnCode = 0;  // flag for column that interrupted // Points to the column that interrupted
int PUSH_FLAG = 0;   // flag to set if interrupted
int dutyCycle = 0.50; //% Duty Cycle (decimal value)
void PB1_IntHandler();  // interrupt handler for columns port C pins 4,5,6

int main(void) {

    //----MCU Initialization----
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); //Set-up the clocking of the MCU to 40MHz

    //--Prepare peripherals (LCD & Keypad)----
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);  // Enable, RS and R/W port for LCD Display
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);  // Data port for LCD display
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);  // Enable Port for keypad columns Pins 4,5,6
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);  // Enable Port for keypad rows Pins 1,2,3,4

    //Set (LCD & Keypad) port pins as outputs
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, ENTIRE_PORT);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4);
    // Set (Keypad) port pins as inputs PIN 4 = Col1, PIN 5 = Col 2, PIN6 = Col 3
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6);
    GPIOIntRegister(GPIO_PORTC_BASE, PB1_IntHandler);
    GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_RISING_EDGE);

    //------------PWM Initialization------------
    //Clock the PWM module by the system clock
    SysCtlPWMClockSet(SYSCTL_PWMDIV_32); //Divide system clock by 32 to run the PWM at 1.25MHz

    //Enabling PWM1 module and Port D
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); //Port where the PWM pin will be selected
    //ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //TODO Why needed?

    //Selecting PWM generator 0 and port D pin 0 (PD0) aa a PWM output pin for module one
    GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0); //Set Port D pin 0 as output //TODO Checkout which ports can be used for PWM functionallity
    GPIOPinConfigure(GPIO_PD0_M1PWM0); //Select PWM Generator 0

    //Determine period register load value
    pwmClockFreq = SysCtlClockGet() / 32; //TODO as Isnt the same as ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_64);? Is something being repeated?
    pwmLoadValue = (pwmClockFreq / DESIRED_PWM_FRECUENCY) - 1; //Load Value = (PWMGeneratorClock * DesiredPWMPeriod) - 1
    PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN); //Set a count-down generator type
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, pwmLoadValue); //Set PWM period determined by the load value

    //Setup PWM Pulse Width Duty Cycle
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, pwmLoadValue * DUTY_CYCLE);
    PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);
    PWMGenEnable(PWM1_BASE, PWM_GEN_0); //Enable PWM Generator
    //-------------------------------------------

    //Setup the LCD
    initializeLCD();
    writeMessage("Br Lvl Selected:", 16);    // Write initial msg on first line

    //Interrupt enable
    GPIOIntEnable(GPIO_PORTC_BASE, GPIO_PIN_4|GPIO_PIN_5| GPIO_PIN_6);


    //Main loop
    while(1){
        SysCtlDelay(5000);   // Delay for Debouncing
        // Write to rows the current scancode
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4, ScanCode);
        if(PUSH_FLAG == 1){//interrupted?
            PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);  // turn PWM on
            PWMGenEnable(PWM1_BASE, PWM_GEN_0); //Enable PWM Generator
            //setCursorPosition(0x40);
            index = 0;// reset index
            PUSH_FLAG=0;   //unset interrupt flag

            //------------Column index mapping------------
            ReturnCode = ReturnCode/16;  // map to columns 1,2,3---16 = 1, 32 =2, 64=3
            if(ReturnCode == 4){//column 3?
                ReturnCode = 2; // map to array ----column1=0,column2=1,column3=2
            }else{
                ReturnCode = ReturnCode/2; //map to array ----column1=0,column2=1,column3=2
            }
            if(ScanCode == 2){  // last row pressed?
                ScanCode = 24;  // set to 24 ===24/8 =3 ---3*3=9--9 = index of last row = *
            }
            index = (ScanCode/8)*3 + ReturnCode;   //triangulate lookup index row*3+ column--3 because theres three keys by rows
            //--------------------------------------------

            if(index >= 0){
                //PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, false);  // turn PWM off
                if(index == 9 || index == 10){  // * ?
                	//---------LCD Output---------
                    setCursorPosition(0x40); //Set cursor to second line
                    writeMessage(bLvlStr[10], 4);   //Write on LCD the selected brigthness level
                    //----------------------------

                    //----Turn off LED
                    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, pwmLoadValue * .01);
                    PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, false); // turn PWM off
                    PWMGenDisable(PWM1_BASE, PWM_GEN_0); //Disable PWM Generator
                    //clearLCD();    //Clear LCD
                    //Does nothing

                }else{
                    //PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, false);  // turn PWM off

                	//---------LCD Output---------
                    //clearLCD();    //Clear LCD
                    setCursorPosition(0x40);
                    //writeMessage("Br Lvl Selected:", 16);    // Write initial msg on first line
                    writeMessage(bLvlStr[index], 4);   //Write on LCD the selected brigthness level
                    //----------------------------

                    //--------LED brighness setup--------
                    float dutyCle = (float) bLvlAmnt[index];
                    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, pwmLoadValue * dutyCle);  // Set PWM to new selected duty cycle
                    PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);  // turn PWM on
                    PWMGenEnable(PWM1_BASE, PWM_GEN_0); //Enable PWM Generator
                    //-----------------------------------
                }
            }
        }

        //------------Row Increment------------
        if(ScanCode >= 16){ //last row reached?
            ScanCode = 2;
        }else{              //pointer go to next row
            ScanCode = ScanCode * 2;
        }
    }
}

void PB1_IntHandler(void)
{
    SysCtlDelay(4000000);  //for debouncing
    ReturnCode = GPIOIntStatus(GPIO_PORTC_BASE,true);  // get column that interrupted
    PUSH_FLAG = 1; // set interrupt flag
    GPIOIntClear (GPIO_PORTC_BASE, GPIO_PIN_4| GPIO_PIN_5 | GPIO_PIN_6);   // pin 4= col1, pin5=col2, pin6=col3
}
