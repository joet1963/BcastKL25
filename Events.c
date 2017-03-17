/* ###################################################################
**     Filename    : Events.c
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
** @file Events.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"

#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */

#include "psk.h"
#include "code.h"
#include "app.h"
#include "tty.h"

extern coder mycodeprocess;
extern ttytype myttyprocess;
extern psktype mypskprocess;
extern byte cwspeed;
extern uint32_t Clock;
extern uint32_t samples;
extern word ledCounter;
extern uint32_t bcastTimeVal;
extern uint32_t HourCounter;
extern bool alarmsig;
extern bool bcastsig;
extern bool onHour;
extern bool active;
extern bool testmode;
extern bool sample_flag;

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
void Cpu_OnNMIINT(void)
{
  /* Write your code here ... */
}

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
void TmDt1_OnAlarm(void)
{
  /* Write your code here ... */
	alarmsig = 1;
}

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
void TI1_OnInterrupt(void)
{
  /* Write your code here ... */
	byte ratio;

	Clock++; // shadow clock to main time date module
	HourCounter++;
	if(++ledCounter > 2000) {GREEN_Off(); ledCounter = 0;}
	CODEprocess(&mycodeprocess, cwspeed);
	TTYprocess( &myttyprocess);
	PSKprocess( &mypskprocess);

	if(mycodeprocess.tx_out)
	{
		PWRUP();
		if(mycodeprocess.key_out){ ratio = firask(127); RED_On();}
		else {ratio = firask(-128); RED_Off();}
		ratio /= mycodeprocess.power; // adjust power
		(void)PWM1_SetRatio8(ratio);
		(void)PWM2_SetRatio8(0);  // DBM bias supply 0 volts
	}

	else if(myttyprocess.tx_out)
	{
		PWRUP();
		(void)PWM1_SetRatio8(255);  // full power carrier
		(void)PWM2_SetRatio8(0);   // DBM bias supply 0 volts
		if(myttyprocess.fsk_out)
		{
			FSKMARK();  //these two frequencies are almost always separated by 170 Hz,
			RED_On();	//and the mark frequency is the higher of the two RF frequencies.
		}
		else
		{
			FSKSPACE();
			RED_Off();
		}
	}

	else if(mypskprocess.tx_out)
	{
		PWRUP();
		if(mypskprocess.psk_out == 1){ ratio = firpsk(127); RED_On();}
		else {ratio = firpsk(-128); RED_Off();}
		(void)PWM1_SetRatio8(ratio);
		(void)PWM2_SetRatio8(128);  // DBM bias supply 1.5 volts
	}

	else {PWRDWN(); RED_Off(); (void)PWM1_SetRatio8(0); (void)PWM2_SetRatio8(0);}

}

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
void SM1_OnRxChar(void)
{
  /* Write your code here ... */
}

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
void SM1_OnTxChar(void)
{
  /* Write your code here ... */
}

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
void SM1_OnError(void)
{
  /* Write your code here ... */
}

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
void TI2_OnInterrupt(void)
{
  /* Write your code here ... */
	sample_flag = 1;
	samples >>= 1;
	if(Bit4_GetVal()) samples |= 0x80000000; // sample T_code pin
	else samples &= 0x7FFFFFFF;

}

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

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
