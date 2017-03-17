/*
 * psk.h
 *
 *  Created on: Oct 3, 2016
 *      Author: Magu
 */

#ifndef PSK_H_
#define PSK_H_

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"


typedef enum {

	initPSK = 0,
	onDelay,
	idlePSK0,
	idlePSK1,
	initPSKchar,	
	outPSKchar,		
	donePSKmess

}pskStates;

typedef struct{

	volatile uint16_t shiftchar;
	volatile byte IMPULSEtimer;		
	pskStates State;
	byte CHbuffer[2];
	byte idleTime;
	byte *pskmesspoint;	
	bool psk_out;
	bool tx_out;
	
}psktype; 


byte sendPSKChar(char c, psktype *pskdata);
byte sendPSKMess(byte *mess, psktype *pskdata);
void PSKprocess(psktype *pskdata);
uint8_t firpsk(int8_t NewSample);

#endif /* PSK_H_ */


/* The preamble is an idle signal of continuous zeroes, corresponding
 * to continuous phase reversals at the symbol rate of 31.25 reversals/second.
 * The postamble is just continuous unmodulated carrier, representing a series of logical ones.
 * 
 * No separate process is needed to define where one character ends and the next begins,
 * since the pattern used to represent a gap between two characters (at least two consecutive zeroes)
 * never occurs in a character. Because no Varicode characters can begin or end with a zero,
 * the shortest character is a single one by itself.
 * 
 *                  0 = phase reversal
 *                  1 = continuous phase
 * 
 * Example bit stream of varicoded character sequence "abc":
 * 
 *           ...|00|1011|00|1011111|00|101111|00|.....
 *                   a          b         c
 */


/*The "preamble" consists of a "warble" of 81 "zeros" being sent  (a zero being represented in PSK31 by a phase shift.)
 *
 * This preamble performs several functions:  Most importantly it allows the receiver to synchronize to the bitstream,
 * it allows the IMD (intermod) reading to be displayed by the receiver, and it provides a listener a distinctive
 *  audible "cue" of the beginning of a transmission.
 *
 * The "tail" consists of 750 milliseconds (3/4 of a second) of unmodulated carrier.  This allows the squelch of the receiver
 * to close gracefully at the end of a transmission, hopefully reducing the possibility of "garbage" characters.
 * Like the "warble" of the preamble, it, too, has a very distinctive sound and provides another
 * audible "cue" of the ending of the transmission.
 */
