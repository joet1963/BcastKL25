/*
 * code.h
 *
 *  Created on: Jul 14, 2016
 *      Author: Magu
 */

#ifndef SOURCES_CODE_H_
#define SOURCES_CODE_H_

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "UTIL1.h"
//#include "app.h"
//#include "key.h"

#define OFFDELAY 1000

#define AR  0x03
#define VA  0x04
#define BT  0x16
#define BK  0x10
#define KN  0x17
#define AS  0x1B
#define AA  0x08
#define CT  0x01


#define _ar  0b01010100
#define _va  0b00010110
#define _bt  0b10001100
#define _bk  0b10001011  // DLE 0x10
#define _kn  0b10110100  //
#define _as  0b01000100  //
#define _aa  0b01011000  // LF 0x0A
#define _ct	 0b10101100


typedef enum {

	initCHAR = 0,
	outCHAR,
	nextCHAR,
	doneMESS

}States;

typedef struct {

	volatile byte shiftchar;
	volatile word MARKtimer;
	volatile word SPACEtimer;
	//byte speed;
	States State;
	byte CHbuffer[2];
	byte *codemesspoint;
	bool key_out;
	bool tx_out;
	byte power;
	//bool kinterrupt;
} coder ;


extern const byte npattern[];

byte sendMess(byte *mess, coder *codedata );
byte sendChar(char c, coder *codedata);
void CODEprocess(coder *codedata, byte speed);
uint8_t firask(int8_t NewSample);






#endif /* SOURCES_CODE_H_ */
