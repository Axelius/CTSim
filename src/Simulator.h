/*
 * Simulator.h
 *
 *  Created on: 20.06.2013
 *      Author: Alexander Winkler
 */

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <Math.h>
#include <limits.h>
#include <time.h>
#include "Logger.h"
#include "Constants.h"


#define IRON 1
#define BONE 2
#define WATER 3
#define AIR 4
#define MUSCLE 5
#define TISSUE 6

#define MINMAT 1
#define MAXMAT 6



typedef struct {
   float energy;
   float mu;
} attenuation;

unsigned int **ironRaw;
unsigned int **boneRaw;
unsigned int **waterRaw;
unsigned int **airRaw;
unsigned int **muscleRaw;
unsigned int **tissueRaw;

unsigned int **result;

attenuation *iron;
attenuation *bone;
attenuation *water;
attenuation *air;
attenuation *muscle;
attenuation *tissue;


size_t ironLength;
size_t boneLength;
size_t waterLength;
size_t airLength;
size_t muscleLength;
size_t tissueLength;



FILE *airImage;
FILE *boneImage;
FILE *ironImage;
FILE *muscleImage;
FILE *tissueImage;
FILE *waterImage;



int allocateRaw(unsigned int ***raw, int row, int col);
int loadPGMToRaw(unsigned int ***raw, FILE *data);
int allocateResult(int row, int col);
int project(int angle);
int exportPGM(FILE* out, unsigned int** write, int x, int y);
int freeRaw(unsigned int ***raw, int row, int col);
int simulation(char *pathToSlice, char *pathToOutputSinogram);
void setUpAttenuation();
void readAttenuationFile(char* pathToAttFile, attenuation** att, size_t* attLength);
double getInterpolatedAttenuation(int material, double energy);
int getAttenuation(int material, double kV, int positionX, int positionY);
void setUpRawFiles(char *pathToSlices);
void allocateAllRaws(void);
void closeAllInputImages(void);
void freeAllRaws(void);


#endif /* SIMULATOR_H_ */
