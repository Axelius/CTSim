/*
 ============================================================================
 Name        : ReconstructC.c
 Author      : Alexander Winkler, Adam Arnesen and Patrick Ostler
 Version     :
 Copyright   : Whatever you want to do with it
 Description : CT Reconstrcution In C: Takes a Sinogram and does a FBP to reconstruct a slice
 ============================================================================
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
//using namespace std;

#define PI 3.141592653589793238462643f
#define FILTER_WIDTH 1024  	//This is the length of the sinc filter.
#define IMG_WIDTH 512		//This is the width of the reconstructed image.
#define IMG_HEIGHT 512		//This is the height of the reconstructed image.
#define NUM_ANGLES 200		//This is the number of projetion angles from the CT data.
int imgwidth = 0;
int imgheight = 0;
int numangles = 0;

int startReconstruction(void);

int reconstruct(FILE *dataFile);

/*
	h is the sync filtere stored only in device memory.  There is no need to
	ever pull this into host memory.
*/
float h[FILTER_WIDTH];
struct timespec beg0, beg1, beg2, beg3, end0, end1, end2, end3;

/*
	This kernel creates a basic sync function to be used as a filter for the sinogram
*/
void createFilter(){
  int idx;
  for(idx = -IMG_WIDTH; idx < IMG_WIDTH; idx++){
    h[idx+IMG_WIDTH] = (0.5f*(idx==0?1.0f:(sin(PI*idx)/(PI*idx)))) - (0.25f*pow((idx==0?1.0f:(sin(PI*idx/2.0f)/(PI*idx/2.0f))), 2));
  }
}

/*
	This kernel filters the original sinogram data line by line by convolving
	each line of the sinogram with the 'h' filter.
*/
void filterSinogram(float *img, float *img_res){
  int j, k, i;
  for (j = 0; j < NUM_ANGLES; j++){
    for (k = 0; k < IMG_WIDTH; k++){
      int gLoc = k + j*IMG_WIDTH;
      float sum = 0;
      float value = 0;

      for(i = -IMG_WIDTH; i < IMG_WIDTH; i++){
	if((k + i) < 0 || (k + i) > IMG_WIDTH-1){
	  value = 0;
	}
	else{
	  value = img[gLoc + i];
	}
	sum += value * h[IMG_WIDTH + i];
      }
      img_res[gLoc] = sum;
    }
  }
}

/*
	This is the main kernel for the reconstruction. Each thresad operates on a
	single pixel of the fully reconstructed image.  For each angel in the sinogram,
	the thread calculates which pixel out of that line of the sinogram would be
	projected onto the pixel that the thread is computing.  This pixel value
	is added to the pixel value and then the thread moves on to the next angel
	and therefore the next line of the sinogram.
*/
void backProject(float deltaTheta, float *resultImage, float *sinogram){
  int i, j, k;
  float cosTheta;
  float sinTheta;
  float ninety = PI/2.0f;
  float one_eighty = PI;
  //iterate over each row of the sinogram each thread gets one pixel from the line.
  for (j = 0; j < IMG_HEIGHT; j++){
    for (k = 0; k < IMG_WIDTH; k++){
      float finalPixel = 0.0f;
      float theta = 0.0f;
      for(i = 0; i < NUM_ANGLES; i++){
	cosTheta = cos(theta);
	sinTheta = sin(theta);
	int index;

	//Check for boundary conditions, infinite slope lines
	if (theta == 0.0f){
	  index = k;
	}
	else if(theta == ninety){
	  index = 511 - j;
	}
	else if(theta == one_eighty){
	  index = 511 - k;
	}
	// Do normal computation for non-corner cases
	else{
	  //Calculate slopes of both lines.
	  float m1 = -cosTheta/sinTheta;
	  float m2 = sinTheta/cosTheta;
	  // Calculate x coordinate of lines intersection
	  float x = ((255.0f - j) - m1*(k - 256.0f))/(m2-m1);
	  // calculate the distance from origin to desired point on rotated line.
	  float d = abs(x)*sqrt(1+pow(m2,2));
	  //Calculate index into sinogram row for this angel
	  if (theta < ninety){
	    if (x < 0.0f)
	      index = 256 + floor(d);
	    else
	      index = 256 - floor(d);
	  }
	  else{// theta > 90.0
	    if (x < 0.0f)
	      index = 256 - floor(d);
	    else
	      index = 256 + floor(d);
	  }
	}
	// boundary check and update running sum
	if(index >= 0 && index < 512){
	  finalPixel += sinogram[index+i*512];
	}
	theta += deltaTheta;
      }
      // do one coherent write for the block back to global memory of the fully reconstructed pixel
      resultImage[k + j*IMG_WIDTH] = finalPixel;
    }
  }
}















int main(int argc, char *argv[]){
	int ret = 0;
	ret = startReconstruction();
	return ret;

}

int startReconstruction(){
	int return_value = 0;
	char str[200];

	FILE *dataFile = fopen("sinograms/o2.pgm","r");

	//Read P2
	fgets(str, 200, dataFile);
	if(!(str[0] == 'P' && str[1] == '2')){
		printf("Not a pgm!");
		return 1;
	}


	fgets(str, 200, dataFile); //Hopefully a commentary
	fscanf(dataFile,"%d",&imgwidth);
	fscanf(dataFile,"%d",&numangles);
	fgets(str, 200, dataFile);

	fgets(str, 200, dataFile);//colordepth, we dont care about

	//printf("width: %d, height: %d", imgwidth, numangles);



	//while(! feof(dataFile)){
	//	fgets(str, 200, dataFile);
	//	printf(str);
	//}


	//imgwidth = 512;		//This is the width of the reconstructed image.
	imgheight = 512;		//This is the height of the reconstructed image.
	//numangles = 200;		//This is the number of projetion angles from the CT data.
	return_value = reconstruct(dataFile);
	return return_value;
}

/*
	This is the primary host code.
*/
int reconstruct(FILE *dataFile){
  int j, i;
  float min, max;
  FILE *fileOut;
  float *orig_sinogram;
  float *filt_sinogram;
  float *output;
  size_t size_sinogram = imgwidth*numangles*sizeof(float);
  size_t size_result = imgwidth*imgheight*sizeof(float);
  output = (float *)malloc(size_result);




  // Reserve host memory for sinogram versions
  orig_sinogram = (float *)malloc(size_sinogram);
  filt_sinogram = (float *)malloc(size_sinogram);


  if (dataFile == NULL){
    printf("Data file: data was not found\n");
    return 0;
  }

  //Read in sinogram from text file. This is probably a slow point!
  for(i=0; i < numangles*imgwidth; i++){
    fscanf(dataFile,"%f",&orig_sinogram[i]);
  }




  fclose(dataFile);

  //Create filter and save to device memory. The filter is never viewed on the host.
   createFilter();
   // Filtere the lines of the sinogram using the generated filter.
   filterSinogram(orig_sinogram, filt_sinogram);

   float deltaTheta = PI/NUM_ANGLES;  //delta theta in radians
   // Back Project and reconstruct full image pixel by pixel.
   backProject(deltaTheta, output, filt_sinogram);

  fileOut = fopen("outData.pgm", "wb");
  fprintf(fileOut, "P2\n%d %d\n255\n", imgwidth, imgheight);



  min = output[0];
  max = output[0];
  for(j = 0; j<imgheight*imgwidth; j++){
	  if(output[j]<min){
		  min = output[j];
	  }

	  if(output[j]>max){
		  max = output[j];
	  }
  }

  //Export image data.
  for(j=0;j<imgwidth;j++){
    for(i = 0; i< imgheight; i++){
      //printf("%e ",output_host[j*IMG_WIDTH + i]);
	  fprintf(fileOut,"%d ",(int)((output[j*imgwidth + i]-min)/(max-min)*255.0));
		//fwrite((const void *) &(output_host[j*IMG_WIDTH + i]), 1,3,fileOut);


    }

  }



  min = orig_sinogram[0];
  max = orig_sinogram[0];

    //Output the input as a picture file
    for(j = 0; j<numangles*imgwidth; j++){
	  if(orig_sinogram[j]<min){
		  min = orig_sinogram[j];
	  }

	  if(orig_sinogram[j]>max){
		  max = orig_sinogram[j];
	  }
  }
  FILE *input = fopen("input.pgm", "wb");
  fprintf(input, "P2\n%d %d\n255\n", imgwidth, numangles);
  for(i=0; i < numangles*imgwidth; i++){
    fprintf(input,"%d ",(int)((orig_sinogram[i]-min)/(max-min)*255.0));
  }
  fclose(input);







  //Free all remaining host memory.
  free(orig_sinogram);
  free(filt_sinogram);
  free(output);
  fclose(fileOut);


  return 0;
}



