/*
 * ConfigReader.c
 *
 *  Created on: 04.07.2013
 *      Author: Alexander
 */


#include "ConfigReader.h"
#include "Logger.h"
#include "Simulator.h"
#include "ReconstructC.h"
#include "Run.h"
#include <string.h>
#include <ctype.h>

void readSettingsFromConfigFile(char *conFile){
	logIt(DEBUG, "readSettingsFromConfigFile(char *conFile) started.");
	char line[256];
	int i = 0;
	FILE *config = fopen(conFile, "r");
	char* found = NULL;
	char* p = NULL;
	while(fgets(line, 256, config) != NULL){
		logIt(DEBUG, "Reading line %d: %s", i, line);

		p = line;
		for( ; *p; ++p) *p = tolower(*p); // String is now lowercase
		logIt(DEBUG, "To Lower Case: %s", line);

		//pathtoOutputSinogram



		i++;
	}

}

int calculateStringLength(char *string) {
	int length = 0;
	while (string[length] != '\0') {
		length++;
	}
	return length;
}
