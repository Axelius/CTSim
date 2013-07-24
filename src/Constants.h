/*
 * Constants.h
 *
 *  Created on: 04.07.2013
 *      Author: Alexander Winkler
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

//Simulation Constants


//rows of the input and output image
#define ROWS 512

//Cols of the input and output image
#define COLS 512

//Width of the sinogram
#define SINOGRAMSIZE 512

//size of a pixel in the slice in cm
#define SIZEOFPIXEL 0.1f

#define DETECTORSATURATION 1000000


//Really, what could this be ;)
#define PI 3.1415926535897932384626433832795028841971693993751058f

//Euler's number e
#define EULER 2.7182818284590452353602874713526624977572470936999595749669676277f


//Reconstruction Constants

//This is the length of the sinc filter.
#define FILTER_WIDTH 512

//This is the width of the reconstructed image.
#define IMG_WIDTH 512

//This is the height of the reconstructed image.
#define IMG_HEIGHT 512



#endif /* CONSTANTS_H_ */
