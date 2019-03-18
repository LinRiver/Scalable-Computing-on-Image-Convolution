#ifndef __INCLUDED_IMAGE_H__
#define __INCLUDED_IMAGE_H__

#include <png.h>
#include <omp.h>

/* In image.h there are Img struct for image properties, Return_code enum as error state return, and four main functions on image processing */

  // In Img struct we place emphasis on the basic proerties for each image, its file address, width, height and memory address
  typedef struct {
    
    const char* file_name;    
    int width;
    int height;
    png_bytep pixel;

  }Img;

  // In Return_code enum we need three main error states
  typedef enum {

    RC_NO_PNG = -3,
    RC_NO_IMG = -2,
    RC_NO_FILE = -1,

  } Return_code;


  Return_code ReadFromFile(Img);
  void ApplyStencil(png_bytep, png_bytep);
  void ReleaseImage(png_bytep);
  Return_code WriteToFile(Img);

#endif
