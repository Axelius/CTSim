/*
 * Attenuation.h
 *
 *  Created on: 04.07.2013
 *      Author: Alexander Winkler
 */

#ifndef ATTENUATION_H_
#define ATTENUATION_H_

#include "Logger.h"
#include "Simulator.h"

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

void setUpAttenuation();
void readAttenuationFile(char* pathToAttFile, attenuation** att, size_t* attLength);
double getInterpolatedAttenuationValue(int material, double energy);
unsigned int getAttenuation(int material, double kV, int positionX, int positionY);



#endif /* ATTENUATION_H_ */
