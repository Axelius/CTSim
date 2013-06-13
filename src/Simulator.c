/*
 ============================================================================
 Name        : Simulator.c
 Author      : Alexander Winkler
 Version     :
 Copyright   : Whatever you want to do with it
 Description : CT Artefact Simulation: Inputs a slice and outputs a sinogram
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <Math.h>
#include <limits.h>
#include <time.h>
#include "Logger.h"
#include "Run.h"

#define ROWS 512
#define COLS 512
#define SINOGRAMSIZE 512
#define PI 3.1415926535897932384626433832795028841971693993751058f
#define ATTF 1


//forward Declarations
int allocateRaw(int row, int col);
int loadPGMToRaw(FILE *data);
int allocateResult(int row, int col);
int project(int angle);
int exportPGM(FILE* out, unsigned int** write, int x, int y);
int freeRaw(int row, int col);

unsigned int **raw;
unsigned int **result;

int simulation(char *pathToSlice, char *pathToOutputSinogram) {
	logIt(INFO, "simulation(char *pathToSlice, char *pathToOutputSinogram) started.");
	printf("slicepath: %s\n", pathToSlice);
	printf("pathToOutputSinogram: %s\n", pathToOutputSinogram);
	fflush(stdout);
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

	logIt(INFO, "Starting Simulation");

	for(a = 0; a<NUM_ANGLES; a++){
		project(a);
	}


	freeRaw(ROWS,COLS);
	printf("projected\n");
	fflush(stdout);
	printf("exporting\n");
	fflush(stdout);
	printf("fileout: %p\n",  (outFile));
	fflush(stdout);




	exportPGM(outFile, result, NUM_ANGLES, SINOGRAMSIZE);

	time(&stop);
	run = difftime(stop, start);




	printf("Simulation finished\nRuntime:%lf\n", run);
	fclose(outFile);
	fclose(file);
	logIt(INFO, "simulation(char *pathToSlice, char *pathToOutputSinogram) finished.");
	return EXIT_SUCCESS;
}

int allocateRaw(int row, int col) {
	logIt(INFO, "allocateRaw(int row, int col) started.");
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
	logIt(INFO, "allocateRaw(int row, int col) finished.");
	return 0;
}

int freeRaw(int row, int col) {
	logIt(INFO, "freeRaw(int row, int col) started.");
	int i = 0;
	for(i = 0; i < row; i++) {
		free(raw[i]);
	}
	free(raw);
	logIt(INFO, "freeRaw(int row, int col) finished.");
	return 0;
}

int allocateResult(int row, int col) {
	logIt(INFO, "allocateResult(int row, int col) started.");
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
	logIt(INFO, "allocateResult(int row, int col) finished.");
	return 0;
}

int loadPGMToRaw(FILE *data){
	logIt(INFO, "loadPGMToRaw(FILE *data) started.");
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


	logIt(INFO, "loadPGMToRaw(FILE *data) finished.");
	return return_value;
}

int project(int angle){
	logIt(INFO, "project(int angle) started.");
	int t = 0;
	int x = 0;
	int y = 0;
	int count = angle;
	angle = angle - NUM_ANGLES/2;
	int s = 0;
	double alpha = (((double)(angle))/((double)NUM_ANGLES))*(PI);


	//	printf("alpha: %f\n",  (alpha));
	//	fflush(stdout);

	for(s = -SINOGRAMSIZE/2; s<SINOGRAMSIZE/2; s++){
		for(t = -COLS; t<COLS; t++){
			x = (int)(t*sin(alpha)+s*cos(alpha)+0.5+COLS/2);
			y = (int)(-t*cos(alpha)+s*sin(alpha)+0.5+COLS/2);
			if(x>=0 && x<COLS && y>=0 && y <COLS){
				//if(raw[x][y] == 255){
				//printf("Metal found\n");
				//fflush(stdout);
				//[count][s+SINOGRAMSIZE/2] = UINT_MAX/ATTF;
				//}
				//if(result[count][s+SINOGRAMSIZE/2] < UINT_MAX/ATTF){
				result[count][s+SINOGRAMSIZE/2] += raw[x][y];
				//}
			}
			//			if((int)(t*sin(alpha)+s*cos(alpha)+0.5)>=-COLS/2 && (int)(t*sin(alpha)+s*cos(alpha)+0.5)<COLS/2 && (int)(-t*cos(alpha)+s*sin(alpha)+0.5)>=-COLS/2 && (int)(-t*cos(alpha)+s*sin(alpha)+0.5)<COLS/2){
			//				result[count][s+SINOGRAMSIZE/2] += raw[(int)(t*sin(alpha)+s*cos(alpha)+0.5+COLS/2)][(int)(-t*cos(alpha)+s*sin(alpha)+0.5+COLS/2)];
			//			}
		}
	}
	logIt(INFO, "project(int angle) finished.");
	return 0;
}

int exportPGM(FILE* out, unsigned int** write, int x, int y){
	logIt(INFO, "exportPGM(FILE* out, unsigned int** write, int x, int y) started.");
	int i = 0;
	int j = 0;

	int min = write[0][0];
	int max = write[0][0];


	//Output as a picture file
	logIt(INFO, "Output as a picture file");
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




	printf("fileout: %p\n",  (out));
	fflush(stdout);
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
	logIt(INFO, "exportPGM(FILE* out, unsigned int** write, int x, int y) finished.");
	return 0;
}




