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

#define ROWS 512
#define COLS 512
#define MAXANGLES 100
#define SINOGRAMSIZE 256
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

int simulation(void) {

	time_t start;
	time_t stop;
	time(&start);
	double run = 0.0;

	int a = 0;
	(void)allocateRaw(ROWS,COLS);
	(void)allocateResult(MAXANGLES, SINOGRAMSIZE);
	FILE *file = fopen("slices/CT000029.pgm","r");
	FILE *outFile = fopen("o2.pgm", "wb");
	(void)loadPGMToRaw(file);
	printf("Starting Simulation\n");
	for(a = 0; a<MAXANGLES; a++){
		project(a);
	}
	freeRaw(ROWS,COLS);
	printf("projected\n");
	fflush(stdout);
	printf("exporting\n");
	fflush(stdout);


	exportPGM(outFile, result,MAXANGLES, SINOGRAMSIZE);
	time(&stop);
	run = difftime(stop, start);









	printf("Simulation finished\nRuntime:%lf\n", run);
	return EXIT_SUCCESS;
}

int allocateRaw(int row, int col) {
	int i = 0;
	raw = malloc(row * sizeof(int *));

	if(raw == 0){
		fprintf(stderr, "out of memory\n");
		return 1;
	}
	for(i = 0; i < row; i++) {
		raw[i] = malloc(col * sizeof(int));
		if(raw[i] == 0){
			fprintf(stderr, "out of memory\n");
			return 1;
		}
	}
	return 0;
}

int freeRaw(int row, int col) {
	int i = 0;
	for(i = 0; i < row; i++) {
		free(raw[i]);
	}
	free(raw);
	return 0;
}

int allocateResult(int row, int col) {
	int i = 0;
	int j = 0;
	result = malloc(row * sizeof(int *));

	if(result == 0){
		fprintf(stderr, "out of memory\n");
		return 1;
	}
	for(i = 0; i < row; i++) {
		result[i] = malloc(col * sizeof(int));
		if(raw[i] == 0){
			fprintf(stderr, "out of memory\n");
			return 1;
		}
		for(j = 0; j<col; j++){
			result[i][j] = 0;
		}
	}
	return 0;
}

int loadPGMToRaw(FILE *data){
	int return_value = 0;
	int i = 0;
	int j = 0;
	char str[200];
	int noOneCares = 0;


	//Read P2
	fgets(str, 200, data);
	if(!(str[0] == 'P' && str[1] == '2')){
		printf("Not a pgm!");
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



	return return_value;
}

int project(int angle){
	int t = 0;
	int x = 0;
	int y = 0;
	int count = angle;
	angle = angle - MAXANGLES/2;
	int s = 0;
	double alpha = (((double)(angle))/((double)MAXANGLES))*(PI);


//	printf("alpha: %f\n",  (alpha));
//	fflush(stdout);

	for(s = -SINOGRAMSIZE/2; s<SINOGRAMSIZE; s++){
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

	return 0;
}

int exportPGM(FILE* out, unsigned int** write, int x, int y){

	int i = 0;
	int j = 0;

	int min = write[0][0];
	int max = write[0][0];


	//Output as a picture file
	for(i = 0; i<x;i++){
		for(j = 0; j<y; j++){
			if(write[i][j]<min){
				min = write[i][j];
				//printf("newmin\n");
			}

			if(write[i][j]>max){
				max = write[i][j];
				//printf("newmax:%d\n", max);
			}
			//printf("i:%d,j:%d\n",i,j);
			//fflush(stdout);
		}
	}




	//printf("fileout: %p\n",  (out));
	//fflush(stdout);
	//FILE *res = fopen("result.pgm", "wb");
	printf("here?\n");
			fflush(stdout);
	fprintf(out, "P2\n# Created by Sim\n%d %d\n255\n", y, x);

	for(i = 0; i<x;i++){
		for(j = 0; j<y; j++){
			//printf("%d \n",(int)(((((double)write[i][j])-min)/((double)max-(double)min))*255.0));
			//fflush(stdout);
			fprintf(out,"%d ",(int)(((((double)write[i][j])-min)/((double)max-(double)min))*255.0));
		}
	}

	fclose(out);
	return 0;
}




