/*
 * tty.h
 *
 *  Created on: Aug 31, 2016
 *      Author: Magu
 */

#ifndef TTY_H_
#define TTY_H_

#define figurescode 0x6E
#define letterscode 0x7E
#define BEL 0x52
#define CRS  0x0A
#define LF  0x22
#define SP 0x12
#define BL  0x02
#define TTYNUL 0x02


#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "UTIL1.h"


typedef enum {
	
	figs = 0,
	letters
	
}ttymode;


typedef enum {

	initTTYchar = 0,
	outLETFIG,	
	outTTYchar,		
	doneTTYmess

}ttyStates;

typedef struct {

	volatile byte shiftchar;
	volatile byte IMPULSEtimer;	
	ttymode mode;
	ttyStates State;
	//word offDelay;
	byte CHbuffer[2];
	byte ELEMENTcount;
	byte *ttymesspoint;	
	bool fsk_out;
	bool tx_out;
	
}ttytype ;


byte sendttyChar(char c, ttytype *ttydata);
byte sendttyMess(byte *mess, ttytype *ttydata);
void TTYprocess(ttytype *ttydata);


#endif /* TTY_H_ */
