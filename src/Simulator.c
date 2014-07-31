/*
 ============================================================================
 Name        : Simulator.c
 Author      : Alexander Winkler
 Version     :
 Copyright   : Whatever you want to do with it
 Description : CT Artefact Simulation: Inputs a slice and outputs a sinogram
 ============================================================================
 */

#include "Simulator.h"




int simulation(char *pathToSlice, char *pathToOutputSinogram) {
	HANDLE *hThread;
	DWORD *threadID;
	t **arg;
	int i = 0;
	int j;
	FILE *outFile;


    LARGE_INTEGER frequency;
    LARGE_INTEGER t1, t2;
    double elapsedTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&t1);


	logIt(DEBUG, "simulation(char *pathToSlice, char *pathToOutputSinogram) started.");
	logIt(TRACE, "pathToOutputSinogram: %s", pathToOutputSinogram );
	logIt(TRACE, "pathToSlice: %s", pathToSlice);

	setUpRawFiles(pathToSlice);



	allocateAllRaws();
	allocateUnsignedIntArray(&result, cfg.numberOfProjectionAngles, SINOGRAMSIZE);

	outFile = fopen(pathToOutputSinogram, "wb");


	(void)loadPGMToRaw(&ironRaw,	ironImage);
	(void)loadPGMToRaw(&boneRaw,	boneImage);
	(void)loadPGMToRaw(&waterRaw,	waterImage);
	(void)loadPGMToRaw(&airRaw,		airImage);
	(void)loadPGMToRaw(&muscleRaw,	muscleImage);
	(void)loadPGMToRaw(&tissueRaw,	tissueImage);


	logIt(INFO, "Starting projection.");

	precalculatedPhotonCounts = malloc(cfg.energyLevels * sizeof(int));
	for(i = 0; i<cfg.energyLevels;i++){
		precalculatedPhotonCounts[i] = getPhotonCount(cfg.minEnergy + i * ((cfg.maxEnergy - cfg.minEnergy)/(cfg.energyLevels-1)));
		logIt(DEBUG, "precalculatedPhotonCounts[%d] = %d", i, precalculatedPhotonCounts[i]);
	}


	{

		hThread = (HANDLE *) malloc(cfg.numberOfThreads * sizeof(HANDLE));
		threadID = (DWORD *) malloc(cfg.numberOfThreads * sizeof(DWORD));
		arg = (t **) malloc(cfg.numberOfThreads * sizeof(t *));
		//Distribute angles on threads

		for(i = 0; i<cfg.numberOfThreads; i++){
			arg[i] = (t *)malloc(sizeof(t));
			arg[i]->startCount = (int)((double)cfg.numberOfProjectionAngles/(double)cfg.numberOfThreads + 0.5) * i;
			arg[i]->endCount = (int)((double)cfg.numberOfProjectionAngles/(double)cfg.numberOfThreads + 0.5) * i + (int)((double)cfg.numberOfProjectionAngles/(double)cfg.numberOfThreads + 0.5) - 1;
			if (i == cfg.numberOfThreads-1){//last thread has to do remaining projections
				arg[i]->endCount = cfg.numberOfProjectionAngles-1;
			}


			hThread[i] = (HANDLE) CreateThread(NULL, 0, projectFromTo, (void *) arg[i], 0, &threadID[i]);
			logIt(INFO, "Thread started: ThreadID: %d, from projection %d to %d.", i, arg[i]->startCount, arg[i]->endCount);

		}



		logIt(DEBUG, "Waiting for Threads to finish.");
		WaitForMultipleObjects(cfg.numberOfThreads, hThread, TRUE, INFINITE);

		for(i = 0; i<cfg.numberOfThreads; i++){
			CloseHandle(hThread[i]);
		}
	}




	//clamp sinogram
	for(i = 0; i<cfg.numberOfProjectionAngles;i++){
		for(j = 0; j<SINOGRAMSIZE; j++){
			logIt(TRACE, "result[%d][%d]=	%d", i, j, result[i][j]);
			if(result[i][j] > cfg.windowMax){
				logIt(TRACE, "Clamp at max");
				result[i][j] = cfg.windowMax;
			}
			if(result[i][j] < cfg.windowMin){
				logIt(TRACE, "Clamp at min");
				result[i][j] = cfg.windowMin;
			}
		}
	}




	exportPGM(outFile, result, cfg.numberOfProjectionAngles, SINOGRAMSIZE);
	freeAllRaws();
	QueryPerformanceCounter(&t2);
	elapsedTime = (double)(t2.QuadPart - t1.QuadPart) / frequency.QuadPart;


	logIt(INFO,  "Projection finished. Runtime: %f seconds.", elapsedTime);
	fclose(outFile);
	closeAllInputImages();



	logIt(DEBUG, "simulation(char *pathToSlice, char *pathToOutputSinogram) finished.");
	return EXIT_SUCCESS;
}

DWORD WINAPI projectFromTo(void * param){

	t *args = (t*) param;
	double angle = 0;
	int fromProjectionCount = args->startCount;
	int toProjectionCount = args->endCount;
	logIt(DEBUG, "projectFromTo(void *param) started.");
	for(;fromProjectionCount<=toProjectionCount; fromProjectionCount++){
		angle = (double) fromProjectionCount * 180.0 / (double)cfg.numberOfProjectionAngles;
		project(fromProjectionCount, angle);
	}
	logIt(DEBUG, "projectFromTo(void *param) finished.");
	return 0;
}

void allocateUnsignedIntArray(unsigned int ***raw, unsigned int row, unsigned int col) {
	int i = 0;
	logIt(DEBUG, "allocateUnsignedIntArray(unsigned int ***raw, int row, int col) started.");


	*raw = malloc(row * sizeof(unsigned int *));

	if(*raw == 0){
		logIt(ERR, "out of memory");
		fprintf(stderr, "out of memory\n");
		return;
	}

	for(i = 0; i < row; i++) {
		(*raw)[i] = calloc(col, sizeof(unsigned int));
		if((*raw)[i] == 0){
			logIt(ERR, "out of memory in inner loop");
			fprintf(stderr, "out of memory\n");
			return;
		}
	}

	logIt(DEBUG, "allocateUnsignedIntArray(unsigned int ***raw, int row, int col) finished.");
}


void freeUnsignedIntArray(unsigned int ***raw, int row, int col) {
	int i = 0;
	logIt(DEBUG, "freeUnsignedIntArray(unsigned int ***raw, int row, int col) started.");

	for(i = 0; i < row; i++) {
		free((*raw)[i]);
	}
	free(*raw);
	logIt(DEBUG, "freeUnsignedIntArray(unsigned int ***raw, int row, int col) finished.");
}



int project(int projectionNumber, double angle){
	int t = 0;
	int x = 0;
	int y = 0;
	int s = 0;
	int mat = 0;
	int energyLoopCount = 0;
	double *intensity;
	double sinAngle = 0.0;
	double cosAngle = 0.0;
	int energyLevel = cfg.minEnergy;

	intensity = calloc(SINOGRAMSIZE, sizeof(double));
	logIt(DEBUG, "project(int projectionNumber=%d, double angle=%f) started.", projectionNumber, angle);
	LARGE_INTEGER frequency;
	LARGE_INTEGER t1, t2;
	double elapsedTime;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);
	angle = (PI/180.0)*(angle-90); //convert from degree to radians and rotate by 90 degrees
	sinAngle = sin(angle);
	cosAngle = cos(angle);



	for(energyLoopCount = 0; energyLoopCount < cfg.energyLevels; energyLevel = cfg.minEnergy + ((cfg.maxEnergy-cfg.minEnergy)/(cfg.energyLevels-1))*(++energyLoopCount)){
		if(precalculatedPhotonCounts[energyLoopCount] == 0){
			continue;
		}
		//logIt(DEBUG, "loop%d, energy=%d, photonCount=%d", energyLoopCount, energyLevel, photonCount);

		for(s = -SINOGRAMSIZE/2; s < SINOGRAMSIZE/2; s++){//s=detector element on line
			intensity[s+SINOGRAMSIZE/2] = (double)precalculatedPhotonCounts[energyLoopCount];
			logIt(TRACE, "intensity[count][s+SINOGRAMSIZE/2] = %f", intensity[s+SINOGRAMSIZE/2]);
			for(t = -COLS; t < COLS; t++){//t=ray length
				x = (int)( t*sinAngle + s*cosAngle + 0.0 + COLS/2);
				y = (int)(-t*cosAngle + s*sinAngle + 0.0 + COLS/2);
				if(x >= 0 && x < COLS && y >= 0 && y < COLS){
					double accumulatedAttenuationForPixel = 0.0;
					for(mat = MINMAT; mat <= MAXMAT; mat++){
						accumulatedAttenuationForPixel += SIZEOFPIXEL * getAttenuation(mat, energyLevel, x, y);
						logIt(TRACE, "intensity[%d][%d] =%f", projectionNumber, s+SINOGRAMSIZE/2, intensity[s+SINOGRAMSIZE/2]);
					}//loop over mats finished
					intensity[s+SINOGRAMSIZE/2] = intensity[s+SINOGRAMSIZE/2] * pow(EULER, -accumulatedAttenuationForPixel*cfg.attenuationMultiplicator);
					//logIt(DEBUG, "energy: %d, intensity: %f",energyLevel, intensity[count][s+SINOGRAMSIZE/2]);
					if(intensity[s+SINOGRAMSIZE/2] <= cfg.detectorThreshold){
						intensity[s+SINOGRAMSIZE/2] = cfg.detectorThreshold;
						//logIt(DEBUG, "Photon beam starved.");
						//break;
					}
				}
			}
			result[projectionNumber][s+SINOGRAMSIZE/2] += (unsigned int)(intensity[s+SINOGRAMSIZE/2]);
			//logIt(DEBUG, "result: %u", result[count][s+SINOGRAMSIZE/2]);
		}
	}
	free(intensity);
	QueryPerformanceCounter(&t2);
	elapsedTime = (double)(t2.QuadPart - t1.QuadPart) / frequency.QuadPart;
	logIt(INFO, "Time for projection %d: %f seconds. For %d projections this is %f seconds total using %d threads.", projectionNumber, elapsedTime, cfg.numberOfProjectionAngles, (elapsedTime) * cfg.numberOfProjectionAngles / (double) cfg.numberOfThreads, cfg.numberOfThreads);
	logIt(DEBUG, "project(int projectionNumber, double angle) finished.");
	return 0;
}



void setUpRawFiles(char *pathToSlices){
	char paths[256];
	logIt(DEBUG, "setUpRawFiles(char *pathToSlices) started.");


	strcpy(paths, pathToSlices);
	strcat(paths, "/air.pgm");
	logIt(TRACE, "Path to air: %s", paths);
	airImage = fopen(paths,"r");

	strcpy(paths, pathToSlices);
	strcat(paths, "/bone.pgm");
	logIt(TRACE, "Path to bone: %s", paths);
	boneImage = fopen(paths,"r");

	strcpy(paths, pathToSlices);
	strcat(paths, "/iron.pgm");
	logIt(TRACE, "Path to iron: %s", paths);
	ironImage = fopen(paths,"r");

	strcpy(paths, pathToSlices);
	strcat(paths, "/muscle.pgm");
	logIt(TRACE, "Path to muscle: %s", paths);
	muscleImage = fopen(paths,"r");

	strcpy(paths, pathToSlices);
	strcat(paths, "/tissue.pgm");
	logIt(TRACE, "Path to tissue: %s", paths);
	tissueImage = fopen(paths,"r");

	strcpy(paths, pathToSlices);
	strcat(paths, "/water.pgm");
	logIt(TRACE, "Path to water: %s", paths);
	waterImage = fopen(paths,"r");

	logIt(DEBUG, "setUpRawFiles(char *pathToSlices) finished.");
}

void allocateAllRaws() {
	logIt(DEBUG, "allocateAllRaws()  started.");
	(void) allocateUnsignedIntArray(&ironRaw, ROWS, COLS);
	(void) allocateUnsignedIntArray(&boneRaw, ROWS, COLS);
	(void) allocateUnsignedIntArray(&waterRaw, ROWS, COLS);
	(void) allocateUnsignedIntArray(&airRaw, ROWS, COLS);
	(void) allocateUnsignedIntArray(&muscleRaw, ROWS, COLS);
	(void) allocateUnsignedIntArray(&tissueRaw, ROWS, COLS);
	logIt(DEBUG, "allocateAllRaws()  finished.");
}

void freeAllRaws() {
	logIt(DEBUG, "freeAllRaws()  started.");
	freeUnsignedIntArray(&ironRaw, ROWS, COLS);
	freeUnsignedIntArray(&boneRaw, ROWS, COLS);
	freeUnsignedIntArray(&waterRaw, ROWS, COLS);
	freeUnsignedIntArray(&airRaw, ROWS, COLS);
	freeUnsignedIntArray(&muscleRaw, ROWS, COLS);
	freeUnsignedIntArray(&tissueRaw, ROWS, COLS);
	logIt(DEBUG, "freeAllRaws()  finished.");
}

void closeAllInputImages() {
	logIt(DEBUG, "closeAllInputImages()  started.");
	fclose(airImage);
	fclose(boneImage);
	fclose(ironImage);
	fclose(muscleImage);
	fclose(tissueImage);
	fclose(waterImage);
	logIt(DEBUG, "closeAllInputImages()  finished.");
}
