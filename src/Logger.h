/*
 * Logger.h
 *
 *  Created on: 13.06.2013
 *      Author: Alexander Winkler
 */
#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdio.h>
#include <stdarg.h>
#include <String.h>

#define OFF 0
#define FATAL 1
#define ERR 2
#define WARN 3
#define INFO 4
#define DEBUG 5
#define TRACE 6
#define ALL 7

#define VERBOSELOGGER 1
#define NONVERBOSELOGGER 2


#define LOGGERSTYLE VERBOSELOGGER
#define LOGLEVEL DEBUG



#if LOGGERSTYLE == VERBOSELOGGER
	#define logIt(messageLevel, fmt, ...) _logItVerb(messageLevel, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#else
	#define logIt(messageLevel, fmt, ...) _logItNonVerb(messageLevel, fmt, ##__VA_ARGS__)
#endif


void _logItNonVerb(int messageLevel, const char *fmt, ...);
void _logItVerb(int messageLevel, const char * file, const char * function, const int line, const char *fmt, ...);





#endif /* LOGGER_H_ */
