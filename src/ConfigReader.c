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
	printf("\a");
	logIt(DEBUG, "readSettingsFromConfigFile(char *conFile) started.");
	char line[256];
	char temp[256];
	int i = 0;
	FILE *config = fopen(conFile, "r");

	while(fgets(line, 256, config) != NULL){
		char* p = temp;
		//logIt(DEBUG, "Reading line %d: %s", i, line);
		strcpy(temp, line);
		for( ; *p; ++p) *p = tolower(*p); //String in p is now lowercase
		char * bla = line;
		for(; *bla; ++bla){
			if(*bla == '\n' || *bla == '\r'){
				*bla = '\0';
			}
		}


		logIt(DEBUG, "To Lower Case: %s", temp);

		//pathtoOutputSinogram
		if(prefix(temp, "pathtooutputsinogram=")){
			logIt(TRACE, "pathtoOutputSinogram found in Config File");
			strcpy(cfg.pathToOutputSinogram , line + strlen("pathtooutputsinogram="));
			logIt(TRACE, "cfg.pathToOutputSinogram=%s", cfg.pathToOutputSinogram);
		}
		if(prefix(temp, "pathtoslice=")){
			logIt(TRACE, "pathtoslice found in Config File");
			strcpy(cfg.pathToSlice, line + strlen("pathtoslice="));
			logIt(TRACE, "cfg.pathToSlice=%s", cfg.pathToSlice);
		}
		if(prefix(temp, "pathtooutputreconstruction=")){
			logIt(TRACE, "pathToOutputReconstruction found in Config File");
			strcpy(cfg.pathToOutputReconstruction, line + strlen("pathtooutputreconstruction="));
			logIt(TRACE, "cfg.pathToOutputReconstruction=%s", cfg.pathToOutputReconstruction);
		}


		i++;
	}
	logIt(DEBUG, cfgString());

}

int calculateStringLength(char *string) {
	int length = 0;
	while (string[length] != '\0') {
		length++;
	}
	return length;
}


int prefix(char * string, char * prefix){
	while(*prefix){
		if(*prefix++ != *string++){
			return 0;
		}
	}
	return 1;
}

char* cfgString(){
	char* message = malloc(1024 * sizeof(char));

	sprintf(message, "pathToSlice=%s, pathToOutputReconstruction=%s, pathToOutputSinogram=%s", cfg.pathToSlice, cfg.pathToOutputReconstruction, cfg.pathToOutputSinogram);
	return message;

}
