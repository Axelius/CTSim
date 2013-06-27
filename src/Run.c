/*
 * Run.c
 *
 *  Created on: 06.06.2013
 *      Author: Alexander
 */
#include "Run.h"


int main(int argc, char *argv[]){
	if(argc != 3){
		printhelp();
		printf("argv[0]=%s\n",argv[0]);
		logIt(ERROR, "Unsupported number of arguments.");
		return 1;
	}
	char pathToSlice[200];
	char pathToOutputReconstruction[200];
	char pathToOutputSinogram[] = "simulatedSinogram.pgm";
	char msg[200];

	sprintf(pathToSlice, argv[1]);
	//pathToSlice = argv[1]; //"slices/CT000029.pgm";
	sprintf(pathToOutputReconstruction, argv[2]);
	//"outData.pgm";
	logIt(DEBUG, "pathToSlice=%s", pathToSlice);
	logIt(DEBUG, "pathToOutputReconstruction=%s", pathToOutputReconstruction);

	int ret = 0;
	setUpAttenuation();
	ret = simulation(pathToSlice, pathToOutputSinogram);
	reconstruction(pathToOutputSinogram, pathToOutputReconstruction);
	return ret;
}

void printhelp(){
	printf("USAGE: CTSim [pathToSlice] [pathToOutputReconstruction]\n");
}

