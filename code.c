/*
 * code.c
 *
 *  Created on: Jul 14, 2016
 *      Author: Magu
 */

#include "code.h"


extern coder mycodeprocess;
extern void Service(void);


//       ASCII - 0x2C = i
const byte npattern[47] = {0xCE,0x86,0x56,0x94,							 // 0x2C...0x2F
                0xFC,0x7C,0x3C,0x1C,0x0C,0x04,0x84,0xC4,0xE4,0xF4,  // 0,1,2....8,9   0x30...0x39
				0xE2,0xAA,0x80,0x8C,0x80,0x32,0x80,					// 0x3A......0x40  0x80s are NULLs
				0x60,0x88,0xA8,0x90,0x40,0x28,0xD0,0x08,0x20,0x78,   // A,B,C.....   0x41....
				0xB0,0x48,0xE0,0xA0,0xF0,0x68,0xD8,0x50,0x10,0xC0,
				0x30,0x18,0x70,0x98,0xB8,0xC8};                    // ....X,Y,Z     ....0x5A

//extern keyer internalkeyer;


/**************************************************************
WinFilter version 0.8
http://www.winfilter.20m.com
akundert@hotmail.com

Filter type: Low Pass
Filter model: Raised Cosine
Roll Off Factor: 1.000000
Sampling Frequency: 1000 Hz
Cut Frequency: 150.000000 Hz
Coefficents Quantization: 8-bit
***************************************************************/
#define NtapASK 31

#define DCgainASK 256

int8_t FIRCoefask[NtapASK] = {
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
           -2,
            3,
           27,
           60,
           77,
           60,
           27,
            3,
           -2,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
    };

uint8_t firask(int8_t NewSample) {

    static int8_t x[NtapASK]; //input samples
    int32_t y=0;            //output sample
    uint8_t n;

    //shift the old samples
    for(n=NtapASK-1; n>0; n--)
       x[n] = x[n-1];

    //Calculate the new output
    x[0] = NewSample;
    for(n=0; n<NtapASK; n++)
        y += FIRCoefask[n] * x[n];

    return (uint8_t)((y / DCgainASK) + 128);  // adjust signed to unsigned
}



byte sendChar(char c, coder *codedata)
{
	if(codedata->State != doneMESS) return ERR_BUSY;
	codedata->CHbuffer[0] = c;
	codedata->CHbuffer[1] = '\0';
	codedata->codemesspoint = codedata->CHbuffer;
	codedata->State = initCHAR;
	return ERR_OK;

}

byte sendMess(byte *mess, coder *codedata)
{
	if(codedata->State != doneMESS) return ERR_BUSY;
	codedata->codemesspoint = mess;
	codedata->State = initCHAR;
	return ERR_OK;
}


void CODEprocess(coder *codedata, byte speed) // called from RTI every 1mS
{
	byte c, i;
	word k = 1200 / speed;

	if(codedata->State == doneMESS)return;

	if (codedata->MARKtimer == 0 )
	{
		codedata->key_out = 0;
		//if(codedata->State != doneMESS) codedata->key_out = 0;
		if(codedata->SPACEtimer) codedata->SPACEtimer--;
	}

	else codedata->MARKtimer--;

	if(codedata->MARKtimer == 0 && codedata->SPACEtimer == 0)
	{	//once both timers are done continue charprocess()

		switch(codedata->State)
		{
			case initCHAR:
				codedata->tx_out = 1;
				c = * codedata->codemesspoint;
				switch (c)
				{                                          // CT
					case '\0':
						codedata->tx_out = 0;
						codedata->State = doneMESS;
						break;

					case ' ':
						codedata->MARKtimer = 0;
						codedata->SPACEtimer = 7 * k;
						codedata->State = nextCHAR;
						break;

					case 0x03:     //ETX
						codedata->shiftchar = _ar;
						codedata->State = outCHAR;
						break;

					case 0x04:     //EOT
						codedata->shiftchar =	_va;
						codedata->State = outCHAR;
						break;

					case 0x16:     //SYN
						codedata->shiftchar =	_bt;
						codedata->State = outCHAR;
						break;

					case 0x10:
						codedata->shiftchar =	_bk;
						codedata->State = outCHAR;
						break;

					case 0x17:
						codedata->shiftchar =	_kn;
						codedata->State = outCHAR;
						break;

					case 0x1B:
						codedata->shiftchar =	_as;
						codedata->State = outCHAR;
						break;

					case 0x08:
						codedata->shiftchar =	_aa;
						codedata->State = outCHAR;
						break;

					case 0x01:
						codedata->shiftchar =	_ct;
						codedata->State = outCHAR;
						break;

					default:
						if (c < 44 || c > 122){
						codedata->MARKtimer = 0;
						codedata->SPACEtimer = 5 * k; // k is speed constant calculated at start
						codedata->State = nextCHAR;
						break;
						}

					    if (c > 96 && c < 123)	c -= 32;  // convert lower case to upper case

						i = c - 44;  // convert ASCII to index
						codedata->shiftchar = npattern[i];
						codedata->State = outCHAR;
				}
				break;

			case outCHAR:

				switch(codedata->shiftchar)
				{
					case 0x80:
						codedata->SPACEtimer = 2 * k; // space between chars
						codedata->State = nextCHAR;
						break;

					default:
						if (codedata->shiftchar & 0x80)codedata-> MARKtimer = 3 * k; //dash
						else codedata->MARKtimer =  k; // dot

						codedata->key_out = 1;

						codedata->shiftchar =	(codedata->shiftchar << 1);  // shift and save for next element
						codedata->SPACEtimer =  k; // space between elements
						  // process elements
				}
				break;

			case nextCHAR:
				codedata->codemesspoint++;
				codedata->State = initCHAR;
				break;

			case doneMESS:
				codedata->tx_out = 0;
				codedata->SPACEtimer = 7 * k; // space to next message
				break;

		}
	}
}




