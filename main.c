/* ###################################################################
**     Filename    : main.c
**     Project     : BcastKL25
**     Processor   : MKL25Z128VLK4
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-11-01, 10:11, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */

#define debug

/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "TI1.h"
#include "TimerIntLdd1.h"
#include "TU1.h"
#include "CLS1.h"
#include "WAIT1.h"
#include "UTIL1.h"
#include "CS1.h"
#include "AS1.h"
#include "ASerialLdd1.h"
#include "TmDt1.h"
#include "TimeDateLdd1.h"
#include "TU2.h"
#include "PWM1.h"
#include "PwmLdd1.h"
#include "TU3.h"
#include "RED.h"
#include "LEDpin1.h"
#include "BitIoLdd1.h"
#include "BLUE.h"
#include "LEDpin2.h"
#include "BitIoLdd2.h"
#include "AD1.h"
#include "AdcLdd1.h"
#include "GREEN.h"
#include "LEDpin3.h"
#include "BitIoLdd3.h"
#include "PWM2.h"
#include "PwmLdd2.h"
#include "TU4.h"
#include "TU4.h"
#include "BitIoLdd4.h"
#include "BitIoLdd5.h"
#include "SM1.h"
#include "SMasterLdd1.h"
#include "TI2.h"
#include "TimerIntLdd2.h"
#include "TU5.h"
#include "Bit4.h"
#include "BitIoLdd6.h"
#include "WAIT2.h"
#include "Bit9.h"
#include "BitIoLdd8.h"
#include "Bit10.h"
#include "BitIoLdd9.h"
#include "Bit3.h"
#include "Bit5.h"
#include "BitIoLdd12.h"
#include "Bit6.h"
#include "BitIoLdd13.h"
#include "Bit7.h"
#include "BitIoLdd14.h"
#include "Bit8.h"
#include "BitIoLdd15.h"
#include "Bit11.h"
#include "BitIoLdd16.h"
#include "Bit12.h"
#include "BitIoLdd17.h"
#include "Bit13.h"
#include "BitIoLdd18.h"
#include "Bit1.h"
#include "BitIoLdd19.h"
#include "Bits1.h"
#include "BitsIoLdd1.h"
#include "Bit2.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* User includes (#include below this line is not maintained by Processor Expert) */

#include "psk.h"
#include "Shell.h"
#include "code.h"
#include "app.h"
#include "tty.h"
#include "wwvb.h"

const TIMEREC sked[3] = {7,5,0,0, 21,55,0,0, 23,5,0,0};
const byte   flist[3] = {3, 7, 11};

const byte fhop[31] = {61,67,54,62,65,63,68,55,50,52,60,55,59,56,58,53,56,63,51,67,62,66,71,52,63,54,70,64,51,57,69};
const byte ttyfhop[31] = {55,58,61,55,62,56,59,54,60,56,55,60,55,60,59,61,54,55,52,62,60,55,61,58,60,57,59,53,63,56,57};


TIMEREC MyTime;
DATEREC MyDate;
TIMEREC Mysynctime;
DATEREC Mysyncdate;
TIMEREC MyBcast;
TIMEREC MyAlarm;
coder mycodeprocess;
ttytype myttyprocess;
psktype mypskprocess;
testtype mytestprocess;
apptype Myappdata;
ioType MyINputs;
ioType MyINmem;

uint32_t Clock;
uint32_t bcastTimeVal;
//uint32_t bcastreg;
uint32_t samples;
byte cwspeed;
byte bcastchannel = 44;
byte ttychannel = 45;
byte pskchannel = 46;

bool EvAlert;
bool alarmsig;
bool bcastsig;
bool mcastsig;
bool ncastsig;
bool onHour;
bool sample_flag;
bool TimeToSync;
uint32_t HourCounter = 0;
word ledCounter;

eventtype myevents[32] = {0};
CLS1_ConstStdIOType *io;

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  /* For example: for(;;) { } */

	GREEN_On();
  SHELL_Init();
  AppInit();
  io = CLS1_GetStdio();
  cwspeed = 20;
  PWM1_Enable();
  CLS1_SendStr("BCAST40 ver. 1.0\r\n", io->stdOut);
  chanset(bcastchannel);
  GREEN_Off();

  while( sendMess("READY", &mycodeprocess) ){};

#ifdef debug

  MyINmem.access = 0;  // debug
  MyINputs.access = 1; // debug
  MyINmem.level  = 0;  // debug
  MyINputs.level = 1; // debug
  MyINmem.power  = 0;  // debug
  MyINputs.power = 1; // debug

#endif

  while(1)
  {

#ifndef debug
  		ioHandler(&MyINputs);
#endif


  	EvAlert = supervisor(&MyINputs, &MyINmem, &mycodeprocess, myevents, &MyTime, &MyDate);
  	application(&MyDate, &MyTime, &mycodeprocess, &myttyprocess, &mypskprocess, myevents, &Myappdata, &EvAlert);
  	ident(&MyTime, &mycodeprocess, &Mysynctime, &Mysyncdate, &Myappdata, EvAlert);  // every hour
  	TestApp(&mycodeprocess, &myttyprocess, &mypskprocess, &mytestprocess, 1);
  	//TmDt1_GetTime(&MyTime);
  	//TmDt1_GetDate(&MyDate);
  	SHELL_Parse();

  }

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
