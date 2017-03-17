/*
 * app.h
 *
 *  Created on: Jul 14, 2016
 *      Author: Magu
 */

#ifndef SOURCES_APP_H_
#define SOURCES_APP_H_

#define BootTime  120000
#define BootDate  20160822
#define	BootAlarm 70000
#define BcastTime 230000
#define ncastTimeVal 18300000 // 23:05 + 5:00

#define atten 30

#define identchan0 44
#define identchan1 62

#define PWRUP()	Bit9_SetVal()
#define PWRDWN() Bit9_ClrVal()
#define FSKMARK() Bit10_SetVal()
#define FSKSPACE() Bit10_ClrVal()
#define LOCK()  Bit2_GetVal(void);
#define TXRLY() Bit3_SetVal();
#define RXRLY() Bit3_ClrVal();


#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "CLS1.h"
#include "UTIL1.h"
#include "TmDt1.h"
#include "Bit5.h"
#include "Bit6.h"
#include "Bit7.h"
#include "Bit8.h"
#include "Bit11.h"
#include "Bit12.h"
#include "Bit13.h"
#include "Bit1.h"
#include "Bits1.h"
#include "BLUE.h"

#include "GREEN.h"
#include "AD1.h"
#include "code.h"
#include "tty.h"
#include "psk.h"
#include "wwvb.h"
#include "shell.h"


typedef struct {

	byte update;
	byte BcastFlag;
	bool ident0;
	bool ident1;
	byte comp;

}apptype;

typedef enum{

	alarm = 0,
	accopen,
	acclose,
	levellow,
	levelhigh,
	poweron,
	poweroff,
	motionstart,
	motionstop,
	noisestart,
	noisestop,
	spare1on,
	spare1off,
	spare2on,
	spare2off,
	spare3on,
	spare3off

}alarms;

typedef enum{

	testoff = 0,
	testcarrier,
	testcode,
	testtty,
	testpsk

}tests;

typedef struct {

	tests state;
	word  length;
	byte debounce;
	bool toggle;

}testtype;

typedef struct {

	DATEREC Date;
	TIMEREC Time;
	alarms type;
	byte count;
	byte persist;

}eventtype;



typedef struct {

	bool access;
	bool level;
	bool power;
	bool motion;
	bool noise;
	bool spare1;
	bool spare2;
	bool spare3;

}ioType;


void application(DATEREC *Date, TIMEREC *Time, coder *codeprocess, ttytype *ttyprocess, psktype *pskprocess, eventtype events[], apptype *appdata, bool *alert);
bool supervisor(ioType *INputs, ioType *INmem, coder *codeprocess, eventtype *pastevents, TIMEREC *Time, DATEREC *Date);
void ident(TIMEREC *Time, coder *codeprocess, TIMEREC *synctime, DATEREC *syncdate, apptype *appdata, bool alert);
int8_t temp(void);
void Service(void);
void AppInit(void);
void BootApp(void);
void TestApp(coder *codeprocess, ttytype *ttyprocess, psktype *pskprocess, testtype *testprocess, bool toggle);
void chanset(byte newchannel);
void ioHandler(ioType *INputs);



/*	MC145151

n	f   	Channel
285	6.999	29
286	7.002	30
287	7.005	31
288	7.008	32
289	7.011	33
290	7.014	34
291	7.017	35
292	7.020	36
293	7.023	37
294	7.026	38
295	7.029	39
296	7.032	40
297	7.035	41
298	7.038	42
299	7.041	43
300	7.044	44
301	7.047	45
302	7.050	46
303	7.053	47
304	7.056	48
305	7.059	49
306	7.062	50
307	7.065	51
308	7.068	52
309	7.071	53
310	7.074	54
311	7.077	55
312	7.080	56
313	7.083	57
314	7.086	58
315	7.089	59
316	7.092	60
317	7.095	61
318	7.098	62
319	7.101	63
320	7.104	64
321	7.107	65
322	7.110	66
323	7.113	67
324	7.116	68
325	7.119	69
326	7.122	70
327	7.125	71
328	7.128	72
329	7.131	73
330	7.134	74
331	7.137	75
332	7.140	76
333	7.143	77
334	7.146	78
335	7.149	79
336	7.152	80
337	7.155	81
338	7.158	82
339	7.161	83
340	7.164	84
341	7.167	85
342	7.170	86
343	7.173	87
344	7.176	88
345	7.179	89
346	7.182	90
347	7.185	91
348	7.188	92
349	7.191	93
350	7.194	94
351	7.197	95
352	7.200	96
353	7.203	97
354	7.206	98
355	7.209	99
356	7.212	100
357	7.215	101
358	7.218	102
359	7.221	103
360	7.224	104
361	7.227	105
362	7.230	106
363	7.233	107
364	7.236	108
365	7.239	109
366	7.242	110
367	7.245	111
368	7.248	112
369	7.251	113
370	7.254	114
371	7.257	115
372	7.260	116
373	7.263	117
374	7.266	118
375	7.269	119
376	7.272	120
377	7.275	121
378	7.278	122
379	7.281	123
380	7.284	124
381	7.287	125
382	7.290	126
383	7.293	127
384	7.296	128
385	7.299	129
386	7.302	130
*/


#endif /* SOURCES_APP_H_ */
