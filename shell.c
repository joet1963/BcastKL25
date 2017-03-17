/*
2 * shell.c
 *
 *  Created on: Jul 14, 2016
 *      Author: Magu
 */


#include "Shell.h"


extern byte cwspeed;
extern byte bcastchannel;

extern TIMEREC MyTime;
extern DATEREC MyDate;
extern TIMEREC MyBcast;
extern TIMEREC MyAlarm;
extern bool alarmsig;
extern bool identsig;
extern uint32_t bcastTimeVal;
extern uint32_t Clock;


/* table with shell parser/handler */
static const CLS1_ParseCommandCallback CmdParserTable[] =
{
  CLS1_ParseCommand,
  Time_ParseCommand,
  Date_ParseCommand,
  Alarm_ParseCommand,
  CW_ParseCommand,
  Bcast_ParseCommand,
  Chan_ParseCommand,
  NULL /* sentinel */
};

static unsigned char localConsole_buf[96]; /* buffer for command line */


void SHELL_Parse(void) {
  (void)CLS1_ReadAndParseWithCommandTable(localConsole_buf, sizeof(localConsole_buf), CLS1_GetStdio(), CmdParserTable);
}

void SHELL_Init(void) {
  localConsole_buf[0] = '\0'; /* initialize buffer */
}


static uint8_t Time_PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr("Time", "Time commands\r\n", io->stdOut);
  CLS1_SendHelpStr(" time help|time", "Print help or display time\r\n", io->stdOut);
  CLS1_SendHelpStr(" time set <hh:mm:ss,hh>", "ex:<18:05:07> ,hh is optional 24hour\r\n", io->stdOut);
  return ERR_OK;
}

static uint8_t Bcast_PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr("BCAST", "BCAST commands\r\n", io->stdOut);
  CLS1_SendHelpStr(" bcast help|bcast", "Print help or display BCAST time\r\n", io->stdOut);
  CLS1_SendHelpStr(" bcast set <hh:mm:ss,hh>", "ex:<18:05:07> ,hh is optional 24hour\r\n", io->stdOut);
  return ERR_OK;
}

static uint8_t Alarm_PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr("Alarm", "Alarm commands\r\n", io->stdOut);
  CLS1_SendHelpStr(" alarm help|alarm", "Print help or display ALARM time\r\n", io->stdOut);
  CLS1_SendHelpStr(" alarm set <hh:mm:ss,hh>", "ex:<18:05:07> ,hh is optional 24hour\r\n", io->stdOut);
  return ERR_OK;
}

static uint8_t CW_PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr("CW Speed", "CW commands\r\n", io->stdOut);
  CLS1_SendHelpStr(" cw help|cw", "Print help or display cw speed\r\n", io->stdOut);
  CLS1_SendHelpStr(" cw set <15>", "5 - 60 WPM\r\n", io->stdOut);
  return ERR_OK;
}

static uint8_t Chan_PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr("Channel", "Channel commands\r\n", io->stdOut);
  CLS1_SendHelpStr(" chan help|chan", "Print help or display channel\r\n", io->stdOut);
  CLS1_SendHelpStr(" chan set <16>", "range 1 - 32\r\n", io->stdOut);
  return ERR_OK;
}

static uint8_t Date_PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr("Date", "Date commands\r\n", io->stdOut);
  CLS1_SendHelpStr(" date help|date", "Print help or display date\r\n", io->stdOut);
  CLS1_SendHelpStr(" date set <dd.mm.yyyy>", "ex:<01.01.10> format dd.mm.yy or dd-mm-yyyy\r\n", io->stdOut);
  return ERR_OK;
}

static uint8_t Time_PrintStatus(const CLS1_StdIOType *io) {
  uint8_t buf[24] = {0};

  (void)TmDt1_GetTime(&MyTime);
  if(MyTime.Hour < 10) UTIL1_strcat(buf, sizeof(buf), "0"); // leading 0
  UTIL1_strcatNum8u(buf, sizeof(buf), MyTime.Hour );
  UTIL1_strcat(buf, sizeof(buf), ":");
  if(MyTime.Min < 10) UTIL1_strcat(buf, sizeof(buf), "0"); // leading 0
  UTIL1_strcatNum8u(buf, sizeof(buf), MyTime.Min );
  UTIL1_strcat(buf, sizeof(buf), ":");
  if(MyTime.Sec < 10) UTIL1_strcat(buf, sizeof(buf), "0"); // leading 0
  UTIL1_strcatNum8u(buf, sizeof(buf), MyTime.Sec );
  UTIL1_strcat(buf, sizeof(buf), "\r\n");
  CLS1_SendStatusStr("Time", buf, io->stdOut);
  return ERR_OK;
}

static uint8_t Alarm_PrintStatus(const CLS1_StdIOType *io) {
  uint8_t buf[24] = {0};

  if(MyAlarm.Hour < 10) UTIL1_strcat(buf, sizeof(buf), "0"); // leading 0
  UTIL1_strcatNum8u(buf, sizeof(buf), MyAlarm.Hour );
  UTIL1_strcat(buf, sizeof(buf), ":");
  if(MyAlarm.Min < 10) UTIL1_strcat(buf, sizeof(buf), "0"); // leading 0
  UTIL1_strcatNum8u(buf, sizeof(buf), MyAlarm.Min );
  UTIL1_strcat(buf, sizeof(buf), ":");
  if(MyAlarm.Sec < 10) UTIL1_strcat(buf, sizeof(buf), "0"); // leading 0
  UTIL1_strcatNum8u(buf, sizeof(buf), MyAlarm.Sec );
  UTIL1_strcat(buf, sizeof(buf), "\r\n");
  CLS1_SendStatusStr("Alarm", buf, io->stdOut);
  return ERR_OK;
}

static uint8_t Bcast_PrintStatus(const CLS1_StdIOType *io) {
  uint8_t buf[24] = {0};

  if(MyBcast.Hour < 10) UTIL1_strcat(buf, sizeof(buf), "0"); // leading 0
  UTIL1_strcatNum8u(buf, sizeof(buf), MyBcast.Hour );
  UTIL1_strcat(buf, sizeof(buf), ":");
  if(MyBcast.Min < 10) UTIL1_strcat(buf, sizeof(buf), "0"); // leading 0
  UTIL1_strcatNum8u(buf, sizeof(buf), MyBcast.Min );
  UTIL1_strcat(buf, sizeof(buf), ":");
  if(MyBcast.Sec < 10) UTIL1_strcat(buf, sizeof(buf), "0"); // leading 0
  UTIL1_strcatNum8u(buf, sizeof(buf), MyBcast.Sec );
  UTIL1_strcat(buf, sizeof(buf), "\r\n");
  CLS1_SendStatusStr("BCAST Time", buf, io->stdOut);
  return ERR_OK;
}

static uint8_t Date_PrintStatus(const CLS1_StdIOType *io) {
  uint8_t buf[24] = {0};

  UTIL1_strcatNum16u(buf, sizeof(buf), MyDate.Year );
  UTIL1_strcat(buf, sizeof(buf), "/");
  if(MyDate.Month < 10) UTIL1_strcat(buf, sizeof(buf), "0"); // leading 0
  UTIL1_strcatNum8u(buf, sizeof(buf), MyDate.Month );
  UTIL1_strcat(buf, sizeof(buf), "/");
  if(MyDate.Day < 10) UTIL1_strcat(buf, sizeof(buf), "0"); // leading 0
  UTIL1_strcatNum8u(buf, sizeof(buf), MyDate.Day );
  UTIL1_strcat(buf, sizeof(buf), "\r\n");
  CLS1_SendStatusStr("Date", buf, io->stdOut);
  return ERR_OK;
}

static uint8_t CW_PrintStatus(const CLS1_StdIOType *io) {
  uint8_t buf[24] = {0};

  UTIL1_strcatNum8u(buf, sizeof(buf), cwspeed );
  UTIL1_strcat(buf, sizeof(buf), "\r\n");
  CLS1_SendStatusStr("CW Speed ", buf, io->stdOut);
  return ERR_OK;
}

static uint8_t Chan_PrintStatus(const CLS1_StdIOType *io) {
  uint8_t buf[24] = {0};

  UTIL1_strcatNum8u(buf, sizeof(buf), bcastchannel );
  UTIL1_strcat(buf, sizeof(buf), "\r\n");
  CLS1_SendStatusStr("Channel ", buf, io->stdOut);
  return ERR_OK;
}

static uint8_t Temp_PrintStatus(const CLS1_StdIOType *io) {
  uint8_t buf[24] = {0};

  UTIL1_strcatNum8s(buf, sizeof(buf), temp() );
  UTIL1_strcat(buf, sizeof(buf), "\r\n");
  CLS1_SendStatusStr("Temp ", buf, io->stdOut);
  return ERR_OK;
}


uint8_t Time_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {

  uint8_t res = 0, hour, minute, second, hSecond;
  const unsigned char *p;

  if (UTIL1_strcmp((char*)cmd, CLS1_CMD_HELP)==0 || UTIL1_strcmp((char*)cmd, "time help")==0) {
    *handled = TRUE;
    res = Time_PrintHelp(io);
  } else if (UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, "time")==0) {
    *handled = TRUE;
    res = Time_PrintStatus(io);
  } else if (UTIL1_strncmp((char*)cmd, "time set", sizeof("time set")-1)==0) {
    *handled = TRUE;
    p = cmd + sizeof("time set")-1;
    res = UTIL1_ScanTime(&p, &hour, &minute, &second, &hSecond);
    if (res==ERR_OK) {

    		(void)TmDt1_SetTime(hour, minute, second, hSecond);
    		Clock = (hour * 3600000) + (minute * 60000) + (second * 1000);
    		(void)TmDt1_GetTime(&MyTime);
    		(void)Time_PrintStatus(io);
    		//identsig = 1;
    		//timeloss = 0;
    		CLS1_SendStr("Time: OK!\r\n", io->stdOut);
    	}

        //(void)outword("OK");
    	else {   CLS1_SendStr("Time: Out of Range!\r\n", io->stdOut);
              // (void)outcharacter('?');
        }

  }
  return res;
}

uint8_t Bcast_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {

  uint8_t res = 0, hour, minute, second, hSecond;
  const unsigned char *p;

  if (UTIL1_strcmp((char*)cmd, CLS1_CMD_HELP)==0 || UTIL1_strcmp((char*)cmd, "bcast help")==0) {
    *handled = TRUE;
    res = Bcast_PrintHelp(io);
  } else if (UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, "bcast")==0) {
    *handled = TRUE;
    res = Bcast_PrintStatus(io);
  }  else if (UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, "send")==0) {
        *handled = TRUE;
        alarmsig = 1;
  } else if (UTIL1_strncmp((char*)cmd, "bcast set", sizeof("bcast set")-1)==0) {
    *handled = TRUE;
    p = cmd + sizeof("bcast set")-1;
    res = UTIL1_ScanTime(&p, &hour, &minute, &second, &hSecond);
    if (res==ERR_OK) {

    		MyBcast.Hour = hour;
    		MyBcast.Min = minute;
    		MyBcast.Sec = second;
    		MyBcast.Sec100 = hSecond;
    		(void)Bcast_PrintStatus(io);
    		//bcastloss = 0;
    		bcastTimeVal = (hour * 3600000) + (minute * 60000) + (second * 1000);
    		CLS1_SendStr(" BCAST Time: OK!\r\n", io->stdOut);
    	}

        //(void)outword("OK");
    	else {   CLS1_SendStr("BCAST Time: Out of Range!\r\n", io->stdOut);
              // (void)outcharacter('?');
        }

  }
  return res;
}



uint8_t Alarm_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {

  uint8_t res = 0, hour, minute, second, hSecond;
  const unsigned char *p;

  if (UTIL1_strcmp((char*)cmd, CLS1_CMD_HELP)==0 || UTIL1_strcmp((char*)cmd, "alarm help")==0) {
    *handled = TRUE;
    res = Alarm_PrintHelp(io);
  } else if (UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, "alarm")==0) {
      *handled = TRUE;
      res = Alarm_PrintStatus(io);
  } else if (UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, "temp")==0) {
       *handled = TRUE;
       res = Temp_PrintStatus(io);
  } else if (UTIL1_strncmp((char*)cmd, "alarm set", sizeof("alarm set")-1)==0) {
    *handled = TRUE;
    p = cmd + sizeof("alarm set")-1;
    res = UTIL1_ScanTime(&p, &hour, &minute, &second, &hSecond);
    if (res==ERR_OK) {

            (void)TmDt1_SetAlarm(hour, minute, second, hSecond);
            MyAlarm.Hour = hour;
            MyAlarm.Min = minute;
            MyAlarm.Sec = second;
            MyAlarm.Sec100 = hSecond;
            (void)Alarm_PrintStatus(io);
            //alarmloss = 0;
    		CLS1_SendStr("Alarm: OK!\r\n", io->stdOut);
    	}

        //(void)outword("OK");
    	else {   CLS1_SendStr("AlarmTime: Out of Range!\r\n", io->stdOut);
              // (void)outcharacter('?');
        }
  }
  return res;
}

uint8_t CW_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {
  uint8_t val;
  uint8_t res;
  const unsigned char *p;

    if (UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, "cw")==0) {
     *handled = TRUE;
     res = CW_PrintStatus(io);
    } else if (UTIL1_strncmp((char*)cmd, "cw set", sizeof("cw set")-1)==0) {
    *handled = TRUE;
    p = cmd + sizeof("cw set")-1;
    res = UTIL1_ScanDecimal8uNumber(&p, &val);
    if (res==ERR_OK) {
    	if(val <= 60 && val >= 5){
            cwspeed = val;
    		CLS1_SendStr("CW Speed: OK!\r\n", io->stdOut);
    	}

        //(void)outword("OK");
    	else {   CLS1_SendStr("CW Speed: Out of Range!\r\n", io->stdOut);
              // (void)outcharacter('?');
        }
    }
  }
  return res;
}

uint8_t Chan_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {
  uint8_t val;
  uint8_t res;
  const unsigned char *p;

    if (UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, "chan")==0) {
     *handled = TRUE;
     res = Chan_PrintStatus(io);
    } else if (UTIL1_strncmp((char*)cmd, "chan set", sizeof("chan set")-1)==0) {
    *handled = TRUE;
    p = cmd + sizeof("chan set")-1;
    res = UTIL1_ScanDecimal8uNumber(&p, &val);
    if (res==ERR_OK) {
    	if(val >= 1 && val <= 32){
    		bcastchannel = val;
            chanset(val);
    		CLS1_SendStr("Channel: OK!\r\n", io->stdOut);
    	}

        //(void)outword("OK");
    	else {   CLS1_SendStr("Channel: Out of Range!\r\n", io->stdOut);
              // (void)outcharacter('?');
        }
    }
  }
  return res;
}

uint8_t Date_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {

  uint8_t res = 0, day, month;
  uint16_t year;
  const unsigned char *p;

  if (UTIL1_strcmp((char*)cmd, CLS1_CMD_HELP)==0 || UTIL1_strcmp((char*)cmd, "date help")==0) {
    *handled = TRUE;
    res = Date_PrintHelp(io);
  } else if (UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, "date")==0) {
    *handled = TRUE;
    res = Date_PrintStatus(io);
  } else if (UTIL1_strncmp((char*)cmd, "date set", sizeof("date set")-1)==0) {
    *handled = TRUE;
    p = cmd + sizeof("date set")-1;
    res = UTIL1_ScanDate(&p, &day, &month, &year);
    if (res==ERR_OK) {

    		(void)TmDt1_SetDate(year, month, day);
    		(void)TmDt1_GetDate(&MyDate);
    		(void)Date_PrintStatus(io);
    		//dateloss = 0;
    		CLS1_SendStr("Date: OK!\r\n", io->stdOut);
    	}

        //(void)outword("OK");
    	else {   CLS1_SendStr("Date: Out of Range!\r\n", io->stdOut);
              // (void)outcharacter('?');
        }

  }
  return res;
}
