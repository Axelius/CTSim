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
	double run = 0.0;
	int startvalue = 0;
	int endvalue = 0;
	int i = 0;
	FILE *outFile;
	time(&start);
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


	logIt(INFO, "Starting simulation.");

	logIt(INFO, "Starting projection.");
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
		logIt(DEBUG, "Thread started: ThreadID: %d, from  angle %d to %d.", i, arg[i]->data1, arg[i]->data2);
		startvalue += (cfg.numberOfProjectionAngles/cfg.numberOfThreads);
		endvalue += (cfg.numberOfProjectionAngles/cfg.numberOfThreads);

	}



	logIt(DEBUG, "Waiting for Threads to finish.");
	WaitForMultipleObjects(cfg.numberOfThreads,hThread,TRUE,INFINITE);

	for(i = 0; i<cfg.numberOfThreads; i++){
		CloseHandle(hThread[i]);
	}

//	int a = 0;
//		for(a = 0; a<cfg.numberOfProjectionAngles; a++){
//			project(a);
//			logIt(INFO, "Projection %d of %d finished.", a, cfg.numberOfProjectionAngles);
//		}
//	logIt(INFO, "Projection completed.");


	freeAllRaws();
	exportPGM(outFile, result, cfg.numberOfProjectionAngles, SINOGRAMSIZE);

	time(&stop);
	run = difftime(stop, start);


	logIt(INFO,  "Simulation finished. Runtime: %lf.", run);
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
		(*raw)[i] = malloc(col * sizeof(unsigned int));
		if((*raw)[i] == 0){
			logIt(ERROR, "out of memory in inner loop");
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
	int t = 0;
	int x = 0;
	int y = 0;
	int count = angle;
	int s = 0;
	int mat = 0;
	double energy = cfg.minEnergy;
	double alpha = (((double)(angle))/((double)cfg.numberOfProjectionAngles))*(PI);

	logIt(DEBUG, "project(int angle) started.");
	angle = angle - cfg.numberOfProjectionAngles/2;

	for(s = -SINOGRAMSIZE/2; s<SINOGRAMSIZE/2; s++){
		for(t = -COLS; t<COLS; t++){
			x = (int)(t*sin(alpha)+s*cos(alpha)+0.5+COLS/2);
			y = (int)(-t*cos(alpha)+s*sin(alpha)+0.5+COLS/2);
			if(x>=0 && x<COLS && y>=0 && y <COLS){
				for(energy = cfg.minEnergy; energy<=cfg.maxEnergy; energy+=((cfg.maxEnergy-cfg.minEnergy)/cfg.energyLevels)){
					for(mat = MINMAT; mat< MAXMAT; mat++){
						result[count][s+SINOGRAMSIZE/2] += getAttenuation(mat, energy, x,y);

						//logIt(FATAL, "result[%d][%d] += getAttenuation(mat = %d, energy = %f, x = %d,y=%d) =	%d", count, s+SINOGRAMSIZE/2, mat, energy, x,y, getAttenuation(mat, energy, x,y));
					}
					if(energy-cfg.maxEnergy < 0.0001){
						break;
					}
				}
			}
		}
	}
	logIt(DEBUG, "project(int angle) finished.");
	return 0;
}

int exportPGM(FILE* out, unsigned int** write, int x, int y){
	int i = 0;
	int j = 0;
	unsigned int min = write[0][0];
	unsigned int max = write[0][0];
	logIt(DEBUG, "exportPGM(FILE* out, unsigned int** write, int x, int y) started.");


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

