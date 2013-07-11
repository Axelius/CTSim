/*
 * Attenuation.c
 *
 *  Created on: 04.07.2013
 *      Author: Alexander Winkler
 */

#include "Attenuation.h"

unsigned int getAttenuation(int material, double kV, int positionX, int positionY) {
	unsigned int **imageRaw;
	logIt(TRACE, "getAttenuation(int material, double kV, int positionX, int positionY) started.");

	switch(material){
	case IRON:
		logIt(TRACE, "Material ID%d: iron", material);
		imageRaw = ironRaw;
		break;
	case BONE:
		logIt(TRACE, "Material ID%d: bone", material);
		imageRaw = boneRaw;
		break;
	case WATER:
		logIt(TRACE, "Material ID%d: water", material);
		imageRaw = waterRaw;
		break;
	case AIR:
		logIt(TRACE, "Material ID%d: air", material);
		imageRaw = airRaw;
		break;
	case MUSCLE:
		logIt(TRACE, "Material ID%d: muscle", material);
		imageRaw = muscleRaw;
		break;
	case TISSUE:
		logIt(TRACE, "Material ID%d: tissue", material);
		imageRaw = tissueRaw;
		break;
	default:
		logIt(ERR, "Material ID%d not found!", material);
		logIt(TRACE, "getInterpolatedAttenuation(int material, double energy) finished.");
		return 0.0f;
	}


	logIt(TRACE, "getInterpolatedAttenuation(int material, double energy) finished.");
	return (unsigned int)(getInterpolatedAttenuationValue(material, kV) * ((double)imageRaw[positionX][positionY]));
}

double getInterpolatedAttenuationValue(int material, double energy) {
	attenuation* mat;
	size_t matLength;
	int i = 0;
	logIt(TRACE, "getInterpolatedAttenuation(int material, double energy) started.");

	switch(material){
	case IRON:
		logIt(TRACE, "Material ID%d: iron", material);
		mat = iron;
		matLength = ironLength;
		break;
	case BONE:
		logIt(TRACE, "Material ID%d: bone", material);
		mat = bone;
		matLength = boneLength;
		break;
	case WATER:
		logIt(TRACE, "Material ID%d: water", material);
		mat = water;
		matLength = waterLength;
		break;
	case AIR:
		logIt(TRACE, "Material ID%d: air", material);
		mat = air;
		matLength = airLength;
		break;
	case MUSCLE:
		logIt(TRACE, "Material ID%d: muscle", material);
		mat = muscle;
		matLength = muscleLength;
		break;
	case TISSUE:
		logIt(TRACE, "Material ID%d: tissue", material);
		mat = tissue;
		matLength = tissueLength;
		break;
	default:
		logIt(ERR, "Material ID%d not found!", material);
		logIt(TRACE, "getInterpolatedAttenuation(int material, double energy) finished.");
		return 0.0f;
	}



	if(energy < mat[0].energy){
		//extrapolate in the lower end
		logIt(TRACE, "extrapolating in the lower end");
		logIt(TRACE, "getInterpolatedAttenuation(int material, double energy) finished.");
		return (((mat[1].mu - mat[0].mu)/(mat[1].energy - mat[0].energy))*(energy - mat[0].energy) + mat[0].mu);
	}
	for(i = 0; i<matLength; i++){
		if(energy-mat[i].energy < 0.000001){
			logIt(TRACE, "perfect match");
			logIt(TRACE, "getInterpolatedAttenuation(int material, double energy) finished.");
			return mat[i].mu;
		}
		if(energy>mat[i].energy && energy<mat[i+1].energy){
			//interpolate here
			logIt(TRACE, "interpolate between element[%d]: (energy: %f, mu: %f) and element[%d]: (energy: %f, mu: %f)", i, mat[i].energy, mat[i].mu, i+1, mat[i+1].energy, mat[i+1].mu);
			logIt(TRACE, "getInterpolatedAttenuation(int material, double energy) finished.");
			return (((mat[i+1].mu - mat[i].mu)/(mat[i+1].energy - mat[i].energy))*(energy - mat[i].energy) + mat[i].mu);
		}
	}
	//extrapolate in the upper end
	logIt(TRACE, "extrapolating in the upper end");
	logIt(TRACE, "getInterpolatedAttenuation(int material, double energy) finished.");
	return (((mat[matLength-1].mu - mat[matLength-2].mu)/(mat[matLength-1].energy - mat[matLength-2].energy))*(energy - mat[matLength-1].energy) + mat[matLength-1].mu);
}

//Reads all attenuation coefficient tables. Path to them is hardcoded
void setUpAttenuation(){
	int i;
	logIt(DEBUG, "setUpAttenuation() started.");
	readAttenuationFile("MassAttenuationCoefficients/iron.txt", &iron, &ironLength);
	readAttenuationFile("MassAttenuationCoefficients/boneCortical.txt", &bone, &boneLength);
	readAttenuationFile("MassAttenuationCoefficients/water.txt", &water, &waterLength);
	readAttenuationFile("MassAttenuationCoefficients/air.txt", &air, &airLength);
	readAttenuationFile("MassAttenuationCoefficients/muscleSceletal.txt", &muscle, &muscleLength);
	readAttenuationFile("MassAttenuationCoefficients/tissueSoft.txt", &tissue, &tissueLength);

	//Log attenuations if loglevel == trace
	i = 0;
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
//Reads table of attenuation coefficients and saves them to attenuation array
void readAttenuationFile(char* pathToAttFile, attenuation** att, size_t* attLength){
	char line[200];
	int i = 0;
	int numAlloc;
	FILE* read;
	logIt(DEBUG, "readAttenuationFile(char* pathToAttFile, attenuation** att, size_t* attLength) started.");

	read = fopen(pathToAttFile, "r");

	while(fgets(line, 200, read) != NULL){
		logIt(TRACE, "Line%d: %s", i, line);
		i++;
	}
	logIt(TRACE, "%d lines found.", i);
	numAlloc = i-6;
	*attLength = i-6;
	*att = malloc(numAlloc * sizeof(attenuation));
	if(att == 0){
		logIt(ERR, "out of memory");
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
