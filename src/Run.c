/*
 * Run.c
 *
 *  Created on: 06.06.2013
 *      Author: Alexander Winkler
 */
#include "Run.h"


int main(int argc, char *argv[]){
	startLogger("log.txt");
	int ret = 0;
	if(argc != 2){
		printhelp();
		return EXIT_FAILURE;
	}
	readSettingsFromConfigFile(argv[1]);
	setUpSpectrum();


	logIt(DEBUG, "pathToSlice=%s", cfg.pathToSlice);
	logIt(DEBUG, "pathToOutputReconstruction=%s", cfg.pathToOutputReconstruction);


	setUpAttenuation();
	logIt(INFO, "Everything set up successfully. Starting simulation...");

	ret = simulation(cfg.pathToSlice, cfg.pathToOutputSinogram);
	reconstruction(cfg.pathToOutputSinogram, cfg.pathToOutputReconstruction);
	logIt(INFO, "Reconstructed image saved as %s. Exiting...", cfg.pathToOutputReconstruction);
	logIt(DEBUG, "main(int argc, char *argv[]) finished.");
	stopLogger();
	return ret;
}

void printhelp(){
	printf("USAGE: CTSim [pathToConfigFile]\n");
}

