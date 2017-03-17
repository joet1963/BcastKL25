/*
 * tty.c
 *
 *  Created on: Aug 31, 2016
 *      Author: Magu
 */

#include "tty.h"
                                                                                              // includes start and 2 stop bits
const byte ttypatternlets[26] = {0x63,0x4F,0x3B,0x4B,0x43,0x5B,0x2F,0x17,0x33,0x6B,0x75,0x27,            // A - Z  65 - 90
		                     0x1F,0x1B,0x0F,0x37,0x77,0x2B,0x53,0x07,0x73,0x3F,0x67,0x5F,0x53,0x47};

const byte ttypatternfigs[32] = {0x13,0x17,0x47,0x17,0x4B,0x03,0x2F,0x1B,0x7B,0x27,0x03,0x03,0x6B, //SP!"#$%&,()*+'-./
		                     0x63,0x1F,0x5F,0x37,0x77,0x67,0x43,0x2B,0x07,0x57,0x73,0x33,0x0F,0x3B,0x3F,0x03,0x03,0x03,0x4F};   // 0 - 9:;<=>  48 - 57

extern void Service(void);

byte sendttyChar(char c, ttytype *ttydata)
{	
	if(ttydata->State != doneTTYmess) return ERR_BUSY;
	ttydata->CHbuffer[0] = c;
	ttydata->CHbuffer[1] = '\0';
	ttydata->ttymesspoint = ttydata->CHbuffer;
	ttydata->State = initTTYchar;
	return ERR_OK;

}

byte sendttyMess(byte *mess, ttytype *ttydata)
{	
	if(ttydata->State != doneTTYmess) return ERR_BUSY;
	ttydata->ttymesspoint = mess;
	ttydata->State = initTTYchar;
	return ERR_OK;
}


void TTYprocess(ttytype *ttydata) // called from RTI every 1mS
{
	byte c, i;
	byte k = 22; // 22 mS 45.45 Baud	
	
	if(ttydata->State == doneTTYmess) return;	
	
	if(ttydata->IMPULSEtimer) ttydata->IMPULSEtimer--;
	else
	{	//once timer is done continue TTYprocess()

		switch(ttydata->State)
		{
			case initTTYchar:
				ttydata->tx_out = 1;
				c = * ttydata->ttymesspoint;
				ttydata->ELEMENTcount = 8;
				switch (c)
				{                                        
					case '\0':
						ttydata->tx_out = 0;
						ttydata->State = doneTTYmess;
						break;
						
					case '\r':
						ttydata->shiftchar = CRS;
						ttydata->State = outTTYchar;						
						break;
						
					case '\n':
						ttydata->shiftchar = LF;
						ttydata->State = outTTYchar;
						break;
						
					case '\a':
						if(ttydata->mode == letters){ // if not already figures
						ttydata->mode = figs;
						ttydata->shiftchar = figurescode;
						ttydata->State = outLETFIG; // send shift code					    		
						}
						else{
						ttydata->shiftchar = BEL;
						ttydata->State = outTTYchar;	
						}
						
					case ' ':
						ttydata->shiftchar = SP;
						ttydata->State = outTTYchar;
						break;				
						
					
					default:						

						if(c >= 65 && c <= 90) // its a letter?
						{
						 	if(ttydata->mode == figs){ // if not already letters
						   		ttydata->mode = letters;
						   		ttydata->shiftchar = letterscode;
								ttydata->State = outLETFIG; // send shift code					    		
							}
											    	
							else{
								i = c - 65;  // convert ASCII to index for Letters
								ttydata->shiftchar = ttypatternlets[i];
								ttydata->State = outTTYchar;
							}
						}						
						
						else if(c >= 32 && c <= 63) // its a fig?
					    {
					    	if(ttydata->mode == letters){ // if not already figures
					    		ttydata->mode = figs;
					    		ttydata->shiftchar = figurescode;
					    		ttydata->State = outLETFIG; // send shift code					    		
					    	}
					    	
					    	else
					    	{
					    		i = c - 32;  // convert ASCII to index for Figs
					    		ttydata->shiftchar = ttypatternfigs[i];
					    		ttydata->State = outTTYchar;
					    	}
					    }
						
						else
						{
							ttydata->shiftchar = TTYNUL;  // BLANK
							ttydata->State = outTTYchar;
						}
						break;
					
				}                     //space 5 bits mark 
				break;

			case outLETFIG:					
			case outTTYchar:		
				
					if(ttydata->ELEMENTcount-- == 0)
					{					
						//ttydata->State = initTTYchar;
						if(ttydata->State == outTTYchar) ttydata->ttymesspoint++; // only if printable char
						ttydata->IMPULSEtimer = 0 ; // compensate for delay
						ttydata->State = initTTYchar;
					}
					else
					{					
						if (ttydata->shiftchar & 0x80)ttydata->fsk_out = 1; //Mark
						else ttydata->fsk_out = 0; //Space						
						ttydata->shiftchar <<= 1;  // shift and save for next element					
						ttydata->IMPULSEtimer = k;	 // process elements	
					}				
				break;
				
				
			
		} // switch state
	} //if impulsetimer
} //ttyprocess




