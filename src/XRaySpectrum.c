/*
 * XRaySpectrum.c
 *
 *  Created on: 18.07.2013
 *      Author: Alexander
 */

#include "XRaySpectrum.h"

void setUpSpectrum(){
	int tubeEnergy = cfg.tubeEnergy;
	char line[200];
	char path[512];
	int i = 0;
	FILE* read;
	int numAlloc = 0;
	logIt(DEBUG, "setUpSpectrum(int tubeEnergy) started.");
	if(tubeEnergy < 90){
		logIt(TRACE, "Selected tube Energy lower than 90keV. 80keV chosen.");
		tubeEnergy = 80;
	}
	else if (tubeEnergy >= 90 && tubeEnergy < 110){
		logIt(TRACE, "Selected tube Energy between 90 and 110. 100keV chosen.");
		tubeEnergy = 100;
	}
	else if (tubeEnergy >= 110 && tubeEnergy < 130){
		logIt(TRACE, "Selected tube Energy between 110 and 130. 120keV chosen.");
		tubeEnergy = 120;
	}
	else {
		logIt(TRACE, "Selected tube Energy higher than 130keV. 140keV chosen.");
		tubeEnergy = 140;
	}
	sprintf (path, "%s/XRaySpectrum%dkeV.txt", cfg.pathToXRaySpectra, tubeEnergy);
	logIt(TRACE, "path to Spectrum file = %s", path);
	read = fopen(path, "r");
	while(fgets(line, 200, read) != NULL){
		logIt(TRACE, "Line%d: %s", i, line);
		i++;
	}
	logIt(TRACE, "%d lines found.", i);
	numAlloc = i-2;
	logIt(TRACE, "Thus allocating %d energyPhoton pairs.", numAlloc);
	spectrumLength = numAlloc;
	spectrum = malloc((spectrumLength) * sizeof(energyPhoton));
	if(spectrum == NULL){
		logIt(ERR, "Not enough memory for allocating spectrum.");
	}
	fclose(read);
	sprintf (path, "%s/XRaySpectrum%dkeV.txt", cfg.pathToXRaySpectra, tubeEnergy);
	read = fopen(path, "r");
	//read the header of the file and discard it
	fgets(line, 200, read);
	i = 0;
	while(i< numAlloc && fgets(line, 200, read) != NULL){
		fscanf(read, "%d	%d", &(spectrum[i].energy), &(spectrum[i].photonCount));
		logIt(TRACE, "spectrum[%d].energy=%d, spectrum[%d].photonCount=%d", i, (spectrum[i].energy), i, (spectrum[i].photonCount));
		i++;
	}
	fclose(read);
	logIt(DEBUG, "setUpSpectrum(int tubeEnergy) finished.");
}

int getPhotonCount(int keV){
	logIt(TRACE, "getPhotonCount(int keV) started.");
	int i = 0;
	if(keV < spectrum[0].energy){
		//extrapolate in the lower end
		logIt(TRACE, "extrapolating in the lower end");
		logIt(TRACE, "getPhotonCount(int keV) finished.");
		return (((spectrum[1].photonCount - spectrum[0].photonCount)/(spectrum[1].energy - spectrum[0].energy))*(keV - spectrum[0].energy) + spectrum[0].photonCount);
	}
	for(i = 0; i<spectrumLength; i++){
		if(keV-spectrum[i].energy < 0.000001){
			logIt(TRACE, "perfect match");
			logIt(TRACE, "getPhotonCount(int keV) finished.");
			return spectrum[i].photonCount;
		}
		if(keV>spectrum[i].energy && keV<spectrum[i+1].energy){
			//interpolate here
			logIt(TRACE, "interpolate between element[%d]: (keV: %d, photonCount: %d) and element[%d]: (keV: %d, PhotonCount: %d)", i, spectrum[i].energy, spectrum[i].photonCount, i+1, spectrum[i+1].energy, spectrum[i+1].photonCount);
			logIt(TRACE, "getPhotonCount(int keV) finished.");
			return (((spectrum[i+1].photonCount - spectrum[i].photonCount)/(spectrum[i+1].energy - spectrum[i].energy))*(keV - spectrum[i].energy) + spectrum[i].photonCount);
		}
	}
	//extrapolate in the upper end
	logIt(TRACE, "extrapolating in the upper end");
	logIt(TRACE, "getPhotonCount(int keV) finished.");
	return (((spectrum[spectrumLength-1].photonCount - spectrum[spectrumLength-2].photonCount)/(spectrum[spectrumLength-1].energy - spectrum[spectrumLength-2].energy))*(keV - spectrum[spectrumLength-1].energy) + spectrum[spectrumLength-1].photonCount);
}
