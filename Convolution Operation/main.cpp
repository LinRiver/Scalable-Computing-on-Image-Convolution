#include <stdlib.h>
#include <mm_malloc.h>
#include <cmath>
#include <omp.h>
#include "image.h"

const int in_width = 6000;
const int in_height = 6000;
const int out_width = 6000;
const int out_height = 6000;
const int nTrials = 10;
const int skipTrials = 3;

/* Stats function is to calculate the average performance by mean value and standard error*/
double Stats(double & x, double & dx) {
  x  /= (double)(nTrials - skipTrials);
  dx  = sqrt(dx/double(nTrials - skipTrials) - x*x);
}

int main(int argc, char** argv) {

  if(argc < 2) {
    printf("Usage: %s {file}\n", argv[0]);
    return(1);
  }

  printf("\n\033[1mEdge detection with a 3x3 stencil\033[0m\n");
  printf("\nImage size: %d x %d\n\n", in_width, in_height);
  printf("\033[1m%5s %15s %15s\033[0m\n", "Step", "Time, ms", "GFLOP/s"); fflush(stdout);

  double t, dt, f, df;

  // Create two Img struct, in_img and out_img for edge detection
  Img in_img;
  in_img.file_name = "test-image.png";
  in_img.width = in_width;
  in_img.height = in_height;
  in_img.pixel = (png_bytep)_mm_malloc(sizeof(png_byte)*in_width*in_height, 64);

  Img out_img;
  out_img.file_name = "output.png";
  out_img.width = out_width;
  out_img.height = out_height;
  out_img.pixel = (png_bytep)_mm_malloc(sizeof(png_byte)*out_width*out_height, 64);

  ReadFromFile(in_img);
  
  // Calculate the average performance based 10 trials 
  for (int iTrial = 1; iTrial <= nTrials; iTrial++) {
    const double t0 = omp_get_wtime();
    ApplyStencil(in_img.pixel, out_img.pixel);
    const double t1 = omp_get_wtime();

    const double ts   = t1-t0; // time in seconds
    const double tms  = ts*1.0e3; // time in milliseconds
    const double fpps = double(in_width*in_height*2*9)*1e-9/ts; // performance in GFLOP/s

    if (iTrial > skipTrials) { // Collect statistics for mean value and standard error
      t  += tms; 
      dt += tms*tms;
      f  += fpps;
      df += fpps*fpps;
    }

    // Output performance
    printf("%5d %15.3f %15.3f %s\n", 
	   iTrial, tms, fpps, (iTrial<=skipTrials?"*":""));
    fflush(stdout);
  }

  Stats(t, dt);  
  Stats(f, df);  
  printf("-----------------------------------------------------\n");
  printf("\033[1m%s\033[0m\n%8s   \033[42m%8.1f+-%.1f\033[0m   \033[42m%8.1f+-%.1f\033[0m\n",
	 "Average performance:", "", t, dt, f, df);
  printf("-----------------------------------------------------\n");
  printf("* - warm-up, not included in average\n\n");

  // Write out the result image
  WriteToFile(out_img);
  printf("\nOutput written into output.png\n");

  ReleaseImage(in_img.pixel);
  ReleaseImage(out_img.pixel);
}
