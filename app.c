/*
 * app.c
 *
 *  Created on: Jul 14, 2016
 *      Author: Magu
 */

#include "app.h"
// change
#include "app.h"


static byte script_buf[64] = {0};


extern TIMEREC sked[];
extern byte flist[];
extern CLS1_ConstStdIOType *io;
extern const byte fhop[];
extern const byte ttyfhop[];
//extern const byte thop[];
extern TIMEREC MyTime;
extern DATEREC MyDate;
extern TIMEREC Mysynctime;
extern DATEREC Mysyncdate;
extern coder mycodeprocess;
extern ttytype myttyprocess;
extern psktype mypskprocess;
extern testtype mytestprocess;
extern apptype Myappdata;
extern TIMEREC MyBcast;
extern uint32_t Clock;
extern uint32_t HourCounter;
extern word ledCounter;
extern uint32_t bcastTimeVal;
//extern uint32_t bcastreg;
extern bool EvAlert;
extern bool alarmsig;
extern bool bcastsig;
extern bool onHour;

extern bool TimeToSync;
extern byte cwspeed;
extern byte bcastchannel;
extern byte ttychannel;
extern byte pskchannel;

// add entry

static byte scriptCODE(eventtype events[], coder *codeprocess );
static byte scriptTTY(eventtype events[], ttytype *ttyprocess );
static byte scriptPSK(eventtype events[], psktype *pskprocess );

int8_t temp(void)
{   // LM34 for Fahrenheit LM35 for Centigrade 10mV/deg
	word adcresult;
	while (AD1_Measure(1)){};
	(void)AD1_GetChanValue16(0, &adcresult);
	return (int8_t)(((((adcresult * 10) / 34) - 32 ) * 5) / 9); //converted to C
	//return (int8_t)((adcresult * 10) / 34); // 3.0 in F
}

int8_t TXpwr(void)
{
	word adcvalue;
	while (AD1_Measure(1)){};
	(void)AD1_GetChanValue16(1, &adcvalue);
	return((adcvalue * 64) / 546) - (82 + atten);  //adjust to dBm
}

uint8_t Batt(void)
{
	word adcreading;
	while (AD1_Measure(1)){};
	(void)AD1_GetChanValue16(2, &adcreading);
	return (uint8_t)(adcreading / 34);
}

static byte cal(int8_t compensation, TIMEREC *Time)
{
	byte adjust;
	TIMEREC NewTime = *Time;
	if(NewTime.Sec < 20 || NewTime.Sec > 40 ) return ERR_RANGE ;
	adjust = NewTime.Sec + compensation;

	(void)TmDt1_SetTime(NewTime.Hour, NewTime.Min, adjust, NewTime.Sec100);
	return ERR_OK;
}

void chanset(byte newchannel)
{
	if(newchannel > 29 && newchannel < 130)
	{
		Bits1_PutVal(newchannel);
	}
}

static byte calcChan( const byte hop[],DATEREC *Date )
{
	return hop[Date->Day - 1];
}


void AppInit(void)
{

	EvAlert = 0;
	Myappdata.BcastFlag = 1;
	Myappdata.ident0 = 1;
	Myappdata.ident1 = 1;
	mycodeprocess.State = doneMESS;
	myttyprocess.State = doneTTYmess;
	mypskprocess.State = donePSKmess;
	mycodeprocess.power = 1;
	//mycodeprocess.kinterrupt = 0;
	mycodeprocess.MARKtimer = 0;
	mycodeprocess.SPACEtimer = 0;
	myttyprocess.IMPULSEtimer = 0;
	mypskprocess.IMPULSEtimer = 0;
	mytestprocess.toggle = 0;
	mytestprocess.length = 0;
	cwspeed = 15;
	bcastchannel = 1;
	ttychannel = 2;
	pskchannel = 3;

}


void BootApp(void)
{
	uint32_t temp = BootTime;
	(void)TmDt1_SetTime((byte)temp/10000, temp%10000/100, temp%10000%100, 0);
	temp = BootAlarm;
	(void)TmDt1_SetAlarm((byte)temp/10000, temp%10000/100, temp%10000%100, 0);
	temp = BootDate;
	(void)TmDt1_SetDate((byte)temp/10000, temp%10000/100, temp%10000%100);
	temp = BcastTime;
	MyBcast.Hour = (byte)temp/10000;
	MyBcast.Min = temp%10000/100;
	MyBcast.Sec = temp%10000%100;
	MyBcast.Sec100 = 0;

}

void TestApp(coder *codeprocess, ttytype *ttyprocess, psktype *pskprocess, testtype *testprocess, bool toggle)
{
	byte chan = 63;

	if(toggle)
	{
		if(++testprocess->debounce > 50)  // debounce switch
		{
			testprocess->toggle = 1;
			testprocess->length = 0;
		}
		return;
	}
	else testprocess->debounce = 0;

	if(++testprocess->length > 60000) testprocess->state = testoff;

	switch(testprocess->state)
	{
		case testoff:
			testprocess->length = 0;
			if(testprocess->toggle)
			{
				chanset(chan);
				testprocess->state = testcode;
				testprocess->toggle = 0;
				CLS1_SendStr("Testing CODE\r\n", io->stdOut);
			}
			break;

		case testcode:
			if(testprocess->toggle)
			{
				while(codeprocess->State != doneMESS){}; // this causes a wait for message to finish
				testprocess->state = testtty;
				testprocess->toggle = 0;
				CLS1_SendStr("Testing TTY\r\n", io->stdOut);
			}
			else (void)sendMess("VVVVVVVVVVVV", codeprocess);
			break;

		case testtty:
			if(testprocess->toggle)
			{
				while(ttyprocess->State != doneMESS){}; // this causes a wait for message to finish
				testprocess->state = testpsk;
				testprocess->toggle = 0;
				CLS1_SendStr("Testing PSK\r\n", io->stdOut);
			}
			else (void)sendttyMess("RYRYRYRYRYRY", ttyprocess);
			break;

		case testpsk:
			if(testprocess->toggle)
			{
				while(pskprocess->State != doneMESS){};
				testprocess->state = testoff;
				testprocess->toggle = 0;
				CLS1_SendStr("Test End\r\n", io->stdOut);
			}
			else (void)sendPSKMess("Test Test Test", pskprocess);
			break;
	}
}

static void updatePersist(eventtype events[])
{
	byte Loc = 0;

	while(Loc < 32){
		if(events[Loc].persist != 0) events[Loc].persist--;
		Loc++;
	}
}

static byte chkDupEvent(eventtype pastevents[], eventtype *currentevent)
{
	byte Loc = 0, i = 0;

	while(i <= 32 && pastevents[Loc].type != currentevent->type){
		i++;
		Loc++;
	}
	return Loc;
}


static byte nextEventLoc(eventtype events[])
{
	byte nextFreeLoc = 0;

	while(events[nextFreeLoc].persist != 0 && nextFreeLoc < 32) nextFreeLoc++;

	return nextFreeLoc;
}

static void xferEvent(eventtype pastevents[], eventtype *currentevent)
{
	byte nextevent;
	byte dupevent = chkDupEvent(pastevents, currentevent);


	if(dupevent > 31) //  duplicate?
	{
		nextevent = nextEventLoc(pastevents);
		pastevents[nextevent] = *currentevent; //xfer current to next free location
		pastevents[nextevent].count++;
		return;
	}
		pastevents[dupevent] = *currentevent; // xfer over duplicate
		pastevents[dupevent].count++;
		 return;
}


void ident(TIMEREC *Time, coder *codeprocess, TIMEREC *synctime, DATEREC *syncdate, apptype *appdata, bool alert)
{
	if(HourCounter > 3600000)
	{
		(void)TmDt1_GetTime(Time);
		CLS1_SendStr("Ident\r\n", io->stdOut);

		HourCounter = (Time->Min * 60000) + (Time->Sec * 1000);  // 0 - 36000000 adjust counter with RTC

		chanset(identchan0);
		if(alert) {while( sendMess("T T T", codeprocess) ){};}
		else { while( sendMess("E E E", codeprocess) ){};}

		chanset(identchan1);
		if(alert) {while( sendMess("T T T", codeprocess)){};}
		else { while( sendMess("E E E", codeprocess)){};}

		if( TimeToSync && (WWVBsync(synctime, syncdate) == 1)) TimeToSync = 0;
	}

}


static uint32_t masterClockSet(TIMEREC *Time)
{
	uint32_t MasterClock = 0;         // 0 - 86400000
	(void)TmDt1_GetTime(Time);
	return MasterClock = (Time->Hour * 3600000) + (Time->Min * 60000) + (Time->Sec * 1000);
}


void application(DATEREC *Date, TIMEREC *Time, coder *codeprocess, ttytype *ttyprocess, psktype *pskprocess, eventtype events[], apptype *appdata, bool *alert)
{
	byte i = 0;
	//bool sent = 0;

	//(void)TmDt1_GetTime(Time);
	//TmDt1_GetDate(&Date);
	if(ledCounter > 1975 ) GREEN_On();

	if(Clock > 86400000)
	{
		TimeToSync = 1;
		Clock = masterClockSet(Time);
		updatePersist(events);
		appdata->update = 0;
		appdata->BcastFlag = 1;
		//while (cal(2, Time)){};
	}



	while(i < 3)
	{
		if( sked[i].Hour == Time->Hour  &&
			sked[i].Min  == Time->Min  &&
			Time->Sec >= 45 ) {
			chanset(flist[i]);

			GREEN_Off();
			(void)scriptPSK( events, pskprocess );	}

		*alert = 0;
		i++;
	}

	if(Clock >= bcastTimeVal)
	{

		CLS1_SendStr("Bcast Begin\r\n", io->stdOut);
		chanset(calcChan(fhop, Date));
		GREEN_Off();
		(void)scriptCODE( events, codeprocess );
		GREEN_Off();
		(void)scriptTTY( events, ttyprocess );
		GREEN_Off();
		(void)scriptPSK( events, pskprocess );
		bcastsig = 0;
		appdata->BcastFlag = 0;
		CLS1_SendStr("Bcast End\r\n", io->stdOut);
		*alert = 0;
	}

	if(Clock >= ncastTimeVal)
	{

		CLS1_SendStr("ncast Begin\r\n", io->stdOut);
		chanset(calcChan(fhop, Date));
		GREEN_Off();
		(void)scriptCODE( events, codeprocess );
		GREEN_Off();
		(void)scriptTTY( events, ttyprocess );
		GREEN_Off();
		(void)scriptPSK( events, pskprocess );
		CLS1_SendStr("ncast End\r\n", io->stdOut);
		*alert = 0;
	}

	if(alarmsig == 1)
	{
		chanset(calcChan(ttyfhop, Date));
		i = 0;

		while(i < 4)
		{
			GREEN_Off();
			(void)scriptTTY( events, ttyprocess );

			i++;
		}

		*alert = 0;
		alarmsig = 0;
	}

}


bool supervisor(ioType *INputs, ioType *INmem,  coder *codeprocess, eventtype *pastevents, TIMEREC *Time, DATEREC *Date)
{
	bool newEvent = 0;
	byte immediate = 0;
	eventtype currentevent = {0};

	if(INputs->access ^ INmem->access)
	{
		(void)TmDt1_GetTime(Time);
		(void)TmDt1_GetDate(Date);
		if(INputs->access == 1) currentevent.type = accopen;
		else currentevent.type = acclose;
		currentevent.persist = 7;
		currentevent.Date = *Date;
		currentevent.Time = *Time;
		xferEvent(pastevents, &currentevent);
		INmem->access = INputs->access;
		newEvent = 1;
		immediate = 1;
	}

	if(INputs->level ^ INmem->level)
	{
		(void)TmDt1_GetTime(Time);
		(void)TmDt1_GetDate(Date);
		if(INputs->level == 1) currentevent.type = levellow;
		else currentevent.type = levelhigh;
		currentevent.persist = 7;
		currentevent.Date = *Date;
		currentevent.Time = *Time;
		xferEvent(pastevents, &currentevent);
		INmem->level = INputs->level;
		newEvent = 1;
		immediate = 1;
	}

	if(INputs->power ^ INmem->power)
	{
		(void)TmDt1_GetTime(Time);
		(void)TmDt1_GetDate(Date);
		if(INputs->power == 1) currentevent.type = poweron;
		else currentevent.type = poweroff;
		currentevent.persist = 7;
		currentevent.Date = *Date;
		currentevent.Time = *Time;
		xferEvent(pastevents, &currentevent);
		INmem->power = INputs->power;
		newEvent = 1;
		immediate = 0;
	}

	if(INputs->motion ^ INmem->motion)
	{
		(void)TmDt1_GetTime(Time);
		(void)TmDt1_GetDate(Date);
		if(INputs->motion == 1) currentevent.type = motionstart;
		else currentevent.type = motionstop;
		currentevent.persist = 7;
		currentevent.Date = *Date;
		currentevent.Time = *Time;
		xferEvent(pastevents, &currentevent);
		INmem->motion = INputs->motion;
		newEvent = 1;
		immediate = 1;
	}

	if(INputs->noise ^ INmem->noise)
	{
		(void)TmDt1_GetTime(Time);
		(void)TmDt1_GetDate(Date);
		if(INputs->noise == 1) currentevent.type = noisestart;
		else currentevent.type = noisestop;
		currentevent.persist = 7;
		currentevent.Date = *Date;
		currentevent.Time = *Time;
		xferEvent(pastevents, &currentevent);
		INmem->noise = INputs->noise;
		newEvent = 1;
		immediate = 0;
	}

	if(INputs->spare1 ^ INmem->spare1)
	{
		(void)TmDt1_GetTime(Time);
		(void)TmDt1_GetDate(Date);
		if(INputs->spare1 == 1) currentevent.type = spare1on;
		else currentevent.type = spare1off;
		currentevent.persist = 7;
		currentevent.Date = *Date;
		currentevent.Time = *Time;
		xferEvent(pastevents, &currentevent);
		INmem->spare1 = INputs->spare1;
		newEvent = 1;
		immediate = 0;
	}

	if(INputs->spare2 ^ INmem->spare2)
	{
		(void)TmDt1_GetTime(Time);
		(void)TmDt1_GetDate(Date);
		if(INputs->spare2 == 1) currentevent.type = spare2on;
		else currentevent.type = spare2off;
		currentevent.persist = 7;
		currentevent.Date = *Date;
		currentevent.Time = *Time;
		xferEvent(pastevents, &currentevent);
		INmem->spare2 = INputs->spare2;
		newEvent = 1;
		immediate = 0;
	}

	if(INputs->spare3 ^ INmem->spare3)
	{
		(void)TmDt1_GetTime(Time);
		(void)TmDt1_GetDate(Date);
		if(INputs->spare3 == 1) currentevent.type = spare3on;
		else currentevent.type = spare3off;
		currentevent.persist = 7;
		currentevent.Date = *Date;
		currentevent.Time = *Time;
		xferEvent(pastevents, &currentevent);
		INmem->spare3 = INputs->spare3;
		newEvent = 1;
		immediate = 0;
	}

	if(immediate)
	{
		chanset(bcastchannel);
		GREEN_Off();
		(void)scriptCODE( pastevents, codeprocess );
	}
	return newEvent;
}


void ioHandler(ioType *INputs)
{

	INputs->access = Bit5_GetVal();
	INputs->level =  Bit6_GetVal();
	INputs->power =  Bit7_GetVal();
	INputs->motion = Bit8_GetVal();
	INputs->noise =  Bit11_GetVal();
	INputs->spare1 = Bit12_GetVal();
	INputs->spare2 = Bit13_GetVal();
	INputs->spare3 = Bit1_GetVal();

}


static void scriptwriter(byte script_buf[], eventtype events[], byte eventnumber)
{
	UTIL1_strcat(script_buf, sizeof(script_buf), " EVENT ");
	UTIL1_strcatNum8u(script_buf, sizeof(script_buf), eventnumber );
	UTIL1_strcat(script_buf, sizeof(script_buf), " TYPE ");
	UTIL1_strcatNum16u(script_buf, sizeof(script_buf), events[eventnumber].type );  // was 8u
	UTIL1_strcat(script_buf, sizeof(script_buf), " COUNT ");
	UTIL1_strcatNum8u(script_buf, sizeof(script_buf), events[eventnumber].count );
	UTIL1_strcat(script_buf, sizeof(script_buf), " TIME ");
	UTIL1_strcatNum8u(script_buf, sizeof(script_buf), events[eventnumber].Date.Month );
	UTIL1_strcat(script_buf, sizeof(script_buf), "/");
	UTIL1_strcatNum8u(script_buf, sizeof(script_buf), events[eventnumber].Date.Day );
	UTIL1_strcat(script_buf, sizeof(script_buf), " ");
	if(events[eventnumber].Time.Hour < 10) UTIL1_strcat(script_buf, sizeof(script_buf), "0"); // leading 0
	UTIL1_strcatNum8u(script_buf, sizeof(script_buf), events[eventnumber].Time.Hour );
	UTIL1_strcat(script_buf, sizeof(script_buf), ":");
	if(events[eventnumber].Time.Min < 10) UTIL1_strcat(script_buf, sizeof(script_buf), "0"); // leading 0
	UTIL1_strcatNum8u(script_buf, sizeof(script_buf), events[eventnumber].Time.Min );
	UTIL1_strcat(script_buf, sizeof(script_buf), ". ");
}

static void scriptvitals(byte script_buf[])
{
	UTIL1_strcat(script_buf, sizeof(script_buf), "TEMP ");
	UTIL1_strcatNum8s(script_buf, sizeof(script_buf), temp() );
	UTIL1_strcat(script_buf, sizeof(script_buf), " PWR ");
	UTIL1_strcatNum8s(script_buf, sizeof(script_buf), TXpwr() );
	UTIL1_strcat(script_buf, sizeof(script_buf), " BATT ");
	UTIL1_strcatNum8u(script_buf, sizeof(script_buf), Batt() );
}

static byte scriptCODE(eventtype events[], coder *codeprocess )
{
	byte i = 0, eventnumber = 0;
	script_buf[0] = '\0'; /* initialize buffer */

	while(sendMess("CCCCCC Begin 0", codeprocess)){};  // preamble	0 for power measurement -----
	scriptvitals(script_buf);
	while(sendMess(script_buf, codeprocess)){};

	while(events[eventnumber].persist != 0 && eventnumber < 32)
	{
		script_buf[0] = '\0'; /* initialize buffer */
		scriptwriter(script_buf, events, eventnumber);
		while( sendMess(script_buf, codeprocess) ){};
		while( sendMess("AR", codeprocess)){}; // this causes a wait for event to finish
		eventnumber++;
	}

	if(eventnumber == 0)
	{
		script_buf[0] = '\0'; /* initialize buffer */
		UTIL1_strcat(script_buf, sizeof(script_buf), "NO EVENTS. ");
		while( sendMess(script_buf, codeprocess) ){};
	}

	while(sendMess(" End VA", codeprocess) ){};
	return eventnumber;
}

static byte scriptTTY(eventtype events[], ttytype *ttyprocess )
{
	byte i = 0, eventnumber = 0;
	script_buf[0] = '\0'; /* initialize buffer */

	while(i < 6){ while(sendttyMess("RYRYRYRYRYRY\r\n", ttyprocess)){}; i++;} // preamble
	while( sendttyMess("BEGIN\r\n", ttyprocess)){};
	scriptvitals(script_buf);
	while( sendttyMess(script_buf, ttyprocess) ){};

	while(events[eventnumber].persist != 0 && eventnumber < 32)
	{
		script_buf[0] = '\0'; /* initialize buffer */
		scriptwriter(script_buf, events, eventnumber);
		while( sendttyMess(script_buf, ttyprocess) ){};
		while(sendttyMess("END", ttyprocess)){}; // this causes a wait for event to finish
		eventnumber++;
	}

	if(eventnumber == 0)
	{
		script_buf[0] = '\0'; /* initialize buffer */
		UTIL1_strcat(script_buf, sizeof(script_buf), "NO EVENTS. ");
		while( sendttyMess(script_buf, ttyprocess) ){};
	}

	while( sendttyMess(" End ", ttyprocess) ){};
	return eventnumber;
}

static byte scriptPSK(eventtype events[], psktype *pskprocess )
{
	byte i = 0, eventnumber = 0;
	script_buf[0] = '\0'; /* initialize buffer */

	while(i < 6){ while(sendPSKMess("RYRYRYRYRYRY\r\n", pskprocess)){}; i++;} // preamble
	while( sendPSKMess("BEGIN\r\n", pskprocess)){};
	scriptvitals(script_buf);
	while( sendPSKMess(script_buf, pskprocess) ){};

	while(events[eventnumber].persist != 0 && eventnumber < 32)
	{
		script_buf[0] = '\0'; /* initialize buffer */
		scriptwriter(script_buf, events, eventnumber);
		while( sendPSKMess(script_buf, pskprocess) ){};
		while(sendPSKMess("\r\n", pskprocess)){}; // this causes a wait for event to finish
		eventnumber++;
	}

	if(eventnumber == 0)
	{
		script_buf[0] = '\0'; /* initialize buffer */
		UTIL1_strcat(script_buf, sizeof(script_buf), "NO EVENTS. ");
		while( sendPSKMess(script_buf, pskprocess) ){};
	}

	while( sendPSKMess(" End ", pskprocess) ){};
	return eventnumber;
}
