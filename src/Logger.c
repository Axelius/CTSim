/*
 * Logger.c
 *
 *  Created on: 13.06.2013
 *      Author: Alexander Winkler
 */


#include "Logger.h"



void logIt(int messageLevel, const char *fmt, ...){
	va_list args;
	va_start(args, fmt);
	if(messageLevel <= LOGLEVEL){
		char msg[256];
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
		vsprintf(msg, fmt, args);
		if(msg[strlen(msg)-1] == '\n'){
			printf("%s", msg);
		}
		else{
			printf("%s\n", msg);
		}

		va_end(args);
		fflush(stdout);
	}


}


