/*
 * shell.h
 *
 *  Created on: Jul 14, 2016
 *      Author: Magu
 */

#ifndef SOURCES_SHELL_H_
#define SOURCES_SHELL_H_


#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "CLS1.h"
#include "UTIL1.h"
#include "TmDt1.h"
#include "app.h"



void SHELL_Parse(void);

void SHELL_Init(void);


uint8_t Time_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);
uint8_t Date_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);
uint8_t Alarm_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);
uint8_t CW_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);
uint8_t Bcast_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);
uint8_t Chan_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);




#endif /* SOURCES_SHELL_H_ */
