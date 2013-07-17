/*
 * Logger.c
 *
 *  Created on: 13.06.2013
 *      Author: Alexander Winkler
 */


#include "Logger.h"

void _logItVerb(int messageLevel, const char * file, const char * function, const int line, const char *fmt, ...){
	if(messageLevel <= LOGLEVEL){
		va_list args;
		va_start(args, fmt);
		char msg[256];
		switch(messageLevel){
#if LOGLEVEL >= TRACE
		case TRACE:	printf("[[LogIt-TRACE][%s].%s.%d] ", file, function, line); break;
#endif
#if LOGLEVEL >= DEBUG
		case DEBUG:	printf("[[LogIt-DEBUG][%s].%s.%d] ", file, function, line); break;
#endif
#if LOGLEVEL >= INFO
		case INFO:	printf("[[LogIt-INFO ][%s].%s.%d] ", file, function, line); break;
#endif
#if LOGLEVEL >= WARN
		case WARN:	printf("[[LogIt-WARN ][%s].%s.%d] ", file, function, line); break;
#endif
#if LOGLEVEL >= ERR
		case ERR:	printf("[[LogIt-ERROR][%s].%s.%d] ", file, function, line); break;
#endif
#if LOGLEVEL >= FATAL
		case FATAL:	printf("[[LogIt-FATAL][%s].%s.%d] ", file, function, line); break;
#endif
		default:	printf("[[LogIt-UNKNW][%s].%s.%d] ", file, function, line); break;
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



void _logItNonVerb(int messageLevel, const char *fmt, ...){
	if(messageLevel <= LOGLEVEL){
		va_list args;
		va_start(args, fmt);
		char msg[256];
		switch(messageLevel){
#if LOGLEVEL >= TRACE
		case TRACE:	printf("[LogIt-TRACE] "); break;
#endif
#if LOGLEVEL >= DEBUG
		case DEBUG:	printf("[LogIt-DEBUG] "); break;
#endif
#if LOGLEVEL >= INFO
		case INFO:	printf("[LogIt-INFO ] "); break;
#endif
#if LOGLEVEL >= WARN
		case WARN:	printf("[LogIt-WARN ] "); break;
#endif
#if LOGLEVEL >= ERR
		case ERR:	printf("[LogIt-ERROR] "); break;
#endif
#if LOGLEVEL >= FATAL
		case FATAL:	printf("[LogIt-FATAL] "); break;
#endif
		default:	printf("[LogIt-UNKNW] "); break;
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


