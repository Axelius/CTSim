/*
 * Constants.h
 *
 *  Created on: 04.07.2013
 *      Author: Alexander Winkler
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

//Simulation Constants

//This is the number of projection angles for the CT data.
#define NUM_ANGLES 60

//rows of the input and output image
#define ROWS 512

//Cols of the input and output image
#define COLS 512

//Width of the sinogram
#define SINOGRAMSIZE 512

//Minimum Energy Level of the X-ray beams in KeV
#define MINENERGY 20

//Maximum Energy Level of the X-ray beams in KeV
#define MAXENERGY 60

//number of simulations for energy levels
#define ENERGYLEVELS 3



//Really, what could this be ;)
#define PI 3.1415926535897932384626433832795028841971693993751058f


//Reconstruction Constants

//This is the length of the sinc filter.
#define FILTER_WIDTH 512

//This is the width of the reconstructed image.
#define IMG_WIDTH 512

//This is the height of the reconstructed image.
#define IMG_HEIGHT 512



#endif /* CONSTANTS_H_ */
