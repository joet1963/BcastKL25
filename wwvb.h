/*
 * wwvb.h
 *
 *  Created on: Jun 15, 2015
 *      Author: Magu
 */

#include "PE_Types.h"
#include "TmDt1.h"

#ifndef WWVB_H_
#define WWVB_H_

#define  sync  0xFE000000     /* FC000000 */
#define  one   0xFFFF0000
#define  zero  0xFFFFFF80
#define  sync_threshold  31  // stringent sync threshold
#define  bit_threshold   28   // less stringent bit threshold

#define WWVB_ERR_OK  0
#define WWVB_ERR_CHK 1
#define WWVB_NO_SYNC 2
#define WWVB_ERR_FRM 3

typedef struct{
	byte    wwv_minutes;
	byte    wwv_hours;
  	uint16_t  wwv_years;
  	uint16_t  wwv_days;
  	byte    UT1_sign_add1;
  	byte    UT1_sign_sub;
  	byte    UT1_sign_add2;
  	byte    UT1_corr;
  	byte    leap_year;
  	byte    leap_second;
  	byte 	DST1;
  	byte	DST2;
  	byte	checksum;
} WWVB_Data ;


byte WWVBsync(TIMEREC *synctime, DATEREC *syncdate);


#endif /* WWVB_H_ */
