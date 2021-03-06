/*
 * Simulator.h
 *
 *  Created on: 20.06.2013
 *      Author: Alexander Winkler
 */

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#define _USE_32BIT_TIME_T 1

#include <stdio.h>
#include <stdlib.h>
#include <Math.h>
#include <limits.h>
#include <string.h>
#include <windows.h>



#include <time.h>
#include "Logger.h"
#include "Constants.h"
#include "Attenuation.h"
#include "ConfigReader.h"
#include "XRaySpectrum.h"
#include "pgmImportExport.h"


#define IRON 1
#define BONE 2
#define WATER 3
#define AIR 4
#define MUSCLE 5
#define TISSUE 6

#define MINMAT 1
#define MAXMAT 6


unsigned int **ironRaw;
unsigned int **boneRaw;
unsigned int **waterRaw;
unsigned int **airRaw;
unsigned int **muscleRaw;
unsigned int **tissueRaw;

unsigned int **result;


int *precalculatedPhotonCounts;



FILE *airImage;
FILE *boneImage;
FILE *ironImage;
FILE *muscleImage;
FILE *tissueImage;
FILE *waterImage;

typedef struct {
   int startCount;
   int endCount;
}t;



void allocateUnsignedIntArray(unsigned int ***raw, unsigned int row, unsigned int col);
int project(int projectionNumber, double angle);
void freeUnsignedIntArray(unsigned int ***raw, int row, int col);
int simulation(char *pathToSlice, char *pathToOutputSinogram);
void setUpRawFiles(char *pathToSlices);
void allocateAllRaws(void);
void closeAllInputImages(void);
void freeAllRaws(void);
DWORD WINAPI projectFromTo(void * param);


#endif /* SIMULATOR_H_ */
