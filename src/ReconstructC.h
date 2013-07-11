/*
 * ReconstructC.h
 *
 *  Created on: 04.07.2013
 *      Author: Alexander Winkler
 */

#ifndef RECONSTRUCTC_H_
#define RECONSTRUCTC_H_

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "Logger.h"
#include "Constants.h"
#include "ConfigReader.h"


int imgwidth;
int imgheight;
int numangles;
char *outputPath;
char *sinoPath;

int startReconstruction(void);

int reconstruct(FILE *dataFile);
int reconstruction(char *pathToSino, char *pathToOutput);

/*
	h is the sync filtere stored only in device memory.  There is no need to
	ever pull this into host memory.
 */
float h[2*IMG_WIDTH];
//struct timespec beg0, beg1, beg2, beg3, end0, end1, end2, end3;

void createFilter(void);
void filterSinogram(float *img, float *img_res);
void backProject(float deltaTheta, float *resultImage, float *sinogram);
int reconstruction(char *pathToSino, char *pathToOutput);
int startReconstruction(void);
int reconstruct(FILE *dataFile);




#endif /* RECONSTRUCTC_H_ */
