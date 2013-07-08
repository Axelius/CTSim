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
#define LOGLEVEL DEBUG

void logIt(int messageLevel, const char *fmt, ...);





#endif /* LOGGER_H_ */
