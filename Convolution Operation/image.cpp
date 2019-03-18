#include <cstdlib>
#include <stdlib.h>
#include <mm_malloc.h>
#include "image.h"


const int img_width = 6000;
const int img_height = 6000;

/* ApplyStencil function is for convolutional operation, specifically a 3 x 3 stencil filter executes the calculation on the 6000 x 6000 image in our study */
void ApplyStencil(png_bytep inimg, png_bytep outimg) {
  
  const int width  = img_width;
  const int height = img_height;

  omp_set_num_threads(16);

/* Here we make weigths in the filter array multipy each pixel value in the image array, and save the added result into another image array as output image */
#pragma omp parallel for
  for (int i = 1; i < height-1; i++) {	  
    for (int j = 1; j < width-1; j++) {
      int val = -inimg[(i-1)*width + j-1] - inimg[(i-1)*width + j] - inimg[(i-1)*width + j+1] 
	              -inimg[(i  )*width + j-1] + 8*inimg[(i  )*width + j] - inimg[(i  )*width + j+1] 
	              -inimg[(i+1)*width + j-1] - inimg[(i+1)*width + j] - inimg[(i+1)*width + j+1];

      val = (val < 0   ? 0   : val);
      val = (val > 255 ? 255 : val);

      outimg[i*width + j] = val;
    }
  }

}


/* ReadFromFile function focuses on image reading with png library, and returns the error states */
Return_code ReadFromFile(Img img) {

  // Make sure whether the file is readable and return RC_NO_FILE state once not readable state
  FILE *fp = fopen(img.file_name, "rb");
  if (!fp) {
    printf("Could not open %s\n", img.file_name);
    return RC_NO_FILE;
  }

  // Make sure whether the file is a PNG and the maximum checked size is 8, in general return RC_NO_PNG state if not work
  png_byte header[8]; 
  fread(header, 1, 8, fp);
  if (png_sig_cmp(header, 0, 8)){
    printf("File %s is not a proper PNG file\n", img.file_name);
    fclose(fp);
    return RC_NO_PNG;
  }

  int width;
  int height;

  // PNG information
  png_structp ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info = png_create_info_struct(ptr);
  setjmp(png_jmpbuf(ptr));
  png_init_io(ptr, fp);
  png_set_sig_bytes(ptr, 8);
  png_read_info(ptr, info);

  png_byte color_type = png_get_color_type(ptr, info);
  png_byte bit_depth = png_get_bit_depth(ptr, info);

  width = png_get_image_width(ptr, info);
  int width_png = width;
  height = png_get_image_height(ptr, info);

  int number_of_passes = png_set_interlace_handling(ptr);
  png_read_update_info(ptr, info);

  // Read the image data
  setjmp(png_jmpbuf(ptr));
  png_bytep* row = (png_bytep*) malloc(sizeof(png_bytep) * height);
  for (int i = 0; i < height; i++)
    row[i] = (png_byte*) malloc(png_get_rowbytes(ptr, info));  
  png_read_image(ptr, row);

  if(png_get_rowbytes(ptr, info) != width_png) {
    printf("Error: the image is not in grayscale\n"); 
  }

  fclose(fp);

  //transform each pixel data from the row array using png_read_image() to the img.pixel array in Img struct
#pragma omp parallel for 
  for(int i = 0; i < height; i++)
    for(int j = 0; j < width; j++)
      img.pixel[i*width+j] = (png_byte) row[i][j];

  for (int i = 0; i < height; i++)
    free(row[i]);
  free(row);

}


/* ReleaseImage function is to free memory using _mm_malloc() */
void ReleaseImage(png_bytep pixel) {

  _mm_free(pixel);
}


/* WriteToFile function generates the image with png library */
Return_code WriteToFile(Img img) {

  // Make sure whether the file is opened for writing and RC_NO_FILE state as it not opened
  FILE *fp = fopen(img.file_name, "wb");
  if (!fp) {
    printf("Could not open %s for writing\n", img.file_name);
    return RC_NO_FILE;
  }
  
  const int width = img.width;
  const int height = img.height;

  // Create handes when ready writing out the image
  png_structp ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);;
  png_infop info = png_create_info_struct(ptr);
  setjmp(png_jmpbuf(ptr));

  png_init_io(ptr, fp);
  png_set_IHDR(ptr, info, width, height, 8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
  png_write_info(ptr, info);

  // transform each pixel data from the img_pixel array in Img struct to the row array using png_write_row()
  png_bytep row = (png_bytep) malloc(sizeof(png_byte)*width);
  for(int i = 0; i < height; i++) {
    for(int j = 0; j < width; j++) {
      png_byte t = (png_byte)img.pixel[i*width+j];
      if (t < 0) t = 0;
      if (t > 255) t = 255;
      row[j] = (png_byte) t;
    }
    png_write_row(ptr, row);
  }

  // Release temporary data allocated
  png_write_end(ptr, NULL);
  png_free_data(ptr, info, PNG_FREE_ALL, -1);
  png_destroy_write_struct(&ptr, (png_infopp)NULL);
  fclose(fp);
  free(row);
}
