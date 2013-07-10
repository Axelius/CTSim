/*
 * Run.c
 *
 *  Created on: 06.06.2013
 *      Author: Alexander Winkler
 */
#include "Run.h"


int main(int argc, char *argv[]){
	readSettingsFromConfigFile("config.cfg");

//	if(argc != 3){
//		printhelp();
//		printf("argv[0]=%s\n",argv[0]);
//		logIt(ERR, "Unsupported number of arguments. Exiting...");
//		return 1;
//	}

//	char pathToSlice[200];
//	char pathToOutputReconstruction[200];
//	char pathToOutputSinogram[] = "simulatedSinogram.pgm";

//	sprintf(pathToSlice, argv[1]);
//	sprintf(pathToOutputReconstruction, argv[2]);

	logIt(DEBUG, "pathToSlice=%s", cfg.pathToSlice);
	logIt(DEBUG, "pathToOutputReconstruction=%s", cfg.pathToOutputReconstruction);

	int ret = 0;
	setUpAttenuation();
	logIt(INFO, "Everything set up successfully. Starting simulation...");

	ret = simulation(cfg.pathToSlice, cfg.pathToOutputSinogram);
	reconstruction(cfg.pathToOutputSinogram, cfg.pathToOutputReconstruction);
	logIt(INFO, "Reconstructed image saved as %s. Exiting...", cfg.pathToOutputReconstruction);
	logIt(DEBUG, "main(int argc, char *argv[]) finished.");
	return ret;
}

void printhelp(){
	printf("USAGE: CTSim [pathToSlice] [pathToOutputReconstruction]\n");
}

