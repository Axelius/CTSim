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
	char line[256];
	char temp[256];
	char * bla;
	int i = 0;
	char* p;
	FILE *config;
	logIt(DEBUG, "readSettingsFromConfigFile(char *conFile) started.");
	config = fopen(conFile, "r");

	if(config == NULL){
		logIt(TRACE, "ConfigFile doesnt exist. Creating default one.");
		config = fopen(conFile, "wb");
		fprintf(config, "pathToSlice=slices/Segmentation1\npathToOutputReconstruction=outData.pgm\npathToOutputSinogram=simulatedSinogram.pgm\nminEnergy=30\nmaxEnergy=120\nenergyLevels=4\nnumberOfProjectionAngles=100\nnumberOfThreads=4\n");
	}

	setCFGToDefault();

	while(fgets(line, 256, config) != NULL){
		p = temp;
		strcpy(temp, line);
		for( ; *p; ++p) *p = (char)tolower(*p); //String in p is now lowercase
		bla = line;
		for(; *bla; ++bla){
			if(*bla == '\n' || *bla == '\r'){
				*bla = '\0';
			}
		}
		logIt(TRACE, "To Lower Case: %s", temp);


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
		if(prefix(temp, "minenergy=")){
			logIt(TRACE, "minenergy found in Config File");
			cfg.minEnergy = atoi(line + strlen("minenergy="));
			logIt(TRACE, "cfg.minEnergy=%d", cfg.minEnergy);
		}
		if(prefix(temp, "maxenergy=")){
			logIt(TRACE, "maxenergy found in Config File");
			cfg.maxEnergy = atoi(line + strlen("maxenergy="));
			logIt(TRACE, "cfg.maxEnergy=%d", cfg.maxEnergy);
		}
		if(prefix(temp, "numberofthreads=")){
			logIt(TRACE, "numberofthreads found in Config File");
			cfg.numberOfThreads = atoi(line + strlen("numberofthreads="));
			logIt(TRACE, "cfg.numberOfThreads=%d", cfg.numberOfThreads);
		}
		if(prefix(temp, "energylevels=")){
			logIt(TRACE, "energylevels found in Config File");
			cfg.energyLevels = atoi(line + strlen("energylevels="));
			logIt(TRACE, "cfg.energyLevels=%d", cfg.energyLevels);
		}
		if(prefix(temp, "numberofprojectionangles=")){
			logIt(TRACE, "numberOfProjectionAngles found in Config File");
			cfg.numberOfProjectionAngles = atoi(line + strlen("numberOfProjectionAngles="));
			logIt(TRACE, "cfg.numberOfProjectionAngles=%d", cfg.numberOfProjectionAngles);
		}



		i++;
	}


	logIt(DEBUG, "Config File read completely");

	repairInvalidCFGEntries();


	logIt(DEBUG, cfgString());
	fclose(config);
	logIt(DEBUG, "readSettingsFromConfigFile(char *conFile) finished.");
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
	sprintf(message, "pathToSlice=%s, pathToOutputReconstruction=%s, pathToOutputSinogram=%s, cfg.minEnergy=%d, cfg.maxEnergy=%d, cfg.energyLevels=%d, cfg.numberOfProjectionAngles=%d, cfg.numberOfThreads=%d", cfg.pathToSlice, cfg.pathToOutputReconstruction, cfg.pathToOutputSinogram, cfg.minEnergy, cfg.maxEnergy, cfg.energyLevels, cfg.numberOfProjectionAngles, cfg.numberOfThreads);
	return message;
}

void setCFGToDefault(){
	logIt(DEBUG, "setCFGToDefault() started.");
	strcpy(cfg.pathToSlice , "slices/segmentation1");
	strcpy(cfg.pathToOutputReconstruction , "outdata.pgm");
	strcpy(cfg.pathToOutputSinogram , "simulatedsinogram.pgm");
	cfg.minEnergy = 30;
	cfg.maxEnergy = 140;
	cfg.energyLevels = 2;
	cfg.numberOfThreads = 4;
	cfg.numberOfProjectionAngles = 100;
	logIt(DEBUG, "setCFGToDefault() finished.");
}

void repairInvalidCFGEntries(){
	int repairs = 0;
	logIt(DEBUG, "Cleaning up invalid configurations if necessary:");
	if(cfg.minEnergy == cfg.maxEnergy){
		logIt(INFO, "cfg.minEnergy == cfg.maxEnergy. Repairing...");
		cfg.energyLevels = 1;
		(cfg.minEnergy)--;
		repairs++;
	}
	if(cfg.energyLevels <=0){
		logIt(INFO, "cfg.energyLevels <=0. Repairing...");
		cfg.energyLevels = 1;
		repairs++;
	}
	if(cfg.numberOfThreads <=0){
		logIt(INFO, "cfg.numberOfThreads <=0. Repairing...");
		cfg.numberOfThreads = 1;
		repairs++;
	}
	if(cfg.numberOfProjectionAngles<=0){
		logIt(INFO, "cfg.numberOfProjectionAngles<=0. Repairing...");
		cfg.numberOfProjectionAngles = 1;
		repairs++;
	}
	if(cfg.minEnergy<0){
		logIt(INFO, "cfg.minEnergy<0. Repairing...");
		cfg.minEnergy = 0;
		repairs++;
	}
	if(cfg.maxEnergy < cfg.minEnergy){
		logIt(INFO, "cfg.maxEnergy < cfg.minEnergy. Repairing...");
		cfg.maxEnergy = cfg.minEnergy+1;
		repairs++;
	}
	if(cfg.pathToOutputReconstruction[0] == '\0'){
		logIt(INFO, "cfg.pathToOutputReconstruction is empty. Repairing...");
		strcpy(cfg.pathToOutputReconstruction, "outData.pgm");
		repairs++;
	}
	if(cfg.pathToOutputSinogram[0] == '\0'){
		logIt(INFO, "cfg.pathToOutputSinogram is empty. Repairing...");
		strcpy(cfg.pathToOutputSinogram, "simulatedSinogram.pgm");
		repairs++;
	}
	if(cfg.pathToSlice[0] == '\0'){
		logIt(INFO, "cfg.pathToSlice is empty. Repairing...");
		strcpy(cfg.pathToSlice, "slices/Segmentation1");
		repairs++;
	}
	if(cfg.energyLevels == 1){
		cfg.maxEnergy = cfg.minEnergy;
	}


	if(repairs == 0){
		logIt(DEBUG, "CFG seems to be valid. Nothing repaired.");
	}
}
