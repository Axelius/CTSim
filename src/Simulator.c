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



unsigned int **raw;
unsigned int **result;


int simulation(char *pathToSlice, char *pathToOutputSinogram) {
	logIt(DEBUG, "simulation(char *pathToSlice, char *pathToOutputSinogram) started.");
	logIt(INFO, "pathToOutputSinogram: %s", pathToOutputSinogram );
	logIt(INFO, "pathToSlice: %s", pathToSlice);
	time_t start;
	time_t stop;
	time(&start);
	double run = 0.0;



	int a = 0;
	(void)allocateRaw(ROWS,COLS);
	(void)allocateResult(NUM_ANGLES, SINOGRAMSIZE);
	FILE *file = fopen(pathToSlice,"r");
	FILE *outFile = fopen(pathToOutputSinogram, "wb");


	(void)loadPGMToRaw(file);

	logIt(INFO, "Starting Simulation.");

	logIt(INFO, "Starting projection.");
	for(a = 0; a<NUM_ANGLES; a++){
		project(a);
	}
	logIt(INFO, "Projection finished.");


	freeRaw(ROWS,COLS);


	exportPGM(outFile, result, NUM_ANGLES, SINOGRAMSIZE);

	time(&stop);
	run = difftime(stop, start);


	logIt(INFO,  "Simulation finished. Runtime: %lf.", run);
	fclose(outFile);
	fclose(file);
	logIt(DEBUG, "simulation(char *pathToSlice, char *pathToOutputSinogram) finished.");
	return EXIT_SUCCESS;
}

int allocateRaw(int row, int col) {
	logIt(DEBUG, "allocateRaw(int row, int col) started.");
	int i = 0;
	raw = malloc(row * sizeof(int *));

	if(raw == 0){
		logIt(ERROR, "out of memory");
		fprintf(stderr, "out of memory\n");
		return 1;
	}
	for(i = 0; i < row; i++) {
		raw[i] = malloc(col * sizeof(int));
		if(raw[i] == 0){
			logIt(ERROR, "out of memory in inner loop");
			fprintf(stderr, "out of memory\n");
			return 1;
		}
	}
	logIt(DEBUG, "allocateRaw(int row, int col) finished.");
	return 0;
}

int freeRaw(int row, int col) {
	logIt(DEBUG, "freeRaw(int row, int col) started.");
	int i = 0;
	for(i = 0; i < row; i++) {
		free(raw[i]);
	}
	free(raw);
	logIt(DEBUG, "freeRaw(int row, int col) finished.");
	return 0;
}

int allocateResult(int row, int col) {
	logIt(DEBUG, "allocateResult(int row, int col) started.");
	int i = 0;
	int j = 0;
	result = malloc(row * sizeof(int *));

	if(result == 0){
		logIt(ERROR, "out of memory");
		fprintf(stderr, "out of memory\n");
		return 1;
	}
	for(i = 0; i < row; i++) {
		result[i] = malloc(col * sizeof(int));
		if(raw[i] == 0){
			logIt(ERROR, "out of memory in inner loop");
			fprintf(stderr, "out of memory\n");
			return 1;
		}
		for(j = 0; j<col; j++){
			result[i][j] = 0;
		}
	}
	logIt(DEBUG, "allocateResult(int row, int col) finished.");
	return 0;
}

int loadPGMToRaw(FILE *data){
	logIt(DEBUG, "loadPGMToRaw(FILE *data) started.");
	int return_value = 0;
	int i = 0;
	int j = 0;
	char str[200];
	int noOneCares = 0;


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
			fscanf(data,"%d",&(raw[i][j]));
		}

	}


	logIt(DEBUG, "loadPGMToRaw(FILE *data) finished.");
	return return_value;
}

int project(int angle){
	logIt(DEBUG, "project(int angle) started.");
	int t = 0;
	int x = 0;
	int y = 0;
	int count = angle;
	angle = angle - NUM_ANGLES/2;
	int s = 0;
	double alpha = (((double)(angle))/((double)NUM_ANGLES))*(PI);


	//	printf("alpha: %f\n",  (alpha));
	//	fflush(stdout);
	//TODO: Loop over all materials
	for(s = -SINOGRAMSIZE/2; s<SINOGRAMSIZE/2; s++){
		for(t = -COLS; t<COLS; t++){
			x = (int)(t*sin(alpha)+s*cos(alpha)+0.5+COLS/2);
			y = (int)(-t*cos(alpha)+s*sin(alpha)+0.5+COLS/2);
			if(x>=0 && x<COLS && y>=0 && y <COLS){
				//TODO: Loop over all x ray intensities
				if(raw[x][y] == 255){
					logIt(DEBUG, "Metal found.");
					result[count][s+SINOGRAMSIZE/2] = UINT_MAX/ATTF;
				}
				else {
					//if(result[count][s+SINOGRAMSIZE/2] != UINT_MAX/ATTF){
					result[count][s+SINOGRAMSIZE/2] += raw[x][y];
				}
			}
			//			if((int)(t*sin(alpha)+s*cos(alpha)+0.5)>=-COLS/2 && (int)(t*sin(alpha)+s*cos(alpha)+0.5)<COLS/2 && (int)(-t*cos(alpha)+s*sin(alpha)+0.5)>=-COLS/2 && (int)(-t*cos(alpha)+s*sin(alpha)+0.5)<COLS/2){
			//				result[count][s+SINOGRAMSIZE/2] += raw[(int)(t*sin(alpha)+s*cos(alpha)+0.5+COLS/2)][(int)(-t*cos(alpha)+s*sin(alpha)+0.5+COLS/2)];
			//			}
		}
	}
	logIt(DEBUG, "project(int angle) finished.");
	return 0;
}

int exportPGM(FILE* out, unsigned int** write, int x, int y){
	logIt(DEBUG, "exportPGM(FILE* out, unsigned int** write, int x, int y) started.");
	int i = 0;
	int j = 0;

	int min = write[0][0];
	int max = write[0][0];


	//Output as a picture file
	logIt(TRACE, "Output as a picture file");
	for(i = 0; i<x;i++){
		for(j = 0; j<y; j++){

			//printf("i:%d,j:%d\n",i,j);
			//fflush(stdout);
			if(write[i][j]<min){
				logIt(TRACE, "New min found");
				min = write[i][j];
				//printf("newmin\n");
			}

			if(write[i][j]>max){
				logIt(TRACE, "New max found");
				max = write[i][j];
				//printf("newmax:%d\n", max);
			}

		}
	}



	//FILE *res = fopen("result.pgm", "wb");
	fprintf(out, "P2\n# Created by Sim\n%d %d\n255\n", y, x);
	//fprintf(out, "P2\n# Created by Sim\n%d %d\n255\n", y, x);

	for(i = 0; i<x;i++){
		for(j = 0; j<y; j++){
			//printf("%d \n",(int)(((((double)write[i][j])-min)/((double)max-(double)min))*255.0));
			//fflush(stdout);
			fprintf(out,"%d ",(int)(((((double)write[i][j])-min)/((double)max-(double)min))*255.0));
		}
	}

	fclose(out);
	logIt(DEBUG, "exportPGM(FILE* out, unsigned int** write, int x, int y) finished.");
	return 0;
}

int getAttenuation(int material, int kV, int positionX, int positionY){

}

void setUpAttenuation(){
	logIt(DEBUG, "setUpAttenuation() started.");
	readAttenuationFile("MassAttenuationCoefficients/iron.txt", &iron, &ironLength);
	readAttenuationFile("MassAttenuationCoefficients/boneCortical.txt", &bone, &boneLength);
	readAttenuationFile("MassAttenuationCoefficients/water.txt", &water, &waterLength);
	readAttenuationFile("MassAttenuationCoefficients/air.txt", &air, &airLength);
	readAttenuationFile("MassAttenuationCoefficients/muscleSceletal.txt", &muscle, &muscleLength);
	readAttenuationFile("MassAttenuationCoefficients/tissueSoft.txt", &tissue, &tissueLength);
	int i = 0;
	if(LOGLEVEL == TRACE){
		for(i = 0; i<ironLength; i++){
			logIt(TRACE, "iron[%d]: iron.energy: %f, iron.mu: %f\n", i, iron[i].energy, iron[i].mu);
		}
		for(i = 0; i<boneLength; i++){
			logIt(TRACE, "bone[%d]: bone.energy: %f, bone.mu: %f\n", i, bone[i].energy, bone[i].mu);
		}

		for(i = 0; i<waterLength; i++){
			logIt(TRACE, "water[%d]: water.energy: %f, water.mu: %f\n", i, water[i].energy, water[i].mu);
		}

		for(i = 0; i<airLength; i++){
			logIt(TRACE, "air[%d]: air.energy: %f, air.mu: %f\n", i, air[i].energy, air[i].mu);
		}

		for(i = 0; i<muscleLength; i++){
			logIt(TRACE, "muscle[%d]: muscle.energy: %f, muscle.mu: %f\n", i, muscle[i].energy, muscle[i].mu);
		}

		for(i = 0; i<tissueLength; i++){
			logIt(TRACE, "tissue[%d]: tissue.energy: %f, tissue.mu: %f\n", i, tissue[i].energy, tissue[i].mu);
		}
	}




	logIt(DEBUG, "setUpAttenuation() finished.");
}

void readAttenuationFile(char* pathToAttFile, attenuation** att, size_t* attLength){
	logIt(DEBUG, "readAttenuationFile(char* pathToAttFile, attenuation** att, size_t* attLength) started.");
	char line[200];
	int i = 0;
	int numAlloc;
	FILE* read = fopen(pathToAttFile, "r");
	while(fgets(line, 200, read) != NULL){
		logIt(TRACE, "Line%d: %s", i, line);
		i++;
	}
	logIt(TRACE, "%d lines found.", i);
	numAlloc = i-6;
	*attLength = i-6;
	*att = malloc(numAlloc * sizeof(attenuation));
	if(att == 0){
		logIt(ERROR, "out of memory");
	}
	logIt(TRACE,  "Allocated memory for %d attenuation elements.", numAlloc);
	fclose(read);
	read = fopen(pathToAttFile, "r");
	logIt(TRACE, "File reopened");
	i = 0;
	//read the header of the file and discard it
	fgets(line, 200, read);
	fgets(line, 200, read);
	fgets(line, 200, read);
	fgets(line, 200, read);

	while(i< numAlloc && fgets(line, 200, read) != NULL){
		fscanf(read, "%E  %E", &(((*att)[i]).energy), &(((*att)[i]).mu));
		logIt(TRACE, "att[%d]: energy: %f, mu: %f\n",i, (((*att)[i]).energy), (((*att)[i]).mu));
		i++;
	}
	//attenuation from the file should now be in the attenuation struct
	fclose(read);
	logIt(DEBUG, "readAttenuationFile(char* pathToAttFile, attenuation** att, size_t* attLength) finished.");
}




