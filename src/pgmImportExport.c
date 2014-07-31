/*
 * pgmImportExport.c
 *
 *  Created on: 31.07.2014
 *      Author: Alexander
 */

#include "pgmImportExport.h"



int exportPGM(FILE* out, unsigned int** write, int x, int y){
	int i = 0;
	int j = 0;
	unsigned int min;
	unsigned int max;
	logIt(DEBUG, "exportPGM(FILE* out, unsigned int** write, int x, int y) started.");
	min = write[0][0];
	max = write[0][0];

	//FInd maximum and minumum for image normalization
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

int loadPGMToRaw(unsigned int ***raw, FILE *data){

	int return_value = 0;
	int i = 0;
	int j = 0;
	char str[200];
	int noOneCares = 0;
	logIt(DEBUG, "loadPGMToRaw(unsigned int ***raw, FILE *data) started.");


	//Read P2
	fgets(str, 200, data);
	if(!(str[0] == 'P' && str[1] == '2')){
		logIt(ERR, "Not a pgm.");
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
