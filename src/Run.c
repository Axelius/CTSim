/*
 * Run.c
 *
 *  Created on: 06.06.2013
 *      Author: Alexander Winkler
 */
#include "Run.h"


int main(int argc, char *argv[]){
	int ret = 0;
	readSettingsFromConfigFile("config.cfg");


	logIt(DEBUG, "pathToSlice=%s", cfg.pathToSlice);
	logIt(DEBUG, "pathToOutputReconstruction=%s", cfg.pathToOutputReconstruction);


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

