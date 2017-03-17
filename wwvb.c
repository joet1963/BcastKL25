/*
 * wwvb.c
 *
 *  Created on: Jun 15, 2015
 *      Author: Magu
 */


#include "wwvb.h"


const byte STDyear[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const byte LEAPyear[]= {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

extern bool sample_flag;
extern uint32_t samples;



// ************************ find_sync() ******************************
//
//	This routine correlates the sample register with the wwvb 2 bit
//	sync sequence that starts each message.  The first bit looks for 
//	correlation on a bit-by-bit basis.  Once the first sync bit is
//	detected, the second bit is tested as a complete 32-bit word.
//	Sync is declared when both sync bits pass the sync threshold test
//
//	Parameters:
//		Entry: number of sync attempts
//		Exit:  returns 1 if sync found, returns 0 if not found
//
// *******************************************************************

static bool find_sync(uint16_t syncattempts)
{
	byte i, j, sync_count;
	uint32_t sync_temp;

	while(syncattempts)   // sync attempts  31.25mS * n
	{
		while(sample_flag == 0){}  //wait for sample
		sample_flag = 0; 
		sync_count = 0;
		sync_temp = sync ^ samples;
		syncattempts--;
		for (i = 0; i < 32; i++)
		{
			if ((sync_temp & 0x00000001) == 0)  sync_count++;      //count the number of agreements, has to be bitwise &
			sync_temp >>= 1;			
		}

		if(sync_count >= sync_threshold)
		{
			for(j = 0; j < 32; j++)	// get 32 new samples
			{
				while(sample_flag == 0)	{} //wait for sample
				sample_flag = 0;
			}

			sync_count = 0;
			sync_temp = sync ^ samples;	
			for (i = 0; i < 32; i++)
			{
				if ((sync_temp & 0x00000001) == 0) sync_count++;   // count the number of agreements, has to be bitwise &
				sync_temp >>= 1;
			}
			if(sync_count >= sync_threshold) return 1;  // sync found
		}
	}
		return 0;
}


// ************************** get_bit() ***********************
//
//	this routine determines whether the received bit is a 
//	one or zero
//
//  Parameters:
//	Entry:  none
//	Exit: returns 1 or 0
//  increments checksum on the 5 framing bits
//
//	Called from:  getWWVBframe()
//
// *************************************************************

static byte get_bit(byte *checksum)
{
    byte i, j, one_count, zero_count;
	uint32_t one_temp, zero_temp;
		
		//SYNC_LED_NegVal();
		
	for (j = 0; j < 32; j++) 	// get 32 new samples
	{
		while(sample_flag == 0) {} // wait for sample time interrupt
		sample_flag =0;
	}

	one_count = 0;
	zero_count = 0;
	one_temp = one ^ samples;
	zero_temp = zero ^ samples;
	for (i = 0; i < 32; i++)
	{
		if ((one_temp & 0x00000001) == 0) //(one_temp.0 == 0)
		one_count++;
		one_temp >>= 1;
							
		if ((zero_temp & 0x00000001) == 0) //(zero_temp.0 == 0)
		zero_count++;
		zero_temp >>= 1;
	}
		
	if (zero_count >= bit_threshold) return 0;
	if (one_count >= bit_threshold)	 return 1;
	*checksum++;
	return 0;
	
}

// ************* end of get_bit() **********************



static void getWWVBframe(WWVB_Data *WWVB_Mess)
{
	byte checksum = 0;

	if(get_bit(&checksum)) WWVB_Mess->wwv_minutes +=40;         // Bit 1
	if(get_bit(&checksum)) WWVB_Mess->wwv_minutes +=20;         // Bit 2
	if(get_bit(&checksum)) WWVB_Mess->wwv_minutes +=10;         // Bit 3
	(void)get_bit(&checksum);                                   // Bit 4

	// first get 4 bits of minutes data (8, 4, 2, 1, MSB first format)

	if(get_bit(&checksum)) WWVB_Mess->wwv_minutes +=8;          // Bit 5
	if(get_bit(&checksum)) WWVB_Mess->wwv_minutes +=4;          // Bit 6
	if(get_bit(&checksum)) WWVB_Mess->wwv_minutes +=2;          // Bit 7
	if(get_bit(&checksum)) WWVB_Mess->wwv_minutes +=1;          // Bit 8
	(void)get_bit(&checksum);                                   // Bit 9
	(void)get_bit(&checksum);                                   // Bit 10
	(void)get_bit(&checksum);                                   // Bit 11

	if(get_bit(&checksum)) WWVB_Mess->wwv_hours +=20;           // Bit 12
	if(get_bit(&checksum)) WWVB_Mess->wwv_hours +=10;           // Bit 13
	(void)get_bit(&checksum);                                   // Bit 14

	if(get_bit(&checksum)) WWVB_Mess->wwv_hours +=8;            // Bit 15
	if(get_bit(&checksum)) WWVB_Mess->wwv_hours +=4;            // Bit 16
	if(get_bit(&checksum)) WWVB_Mess->wwv_hours +=2;            // Bit 17
	if(get_bit(&checksum)) WWVB_Mess->wwv_hours +=1;            // Bit 18
	(void)get_bit(&checksum);                                   // Bit 19
	(void)get_bit(&checksum);                                   // Bit 20
	(void)get_bit(&checksum);                                   // Bit 21

	if(get_bit(&checksum)) WWVB_Mess->wwv_days +=200;           // Bit 22
	if(get_bit(&checksum)) WWVB_Mess->wwv_days +=100;           // Bit 23
	(void)get_bit(&checksum);                                   // Bit 24
	if(get_bit(&checksum)) WWVB_Mess->wwv_days +=80;            // Bit 25
	if(get_bit(&checksum)) WWVB_Mess->wwv_days +=40;            // Bit 26
	if(get_bit(&checksum)) WWVB_Mess->wwv_days +=20;            // Bit 27
	if(get_bit(&checksum)) WWVB_Mess->wwv_days +=10;            // Bit 28
	(void)get_bit(&checksum);                                   // Bit 29
	if(get_bit(&checksum)) WWVB_Mess->wwv_days +=8;             // Bit 30
	if(get_bit(&checksum)) WWVB_Mess->wwv_days +=4;             // Bit 31
	if(get_bit(&checksum)) WWVB_Mess->wwv_days +=2;             // Bit 32
	if(get_bit(&checksum)) WWVB_Mess->wwv_days +=1;             // Bit 33
	(void)get_bit(&checksum);                                   // Bit 34
	(void)get_bit(&checksum);                                   // Bit 35

	if(get_bit(&checksum)) WWVB_Mess->UT1_sign_add1 +=1;        // Bit 36
	if(get_bit(&checksum)) WWVB_Mess->UT1_sign_sub +=1;         // Bit 37
	if(get_bit(&checksum)) WWVB_Mess->UT1_sign_add2 +=1;        // Bit 38
	(void)get_bit(&checksum);                                   // Bit 39
	if(get_bit(&checksum)) WWVB_Mess->UT1_corr +=8;             // Bit 40
	if(get_bit(&checksum)) WWVB_Mess->UT1_corr +=4;             // Bit 41
	if(get_bit(&checksum)) WWVB_Mess->UT1_corr +=2;             // Bit 42
	if(get_bit(&checksum)) WWVB_Mess->UT1_corr +=1;             // Bit 43
	(void)get_bit(&checksum);                                   // Bit 44

	if(get_bit(&checksum)) WWVB_Mess->wwv_years +=80;           // Bit 45
	if(get_bit(&checksum)) WWVB_Mess->wwv_years +=40;           // Bit 46
	if(get_bit(&checksum)) WWVB_Mess->wwv_years +=20;           // Bit 47
	if(get_bit(&checksum)) WWVB_Mess->wwv_years +=10;           // Bit 48
	(void)get_bit(&checksum);                                   // Bit 49
	if(get_bit(&checksum)) WWVB_Mess->wwv_years +=8;            // Bit 50
	if(get_bit(&checksum)) WWVB_Mess->wwv_years +=4;            // Bit 51
	if(get_bit(&checksum)) WWVB_Mess->wwv_years +=2;            // Bit 52
	if(get_bit(&checksum)) WWVB_Mess->wwv_years +=1;            // Bit 53
	(void)get_bit(&checksum);                                   // Bit 54
	if(get_bit(&checksum)) WWVB_Mess->leap_year +=1;            // Bit 55
	if(get_bit(&checksum)) WWVB_Mess->leap_second +=1;          // Bit 56
	if(get_bit(&checksum)) WWVB_Mess->DST1 +=1;                 // Bit 57
	if(get_bit(&checksum)) WWVB_Mess->DST2 +=1;                 // Bit 58

	WWVB_Mess->checksum = checksum;

}


static byte WWVBrx(WWVB_Data *WWVB_Buffer, byte acqperiod)
{
	WWVB_Data WWVB_Mess1 = {0}, WWVB_Mess2 = {0};
	byte error = 0;

	if(find_sync(acqperiod * 1920))
		{
			getWWVBframe(&WWVB_Mess1);  // 1920 is 1 minute of sync
			if(WWVB_Mess1.checksum != 5) return WWVB_ERR_FRM;
			if(find_sync(acqperiod * 1920))
				{
					getWWVBframe(&WWVB_Mess2);  // 2nd Frame
					if(WWVB_Mess2.checksum != 5) error++;
					if(WWVB_Mess1.wwv_hours != WWVB_Mess2.wwv_hours) error++;
					if(WWVB_Mess1.wwv_days != WWVB_Mess2.wwv_days) error++;
					if(WWVB_Mess1.wwv_years != WWVB_Mess2.wwv_years) error++;
					if(WWVB_Mess1.UT1_sign_add1 != WWVB_Mess2.UT1_sign_add1) error++;
					if(WWVB_Mess1.UT1_sign_add2 != WWVB_Mess2.UT1_sign_add2) error++;
					if(WWVB_Mess1.UT1_sign_sub != WWVB_Mess2.UT1_sign_sub) error++;
					if(WWVB_Mess1.UT1_corr != WWVB_Mess2.UT1_corr) error++;
					if(WWVB_Mess1.leap_second != WWVB_Mess2.leap_second) error++;
					if(WWVB_Mess1.leap_year != WWVB_Mess2.leap_year) error++;
					if(WWVB_Mess1.DST1 != WWVB_Mess2.DST1) error++;
					if(WWVB_Mess1.DST2 != WWVB_Mess2.DST2) error++;
					if(WWVB_Mess1.wwv_minutes != WWVB_Mess2.wwv_minutes +1) error++;
					if(error)
						{
							*WWVB_Buffer = WWVB_Mess1;
							return WWVB_ERR_CHK;
						}
				}
			*WWVB_Buffer = WWVB_Mess2;
			return WWVB_ERR_OK;
		}
	return WWVB_NO_SYNC;
}


byte WWVBsync(TIMEREC *synctime, DATEREC *syncdate)
{
	WWVB_Data WWVB_Buffer; byte month = 0;
	word day;

	if(WWVBrx(&WWVB_Buffer, 5) == WWVB_ERR_OK)
	{
		TmDt1_SetTime(WWVB_Buffer.wwv_hours, WWVB_Buffer.wwv_minutes, 0, 0);
		TmDt1_GetTime(synctime); // this gets the sync time into the synctime struc
		day = WWVB_Buffer.wwv_days;

		if(WWVB_Buffer.leap_year)
		{
			while (day > LEAPyear[month])
			{
			 	day = day - LEAPyear[month];
				month++;
			}
		}

		else
		{
		   	while (day > STDyear[month])
			{
			 	day = day - STDyear[month];
				month++;
			}
		}

		TmDt1_SetDate((2000 + WWVB_Buffer.wwv_years), (month + 1), (byte)day);
		TmDt1_GetDate(syncdate); // this gets the sync date into the syncdate struc
		return 1;
	}
	return 0;
}











