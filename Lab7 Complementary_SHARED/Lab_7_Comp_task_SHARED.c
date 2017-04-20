#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/i2c.h"
#include "custom-libraries/MIL_LCD_lib.h"
#include "custom-libraries/tivaUtils.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "inc/hw_gpio.h"

///

#define DESIRED_PWM_FRECUENCY 440 //In Hz

#define DUTY_CYCLE 0.50 //% DutyCycle (decimal value)

//Note Volatile guarantees that the comoiler will not eliminate the variables with that keywoerd
//Note: Be sure to add TARGET_IS_BLIZZARD_RB1 for ROM configuration

volatile uint32_t pwmClockFreq = 0;
volatile uint32_t pwmLoadValue = 0;

int ReturnCode = 0;
int PUSH_FLAG = 0;   // flag to set if interrupted
int dutyCycle = 0.50; //% Duty Cycle (decimal value)
void PBs_IntHandler();  // interrupt handler for Push buttons
///



 //Defines for DS1307
 #define SLAVE_ADDRESS 0x68
 #define SEC 0x00
 #define MIN 0x01
 #define HRS 0x02
 #define DAY 0x03
 #define DATE 0x04
 #define MONTH 0x05
 #define YEAR 0x06
 #define CNTRL 0x07

uint32_t g_ui32SysClock;

//initialize I2C module 3
//Slightly modified version of TI's example code
void InitI2C3(void)
{
  //enable I2C module 3
  SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);
  //reset module
  SysCtlPeripheralReset(SYSCTL_PERIPH_I2C2);
  //enable GPIO peripheral that contains I2C 3
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);


  // Select the I2C function for these pins.
  GPIOPinTypeI2CSCL(GPIO_PORTE_BASE, GPIO_PIN_4);
  GPIOPinTypeI2C(GPIO_PORTE_BASE, GPIO_PIN_5);
  // Configure the pin muxing for I2C3 functions on port D0 and D1.
  GPIOPinConfigure(GPIO_PE4_I2C2SCL);
  GPIOPinConfigure(GPIO_PE5_I2C2SDA);

  I2CTxFIFOConfigSet(I2C2_BASE, I2C_FIFO_CFG_TX_MASTER);    // dudable **

  // Enable and initialize the I2C3 master module. Use the system clock for
  // the I2C3 module. The last parameter sets the I2C data transfer rate.
  // If false the data rate is set to 100kbps and if true the data rate will
  // be set to 400kbps.
  I2CMasterInitExpClk(I2C2_BASE, g_ui32SysClock, false);
  //clear I2C FIFOs
  //HWREG(I2C0_BASE + I2C_O_FIFOCTL) = 80008000;
  I2CRxFIFOFlush(I2C2_BASE);    //dudable ***
  I2CTxFIFOFlush(I2C2_BASE);    // dudable ***
}

//Set Time
void SetTimeDate(unsigned char sec, unsigned char min, unsigned char hour,unsigned char day, unsigned char date, unsigned char month,unsigned char year)
{
     I2CSend(SLAVE_ADDRESS,8,SEC,dec2bcd(sec),dec2bcd(min),dec2bcd(hour),dec2bcd(day),dec2bcd(date),dec2bcd(month),dec2bcd(year));
}

//sends an I2C command to the specified slave
void I2CSend(uint8_t slave_addr, uint8_t num_of_args, ...)
{
  // Tell the master module what address it will place on the bus when
  // communicating with the slave.
  I2CMasterSlaveAddrSet(I2C2_BASE, slave_addr, false);
  //stores list of variable number of arguments
  va_list vargs;
  //specifies the va_list to "open" and the last fixed argument
  //so vargs knows where to start looking
  va_start(vargs, num_of_args);
  //put data to be sent into FIFO
  I2CMasterDataPut(I2C2_BASE, va_arg(vargs, uint32_t));
  //if there is only one argument, we only need to use the
  //single send I2C function
  if(num_of_args == 1)
  {
    //Initiate send of data from the MCU
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    // Wait until MCU is done transferring.
    while(I2CMasterBusy(I2C2_BASE));
    //"close" variable argument list
    va_end(vargs);
  }
  //otherwise, we start transmission of multiple bytes on the
  //I2C bus
  else
  {
    //Initiate send of data from the MCU
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    // Wait until MCU is done transferring.
    while(I2CMasterBusy(I2C2_BASE));
    //send num_of_args-2 pieces of data, using the
    //BURST_SEND_CONT command of the I2C module
    unsigned char i;
    for(i = 1; i < (num_of_args - 1); i++)
    {
      //put next piece of data into I2C FIFO
      I2CMasterDataPut(I2C2_BASE, va_arg(vargs, uint32_t));
      //send next data that was just placed into FIFO
      I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
      // Wait until MCU is done transferring.
      while(I2CMasterBusy(I2C2_BASE));
    }
    //put last piece of data into I2C FIFO
    I2CMasterDataPut(I2C2_BASE, va_arg(vargs, uint32_t));
    //send next data that was just placed into FIFO
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    // Wait until MCU is done transferring.
    while(I2CMasterBusy(I2C2_BASE));
    //"close" variable args list
    va_end(vargs);
  }
}

//read specified register on slave device
uint32_t I2CReceive(uint32_t slave_addr, uint8_t reg)
{
  //while(I2CMasterBusy(I2C3_BASE));
  //specify that we are writing (a register address) to the
  //slave device
  I2CMasterSlaveAddrSet(I2C2_BASE, slave_addr, false);
  //specify register to be read
  I2CMasterDataPut(I2C2_BASE, reg);
  //send control byte and register address byte to slave device
  I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_START);

  //SysCtlDelay(10000);

  //wait for MCU to finish transaction
  while(I2CMasterBusy(I2C2_BASE)){};
  //specify that we are going to read from slave device
  I2CMasterSlaveAddrSet(I2C2_BASE, slave_addr, true);
  //send control byte and read from the register we
  //specified
  I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
  //wait for MCU to finish transaction
  //SysCtlDelay(10000);
  while(I2CMasterBusy(I2C2_BASE)){};
  //return data pulled from the specified register
  return I2CMasterDataGet(I2C2_BASE);
}


//convert dec to bcd
unsigned char dec2bcd(unsigned char val)
{
     return (((val / 10) << 4) | (val % 10));
}
// convert BCD to binary
unsigned char bcd2dec(unsigned char val)
{
 return (((val & 0xF0) >> 4) * 10) + (val & 0x0F);
}


//Get Time and Date
unsigned char GetClock(unsigned char reg)
{
     unsigned char clockData = I2CReceive(SLAVE_ADDRESS,reg);
     return bcd2dec(clockData);
     //return clockData;
}


int main(void) {

    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);


    g_ui32SysClock = SysCtlClockGet();
    //-- Initialize MCU PBs----
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);  //Enable PB F0 & F4

    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
    GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOIntRegister(GPIO_PORTF_BASE, PBs_IntHandler);
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_RISING_EDGE);
    //Interrupt enable
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0);
    //--------------------

    //------------PWM Initialization------------
     //Clock the PWM module by the system clock
     SysCtlPWMClockSet(SYSCTL_PWMDIV_32); //Divide system clock by 32 to run the PWM at 1.25MHz

     //Enabling PWM1 module and Port D
     SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
     SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); //Port where the PWM pin will be selected

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

    //initialize I2C module 3
    InitI2C3();

    SetTimeDate(0,0,0,18,18,04,17);
    unsigned char sec,min,hour,day,date,month,year;


    //----LCD Setup----
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);  // Enable, RS and R/W port for LCD Display
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);  // Data port for LCD display
    //Set LCD pins as outputs
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, ENTIRE_PORT);
    //-----------------


    initializeLCD();
//
    writeMessage("Time:", 5);
    setCursorPosition(0x40);
    writeMessage("Date:", 5);




   // unsigned char sec,min,hour,day,date,month,year;


    while(true) {
    setCursorPosition(5);

    hour = GetClock(HRS);

    writeChar(hour/10 +48);

    writeChar((hour-((hour/10)*10)) + 48);

    writeChar(10 + 48);   // writes semi-colon character = ":"

    min = GetClock(MIN);
    writeChar(min/10 +48);

    writeChar((min-((min/10)*10)) + 48);

    writeChar(10 + 48);   // writes semi-colon character = ":"

    sec = GetClock(SEC);


    writeChar(sec/10 +48);

    writeChar((sec-((sec/10)*10)) + 48);


    day = GetClock(DAY);


    date = GetClock(DATE);
    setCursorPosition(0x45);
    writeChar(date/10 +48);

    writeChar((date-((date/10)*10)) + 48);

    writeChar(10 + 48);   // writes semi-colon character = ":"

    month = GetClock(MONTH);
    writeChar(month/10 +48);

    writeChar((month-((month/10)*10)) + 48);

    writeChar(10 + 48);   // writes semi-colon character = ":"

    year = GetClock(YEAR);

    writeChar(year/10 +48);

    writeChar((year-((year/10)*10)) + 48);

    SysCtlDelay(100);
    }

}

void PBs_IntHandler(void)
{
    SysCtlDelay(4000000);  //for debouncing
    ReturnCode = GPIOIntStatus(GPIO_PORTF_BASE,true);  // get button that interrupted
    PUSH_FLAG = 1; // set interrupt flag
    GPIOIntClear (GPIO_PORTF_BASE, GPIO_PIN_4| GPIO_PIN_0);   // pin 4= On, pin0=Off
}
