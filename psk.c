/*
 * psk.c
 *
 *  Created on: Oct 3, 2016
 *      Author: Magu
 */


#include "psk.h"

#define Ntappsk 31

#define DCgainpsk 2048

uint16_t varicode[] = {
  0x0355,  // 0 NUL
  0x036d,  // 1 SOH
  0x02dd,  // 2 STX
  0x03bb,  // 3 ETX
  0x035d,  // 4 EOT
  0x03eb,  // 5 ENQ
  0x03dd,  // 6 ACK
  0x02fd,  // 7 BEL
  0x03fd,  // 8 BS
  0x00f7,  // 9 HT
  0x0017,  // 10 LF
  0x03db,  // 11 VT
  0x02ed,  // 12 FF
  0x001f,  // 13 CR
  0x02bb,  // 14 SO
  0x0357,  // 15 SI
  0x03bd,  // 16 DLE
  0x02bd,  // 17 DC1
  0x02d7,  // 18 DC2
  0x03d7,  // 19 DC3
  0x036b,  // 20 DC4
  0x035b,  // 21 NAK
  0x02db,  // 22 SYN
  0x03ab,  // 23 ETB
  0x037b,  // 24 CAN
  0x02fb,  // 25 EM
  0x03b7,  // 26 SUB
  0x02ab,  // 27 ESC
  0x02eb,  // 28 FS
  0x0377,  // 29 GS
  0x037d,  // 30 RS
  0x03fb,  // 31 US
  0x0001,  // 32 SP
  0x01ff,  // 33 !
  0x01f5,  // 34 @
  0x015f,  // 35 #
  0x01b7,  // 36 $
  0x02ad,  // 37 %
  0x0375,  // 38 &
  0x01fd,  // 39 '
  0x00df,  // 40 (
  0x00ef,  // 41 )
  0x01ed,  // 42 *
  0x01f7,  // 43 +
  0x0057,  // 44 ,
  0x002b,  // 45 -
  0x0075,  // 46 .
  0x01eb,  // 47 /
  0x00ed,  // 48 0
  0x00bd,  // 49 1
  0x00b7,  // 50 2
  0x00ff,  // 51 3
  0x01dd,  // 52 4
  0x01b5,  // 53 5
  0x01ad,  // 54 6
  0x016b,  // 55 7
  0x01ab,  // 56 8
  0x01db,  // 57 9
  0x00af,  // 58 :
  0x017b,  // 59 ;
  0x016f,  // 60 <
  0x0055,  // 61 =
  0x01d7,  // 62 >
  0x03d5,  // 63 ?
  0x02f5,  // 64 @
  0x005f,  // 65 A
  0x00d7,  // 66 B
  0x00b5,  // 67 C
  0x00ad,  // 68 D
  0x0077,  // 69 E
  0x00db,  // 70 F
  0x00bf,  // 71 G
  0x0155,  // 72 H
  0x007f,  // 73 I
  0x017f,  // 74 J
  0x017d,  // 75 K
  0x00eb,  // 76 L
  0x00dd,  // 77 M
  0x00bb,  // 78 N
  0x00d5,  // 79 O
  0x00ab,  // 80 P
  0x0177,  // 81 Q
  0x00f5,  // 82 R
  0x007b,  // 83 S
  0x005b,  // 84 T
  0x01d5,  // 85 U
  0x015b,  // 86 V
  0x0175,  // 87 W
  0x015d,  // 88 X
  0x01bd,  // 89 Y
  0x02d5,  // 90 Z
  0x01df,  // 91 [
  0x01ef,  // 92 
  0x01bf,  // 93 ]
  0x03f5,  // 94 ^
  0x016d,  // 95 _
  0x03ed,  // 96 `
  0x000d,  // 97 a
  0x007d,  // 98 b
  0x003d,  // 99 c
  0x002d,  // 100 d
  0x0003,  // 101 e
  0x002f,  // 102 f
  0x006d,  // 103 g
  0x0035,  // 104 h
  0x000b,  // 105 i
  0x01af,  // 106 j
  0x00fd,  // 107 k
  0x001b,  // 108 l
  0x0037,  // 109 m
  0x000f,  // 110 n
  0x0007,  // 111 o
  0x003f,  // 112 p
  0x01fb,  // 113 q
  0x0015,  // 114 r
  0x001d,  // 115 s
  0x0005,  // 116 t
  0x003b,  // 117 u
  0x006f,  // 118 v
  0x006b,  // 119 w
  0x00fb,  // 120 x
  0x005d,  // 121 y
  0x0157,  // 122 z
  0x03b5,  // 123 {
  0x01bb,  // 124 |
  0x02b5,  // 125 }
  0x03ad,  // 126 ~
  0x02b7   // 127 (del)
};


/**************************************************************
WinFilter version 0.8
http://www.winfilter.20m.com
akundert@hotmail.com

Filter type: Low Pass
Filter model: Raised Cosine
Roll Off Factor: 0.500000
Sampling Frequency: 1000 Hz
Cut Frequency: 32.000000 Hz
Coefficents Quantization: 8-bit
***************************************************************/


uint8_t FIRCoefpsk[Ntappsk] = { 
            1,
            8,
           16,
           25,
           35,
           45,
           56,
           67,
           77,
           87,
           96,
          104,
          110,
          115,
          118,
          119,
          118,
          115,
          110,
          104,
           96,
           87,
           77,
           67,
           56,
           45,
           35,
           25,
           16,
            8,
            1
    };

extern void Service(void);
 
uint8_t firpsk(int8_t NewSample) {

    static int8_t x[Ntappsk]; //input samples
    int32_t y=0;            //output sample
    uint8_t n;

    //shift the old samples
    for(n=Ntappsk-1; n>0; n--)
       x[n] = x[n-1];

    //Calculate the new output
    x[0] = NewSample;
    for(n=0; n<Ntappsk; n++)
        y += FIRCoefpsk[n] * x[n];

    return  (uint8_t)((y / DCgainpsk)+128); // adjust signed to unsigned
} 


byte sendPSKChar(char c, psktype *pskdata)
{	
	if(pskdata->State != donePSKmess) return ERR_BUSY;
	pskdata->CHbuffer[0] = c;
	pskdata->CHbuffer[1] = '\0';
	pskdata->pskmesspoint = pskdata->CHbuffer;
	pskdata->State = initPSK;
	return ERR_OK;
}

byte sendPSKMess(byte *mess, psktype *pskdata)
{	
	if(pskdata->State != donePSKmess) return ERR_BUSY;
	pskdata->pskmesspoint = mess;
	pskdata->State = initPSK;
	return ERR_OK;
}


void PSKprocess(psktype *pskdata) // called from RTI every 1mS
{
	byte c;
	byte k = 32; // 32 mS 31.25 Baud	
	
	if(pskdata->State == donePSKmess) return;	
	
	if(pskdata->IMPULSEtimer) pskdata->IMPULSEtimer--;
	else
	{	//once timer is done continue PSKprocess()

		switch(pskdata->State)
		{
			
			case initPSK:
				pskdata->idleTime = 8;  // 8 * 32 mS idle time before transmitting chars
				pskdata->tx_out = 1;
				pskdata->State = onDelay;
				break;
				
			case onDelay:
				
				if(--pskdata->idleTime == 0) pskdata->State = idlePSK0;
				pskdata->psk_out ^=  1; //phase shift
				pskdata->IMPULSEtimer = k; // k-1 ;
				break;
				
			case idlePSK0:
				
				pskdata->psk_out ^=  1; //phase shift
				pskdata->IMPULSEtimer = k; // k-1 ; 
				pskdata->State = idlePSK1;				
				break;
				
			case idlePSK1:
				
				pskdata->psk_out ^=  1; //phase shift
				pskdata->IMPULSEtimer = k-1; // k-3 ; 	// -2 to compensate for initPSKchar 2 mS delay that's added			
				pskdata->State = initPSKchar;
				break;				
		
			case initPSKchar:
				
				c = *pskdata->pskmesspoint;
				
				switch (c)
				{                                        
					case '\0':
						pskdata->tx_out = 0;
						pskdata->State = donePSKmess;
						break;						
					
					default:								
					    	
					   	if( c <= 127 ) pskdata->shiftchar = varicode[c];					   	   						
						else pskdata->shiftchar = 32;  // BLANK						
						pskdata->State = outPSKchar;						
						break;					
				}
				break;
							
			case outPSKchar:					
					                              // 2 0's between chars 
				if(pskdata->shiftchar == 0)
				{					
					pskdata->psk_out ^= 1; //phase shift terminates char     1st 0
					pskdata->IMPULSEtimer = k; //k-1;
					pskdata->pskmesspoint++;
					pskdata->State = idlePSK1; // space to next char
				}
																	
				else
				{
					if(!(pskdata->shiftchar & 0x0001)) pskdata->psk_out ^= 1; //phase shift						
					pskdata->shiftchar >>= 1;  // shift and save for next element						
					pskdata->IMPULSEtimer = k; //k-1;	// process elements											
				}						
				break;
		}
	}
}
