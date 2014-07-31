/*
 * Run.c
 *
 *  Created on: 06.06.2013
 *      Author: Alexander Winkler
 */
#include "Run.h"


int main(int argc, char *argv[]){
    LARGE_INTEGER frequency;
    LARGE_INTEGER t1, t2;
    double elapsedTime;
	int ret = 0;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&t1);
	if(argc != 2){
		printhelp();
		return EXIT_FAILURE;
	}
	startLogger("log.txt");
	readSettingsFromConfigFile(argv[1]);
	setUpSpectrum();


	logIt(DEBUG, "pathToSlice=%s", cfg.pathToSlice);
	logIt(DEBUG, "pathToOutputReconstruction=%s", cfg.pathToOutputReconstruction);


	setUpAttenuation();
	logIt(INFO, "Everything set up successfully. Starting simulation...");

	ret = simulation(cfg.pathToSlice, cfg.pathToOutputSinogram);
	reconstruction(cfg.pathToOutputSinogram, cfg.pathToOutputReconstruction);
	QueryPerformanceCounter(&t2);
	elapsedTime = (double)(t2.QuadPart - t1.QuadPart) / frequency.QuadPart;
	logIt(INFO, "Total computation time: %f seconds.", elapsedTime);
	logIt(INFO, "Reconstructed image saved as %s. Exiting...", cfg.pathToOutputReconstruction);
	logIt(DEBUG, "main(int argc, char *argv[]) finished.");
	stopLogger();
	return ret;
}

void printhelp(){
	printf("USAGE: CTSim [pathToConfigFile]\n");
}

