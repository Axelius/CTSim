/*
 * Logger.c
 *
 *  Created on: 13.06.2013
 *      Author: Alexander
 */

#include <stdio.h>
#include "Logger.h"


void logIt(int messageLevel, const char *message){
	if(messageLevel <= LOGLEVEL){
		printf("[LogIt] ");
		switch(messageLevel){
		case FATAL: printf("FATAL: "); break;
		case ERROR: printf("ERROR: "); break;
		case WARN: printf("WARN: "); break;
		case INFO: printf("INFO: "); break;
		case DEBUG: printf("DEBUG: "); break;
		case TRACE: printf("TRACE: "); break;
		default: break;
		}
		printf("%s\n", message);
		fflush(stdout);
	}
}


