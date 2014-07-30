/*
 * Logger.c
 *
 *  Created on: 13.06.2013
 *      Author: Alexander Winkler
 */


#include "Logger.h"

void _logItVerb(int messageLevel, const char * file, const char * function, const int line, const char *fmt, ...){
	if(messageLevel <= LOGLEVEL){
		char msg[MAXMESSAGELENGTH];
		va_list args;
		va_start(args, fmt);
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
		char msg[MAXMESSAGELENGTH];
		va_list args;
		va_start(args, fmt);
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

#if LOGGERSTYLE == TOFILE
void _logItToFile(int messageLevel, const char * file, const char * function, const int line, const char *fmt, ...){
	if(messageLevel <= LOGLEVEL){
		va_list args;
		va_start(args, fmt);
		char msg[MAXMESSAGELENGTH];
		switch(messageLevel){
#if LOGLEVEL >= TRACE
		case TRACE:	fprintf(_logFile, "[[LogIt-TRACE][%s].%s.%d] ", file, function, line); break;
#endif
#if LOGLEVEL >= DEBUG
		case DEBUG:	fprintf(_logFile, "[[LogIt-DEBUG][%s].%s.%d] ", file, function, line); break;
#endif
#if LOGLEVEL >= INFO
		case INFO:	fprintf(_logFile, "[[LogIt-INFO ][%s].%s.%d] ", file, function, line); break;
#endif
#if LOGLEVEL >= WARN
		case WARN:	fprintf(_logFile, "[[LogIt-WARN ][%s].%s.%d] ", file, function, line); break;
#endif
#if LOGLEVEL >= ERR
		case ERR:	fprintf(_logFile, "[[LogIt-ERROR][%s].%s.%d] ", file, function, line); break;
#endif
#if LOGLEVEL >= FATAL
		case FATAL:	fprintf(_logFile, "[[LogIt-FATAL][%s].%s.%d] ", file, function, line); break;
#endif
		default:	fprintf(_logFile, "[[LogIt-UNKNW][%s].%s.%d] ", file, function, line); break;
		}
		vsprintf(msg, fmt, args);
		if(msg[strlen(msg)-1] == '\n'){
			fprintf(_logFile, "%s", msg);
		}
		else{
			fprintf(_logFile, "%s\n", msg);
		}

		va_end(args);
		fflush(stdout);
	}

}
#endif


