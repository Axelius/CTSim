/*
 * Logger.c
 *
 *  Created on: 13.06.2013
 *      Author: Alexander Winkler
 */


#include "Logger.h"

void logItVerb(int messageLevel,const char * file, const char * function, const int line, const char *fmt, ...){
	va_list args;
	va_start(args, fmt);
	if(messageLevel <= LOGLEVEL){
		char msg[256];
		switch(messageLevel){
		case FATAL:	printf("[[LogIt-FATAL][%s].%s.%d] ", file, function, line); break;
		case ERR:	printf("[[LogIt-ERROR][%s].%s.%d] ", file, function, line); break;
		case WARN:	printf("[[LogIt-WARN ][%s].%s.%d] ", file, function, line); break;
		case INFO:	printf("[[LogIt-INFO ][%s].%s.%d] ", file, function, line); break;
		case DEBUG:	printf("[[LogIt-DEBUG][%s].%s.%d] ", file, function, line); break;
		case TRACE:	printf("[[LogIt-TRACE][%s].%s.%d] ", file, function, line); break;
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



void logItNonVerb(int messageLevel, const char *fmt, ...){
	va_list args;
	va_start(args, fmt);
	if(messageLevel <= LOGLEVEL){
		char msg[256];
		switch(messageLevel){
		case FATAL:	printf("[LogIt-FATAL] "); break;
		case ERR:	printf("[LogIt-ERROR] "); break;
		case WARN:	printf("[LogIt-WARN ] "); break;
		case INFO:	printf("[LogIt-INFO ] "); break;
		case DEBUG:	printf("[LogIt-DEBUG] "); break;
		case TRACE:	printf("[LogIt-TRACE] "); break;
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


