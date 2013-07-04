/*
 * Run.c
 *
 *  Created on: 06.06.2013
 *      Author: Alexander Winkler
 */
#include "Run.h"


int main(int argc, char *argv[]){

	if(argc != 3){
		printhelp();
		printf("argv[0]=%s\n",argv[0]);
		logIt(ERROR, "Unsupported number of arguments. Exiting...");
		return 1;
	}

	char pathToSlice[200];
	char pathToOutputReconstruction[200];
	char pathToOutputSinogram[] = "simulatedSinogram.pgm";

	sprintf(pathToSlice, argv[1]);
	sprintf(pathToOutputReconstruction, argv[2]);

	logIt(DEBUG, "pathToSlice=%s", pathToSlice);
	logIt(DEBUG, "pathToOutputReconstruction=%s", pathToOutputReconstruction);

	int ret = 0;
	setUpAttenuation();
	logIt(INFO, "Everything set up successfully. Starting simulation...");
	ret = simulation(pathToSlice, pathToOutputSinogram);
	reconstruction(pathToOutputSinogram, pathToOutputReconstruction);
	logIt(INFO, "Reconstructed image saved as %s. Exiting...", pathToOutputReconstruction);
	logIt(DEBUG, "main(int argc, char *argv[]) finished.");
	return ret;
}

void printhelp(){
	printf("USAGE: CTSim [pathToSlice] [pathToOutputReconstruction]\n");
}

