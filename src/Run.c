/*
 * Run.c
 *
 *  Created on: 06.06.2013
 *      Author: Alexander
 */
#include "Logger.h"
#include "Run.h"


int main(int argc, char *argv[]){
	char pathToSlice[] = "slices/CT000029.pgm";
	char pathToOutputSinogram[] = "o2.pgm";
	char pathToOutputReconstruction[] = "outData.pgm";

	int ret = 0;
	ret = simulation(pathToSlice, pathToOutputSinogram);
	reconstruction(pathToOutputSinogram, pathToOutputReconstruction);
	return ret;
}

