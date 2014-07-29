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

	time_t start;
	time_t stop;

	HANDLE *hThread;
	DWORD *threadID;
	t **arg;
	int startvalue = 0;
	int endvalue = 0;
	int i = 0;
	int j;
	FILE *outFile;
	time(&start);
	logIt(DEBUG, "simulation(char *pathToSlice, char *pathToOutputSinogram) started.");
	logIt(TRACE, "pathToOutputSinogram: %s", pathToOutputSinogram );
	logIt(TRACE, "pathToSlice: %s", pathToSlice);

	setUpRawFiles(pathToSlice);





	allocateAllRaws();
	allocateUnsignedIntArray(&result, cfg.numberOfProjectionAngles, SINOGRAMSIZE);
	allocateDoubleArray(&intensity, cfg.numberOfProjectionAngles, SINOGRAMSIZE);
	outFile = fopen(pathToOutputSinogram, "wb");


	(void)loadPGMToRaw(&ironRaw,	ironImage);
	(void)loadPGMToRaw(&boneRaw,	boneImage);
	(void)loadPGMToRaw(&waterRaw,	waterImage);
	(void)loadPGMToRaw(&airRaw,		airImage);
	(void)loadPGMToRaw(&muscleRaw,	muscleImage);
	(void)loadPGMToRaw(&tissueRaw,	tissueImage);


	logIt(INFO, "Starting simulation.");

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

		startvalue = 0;
		endvalue = (cfg.numberOfProjectionAngles/cfg.numberOfThreads);
		for(i = 0; i<cfg.numberOfThreads; i++){
			arg[i] = (t *)malloc(sizeof(t));
			arg[i]->data1 = startvalue;
			if(i != cfg.numberOfThreads-1){
				arg[i]->data2 = endvalue;
			}
			else{
				arg[i]->data2 = cfg.numberOfProjectionAngles;
			}

			hThread[i] = (HANDLE) CreateThread(NULL, 0, projectFromTo, (void *) arg[i], 0, &threadID[i]);
			logIt(DEBUG, "Thread started: ThreadID: %d, from angle %d to %d.", i, arg[i]->data1, arg[i]->data2);
			startvalue += (cfg.numberOfProjectionAngles/cfg.numberOfThreads);
			endvalue += (cfg.numberOfProjectionAngles/cfg.numberOfThreads);

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
	time(&stop);


	logIt(INFO,  "Simulation finished. Runtime: %lf.", difftime(stop, start));
	fclose(outFile);
	closeAllInputImages();



	logIt(DEBUG, "simulation(char *pathToSlice, char *pathToOutputSinogram) finished.");
	return EXIT_SUCCESS;
}

DWORD WINAPI projectFromTo(void * param){

	t *args = (t*) param;
	int fromAngle = args->data1;
	int toAngle = args->data2;
	logIt(DEBUG, "projectFromTo(void *param) started.");
	for(;fromAngle<toAngle; fromAngle++){
		project(fromAngle);
	}
	logIt(DEBUG, "projectFromTo(void *param) finished.");
	return 0;
}

void allocateUnsignedIntArray(unsigned int ***raw, unsigned int row, unsigned int col) {
	int i = 0;
	logIt(DEBUG, "allocateUnsignedIntArray(unsigned int ***raw, int row, int col) started.");


	*raw = malloc(row * sizeof(unsigned int *));

	if(*raw == 0){
		logIt(ERROR, "out of memory");
		fprintf(stderr, "out of memory\n");
		return;
	}

	for(i = 0; i < row; i++) {
		(*raw)[i] = calloc(col, sizeof(unsigned int));
		if((*raw)[i] == 0){
			logIt(ERROR, "out of memory in inner loop");
			fprintf(stderr, "out of memory\n");
			return;
		}
	}

	logIt(DEBUG, "allocateUnsignedIntArray(unsigned int ***raw, int row, int col) finished.");
}

void allocateDoubleArray(double ***raw, unsigned int row, unsigned int col) {
	int i = 0;
	logIt(DEBUG, "allocateDoubleIntArray(double ***raw, int row, int col) started.");


	*raw = malloc(row * sizeof(double *));

	if(*raw == 0){
		logIt(ERROR, "out of memory");
		fprintf(stderr, "out of memory\n");
		return;
	}

	for(i = 0; i < row; i++) {
		(*raw)[i] = calloc(col, sizeof(double));
		if((*raw)[i] == 0){
			logIt(ERROR, "out of memory in inner loop");
			fprintf(stderr, "out of memory\n");
			return;
		}
	}

	logIt(DEBUG, "allocateDoubleIntArray(double int ***raw, int row, int col) finished.");
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



int loadPGMToRaw(unsigned int ***raw, FILE *data){

	int return_value = 0;
	int i = 0;
	int j = 0;
	char str[200];
	int noOneCares = 0;
	logIt(DEBUG, "loadPGMToRaw(unsigned int ***raw, FILE *data) started.");


	logIt(TRACE, "here??");
	//Read P2
	fgets(str, 200, data);
	if(!(str[0] == 'P' && str[1] == '2')){
		logIt(ERROR, "Not a pgm.");
		return 1;
	}


	fgets(str, 200, data); //Hopefully a commentary
	fscanf(data,"%d",&noOneCares);
	fscanf(data,"%d",&noOneCares);
	fgets(str, 200, data);

	fgets(str, 200, data);//colordepth, we dont care about



	for(i=0; i < ROWS; i++){
		for(j = 0; j<COLS; j++){
			fscanf(data,"%u",&((*raw)[i][j]));
		}

	}




	logIt(DEBUG, "loadPGMToRaw(unsigned int ***raw, FILE *data) finished.");
	return return_value;
}

int project(int angle){
	time_t projectionStartTime;
	time_t projectionEndTime;
	double alpha;
	int t = 0;
	int x = 0;
	int y = 0;
	int count = angle;
	int s = 0;
	int mat = 0;
	int energyLevel = cfg.minEnergy;
	int energyLoopCount = 0;

	logIt(DEBUG, "project(int angle=%d) started.", angle);
	time(&projectionStartTime);
	angle = angle - cfg.numberOfProjectionAngles/2;
	alpha = (((double)(angle))/((double)cfg.numberOfProjectionAngles))*(PI);



	for(energyLoopCount = 0; energyLoopCount < cfg.energyLevels; energyLevel = cfg.minEnergy + ((cfg.maxEnergy-cfg.minEnergy)/(cfg.energyLevels-1))*(++energyLoopCount)){
		if(precalculatedPhotonCounts[energyLoopCount] == 0){
			continue;
		}
		//logIt(DEBUG, "loop%d, energy=%d, photonCount=%d", energyLoopCount, energyLevel, photonCount);

		for(s = -SINOGRAMSIZE/2; s < SINOGRAMSIZE/2; s++){//s=detector element on line
			intensity[count][s+SINOGRAMSIZE/2] = (double)precalculatedPhotonCounts[energyLoopCount];
			logIt(TRACE, "intensity[count][s+SINOGRAMSIZE/2] = %f", intensity[count][s+SINOGRAMSIZE/2]);
			for(t = -COLS; t < COLS; t++){//t=ray length
				x = (int)( t*sin(alpha) + s*cos(alpha) + 0.0 + COLS/2);
				y = (int)(-t*cos(alpha) + s*sin(alpha) + 0.0 + COLS/2);
				if(x >= 0 && x < COLS && y >= 0 && y < COLS){
					double accumulatedAttenuationForPixel = 0;
					for(mat = MINMAT; mat <= MAXMAT; mat++){
						accumulatedAttenuationForPixel += SIZEOFPIXEL * getAttenuation(mat, energyLevel, x, y);
						//logIt(DEBUG, "intens: %f", intensity[count][s+SINOGRAMSIZE/2]);
						//result[count][s+SINOGRAMSIZE/2] +=  ((double)precalculatedPhotonCounts[energyLoopCount]) *(intensityBackup * temp);
						logIt(TRACE, "intensity[%d][%d] =%f",count, s+SINOGRAMSIZE/2, intensity[count][s+SINOGRAMSIZE/2]);
					}//loop over mats finished
					//logIt(DEBUG, "accPixelAtt: %f", accumulatedAttenuationForPixel);
					intensity[count][s+SINOGRAMSIZE/2] = intensity[count][s+SINOGRAMSIZE/2] * pow(EULER, -accumulatedAttenuationForPixel);
					//logIt(DEBUG, "intensity: %f", intensity[count][s+SINOGRAMSIZE/2]);
					if(intensity[count][s+SINOGRAMSIZE/2] <= 6000){
						intensity[count][s+SINOGRAMSIZE/2] = 6000.0;
						logIt(TRACE, "Photon beam starved.");
						break;
					}

				}

			}
			result[count][s+SINOGRAMSIZE/2] += (unsigned int)(intensity[count][s+SINOGRAMSIZE/2] * energyLevel);
			//logIt(DEBUG, "result: %u", result[count][s+SINOGRAMSIZE/2]);
		}
	}

	time(&projectionEndTime);
	logIt(DEBUG, "Time for one projection: %lf seconds. For %d projections this is %f seconds total using %d threads.", (difftime(projectionEndTime, projectionStartTime)), cfg.numberOfProjectionAngles, ((double) difftime(projectionEndTime, projectionStartTime)) * cfg.numberOfProjectionAngles / (double) cfg.numberOfThreads, cfg.numberOfThreads);
	logIt(DEBUG, "project(int angle) finished.");
	return 0;
}

int exportPGM(FILE* out, unsigned int** write, int x, int y){
	int i = 0;
	int j = 0;
	unsigned int min;
	unsigned int max;
	logIt(DEBUG, "exportPGM(FILE* out, unsigned int** write, int x, int y) started.");
	min = write[0][0];
	max = write[0][0];


	//Output as a picture file
	logIt(TRACE, "Output as a picture file");
	for(i = 0; i<x;i++){
		for(j = 0; j<y; j++){
			if(write[i][j]<min){
				logIt(TRACE, "New min found");
				min = write[i][j];
			}
			if(write[i][j]>max){
				logIt(TRACE, "New max found");
				max = write[i][j];
			}
		}
	}

	fprintf(out, "P2\n# Created by Sim\n%d %d\n255\n", y, x);

	for(i = 0; i<x;i++){
		for(j = 0; j<y; j++){
			//logIt(DEBUG, "(int)(((((double)write[i][j])-min)/((double)max-(double)min))*255.0))=%d", (int)(((((double)write[i][j])-min)/((double)max-(double)min))*255.0));
			fprintf(out,"%d ",(int)(((((double)write[i][j])-min)/((double)max-(double)min))*255.0));
		}
	}

	fclose(out);
	logIt(DEBUG, "exportPGM(FILE* out, unsigned int** write, int x, int y) finished.");
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

