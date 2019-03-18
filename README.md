# Scalable-Computing-on-Image-Convolution
## Introduction
Based on "Fundamentals of Parallelism on Intel Architecture" in Coursera, I implement convolutional operation on image processing with C language.
In this project, I evaluate the performance on one of image processing, Edge Detection with stencil operation. To be more specific, stencil operators take an input image, a large 36 megapixel image, and execute convolution operation with stencil kernel – “nine point stencil for edge detection” - to generate the output image. In order to the improvement on stencil calculation, the first main idea is parallel computing. Here OpenMP can boost the performance across multiple threads rather than only single thread on serial program. In addition, we change the data type on two arrays from float point to png_byte since our performance is limited by the memory bandwidth. Type png_byte can make the image contain pixels with eight bit intensity depth in order to use memory less. However, it is possible to run out of bounds when stencil calculation process. Thus our solution is to do stencil calculation in type int after reading from memory as png_byte, and then write out int back to png_byte.

## Reference
1. Difference between serial and parallel processing, http://www.itrelease.com/2017/11/difference-serial-parallel-processing/
2. OpenMP, http://www.admin-magazine.com/HPC/Articles/Programming-with-OpenMP 
3. libpng.txt – A description on how to use and modify libpng, http://www.libpng.org/pub/png/libpng-1.4.0-manual.pdf
4. Understanding measures of supercomputer performance and storage system capacity, https://kb.iu.edu/d/apeq#measure-flops
5. Allocating and Free Aligned Memory Blocks, https://software.intel.com/en-us/node/523368
6. Fundamentals of Parallelism on Intel Architecture, https://www.coursera.org/learn/parallelism-ia
