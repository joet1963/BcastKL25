/* ###################################################################
**     Filename    : Events.h
**     Project     : BcastKL25
**     Processor   : MKL25Z128VLK4
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-11-01, 10:11, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Contents    :
**         Cpu_OnNMIINT - void Cpu_OnNMIINT(void);
**
** ###################################################################*/
/*!
** @file Events.h
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         

#ifndef __Events_H
#define __Events_H
/* MODULE Events */

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
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

#ifdef __cplusplus
extern "C" {
#endif 

/*
** ===================================================================
**     Event       :  Cpu_OnNMIINT (module Events)
**
**     Component   :  Cpu [MKL25Z128LK4]
*/
/*!
**     @brief
**         This event is called when the Non maskable interrupt had
**         occurred. This event is automatically enabled when the [NMI
**         interrupt] property is set to 'Enabled'.
*/
/* ===================================================================*/
void Cpu_OnNMIINT(void);


void TmDt1_OnAlarm(void);
/*
** ===================================================================
**     Event       :  TmDt1_OnAlarm (module Events)
**
**     Component   :  TmDt1 [TimeDate]
**     Description :
**         This event is called whenever an actual time is equal to the
**         alarm time (set by <SetAlarm> method).
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

/*
** ===================================================================
**     Event       :  TI1_OnInterrupt (module Events)
**
**     Component   :  TI1 [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the component is enabled - <Enable> and the events are
**         enabled - <EnableEvent>). This event is enabled only if a
**         <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void TI1_OnInterrupt(void);

void SM1_OnRxChar(void);
/*
** ===================================================================
**     Event       :  SM1_OnRxChar (module Events)
**
**     Component   :  SM1 [SynchroMaster]
**     Description :
**         This event is called after a correct character is received.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void SM1_OnTxChar(void);
/*
** ===================================================================
**     Event       :  SM1_OnTxChar (module Events)
**
**     Component   :  SM1 [SynchroMaster]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void SM1_OnError(void);
/*
** ===================================================================
**     Event       :  SM1_OnError (module Events)
**
**     Component   :  SM1 [SynchroMaster]
**     Description :
**         This event is called when a channel error (not the error
**         returned by a given method) occurs. The errors can be read
**         using <GetError> method.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

/*
** ===================================================================
**     Event       :  TI2_OnInterrupt (module Events)
**
**     Component   :  TI2 [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the component is enabled - <Enable> and the events are
**         enabled - <EnableEvent>). This event is enabled only if a
**         <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void TI2_OnInterrupt(void);

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

#endif 
/* ifndef __Events_H*/
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
