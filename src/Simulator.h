/*
 * Simulator.h
 *
 *  Created on: 20.06.2013
 *      Author: Alexander
 */

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

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
#define ATTF 100
#define IRON 1
#define BONE 2
#define WATER 3
#define AIR 4
#define MUSCLE 5
#define TISSUE 6



typedef struct {
   float energy;
   float mu;
} attenuation;

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


//forward Declarations
int allocateRaw(int row, int col);
int loadPGMToRaw(FILE *data);
int allocateResult(int row, int col);
int project(int angle);
int exportPGM(FILE* out, unsigned int** write, int x, int y);
int freeRaw(int row, int col);
int simulation(char *pathToSlice, char *pathToOutputSinogram);
void setUpAttenuation();
void readAttenuationFile(char* pathToAttFile, attenuation** att, size_t* attLength);
double getInterpolatedAttenuation(int material, double energy);


#endif /* SIMULATOR_H_ */
