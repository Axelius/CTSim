/*
 * XRaySpectrum.h
 *
 *  Created on: 18.07.2013
 *      Author: Alexander
 */

#ifndef XRAYSPECTRUM_H_
#define XRAYSPECTRUM_H_

#include "Logger.h"
#include "ConfigReader.h"
#include <stdlib.h>


typedef struct {
   int energy;
   int photonCount;
} energyPhoton;


energyPhoton *spectrum;
size_t spectrumLength;


void setUpSpectrum();
int getPhotonCount(int keV);



#endif /* XRAYSPECTRUM_H_ */
