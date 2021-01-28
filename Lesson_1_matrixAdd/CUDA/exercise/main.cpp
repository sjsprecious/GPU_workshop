#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pch.h"
#include <ctime>
#include <chrono>
#include <ratio> 

int main(int argc, char* argv[]) {
  using namespace std::chrono;

  float *h_A, *h_B, *h_C, *h_check;
  high_resolution_clock::time_point t0, t1;
  duration<double> t1sum;
  int dx, dy; 

  if (argc > 1 && argc < 4) {
    dx = atoi(argv[1]);
    dy = atoi(argv[2]);
  } else if (argc >= 4){
    printf("Usage: ./executable dimX dimY\n");
    return -1;
  } else {
    // set to default dimensions (1024) if no arguments are passed
    dx = DEFAULT_DIM;
    dy = DEFAULT_DIM;
  }

  t0 = high_resolution_clock::now();

  // Allocate memory to host matrices
  h_A = (float*)malloc(dx*dy*sizeof(float));
  h_B = (float*)malloc(dx*dy*sizeof(float));
  h_C = (float*)malloc(dx*dy*sizeof(float));
  h_check = (float*)malloc(dx*dy*sizeof(float));
  
  // Init matrices with default values of 3.0 for matrix A and 2.0 for matrix B
  InitializeMatrixSame(h_A, dx, dy, MAT_A_VAL,"h_A");
  InitializeMatrixSame(h_B, dx, dy, MAT_B_VAL,"h_B");

  t1 = high_resolution_clock::now();
  t1sum = duration_cast<duration<double>>(t1-t0);
  printf("Init took %f seconds. Begin compute.\n", t1sum.count());

  // Calculate A+B=C on the host
  t0 = high_resolution_clock::now();
  cpu_matrix_add(h_A, h_B, h_check, dx, dy);
  t1 = high_resolution_clock::now();
  t1sum = duration_cast<duration<double>>(t1-t0);
  printf("CPU Matrix Addition took %f seconds.\n", t1sum.count());

  // Calcuate A+B=C on the device
  t0 = high_resolution_clock::now();
  gpu_matrix_add(h_A, h_B, h_C, dx, dy);
  t1 = high_resolution_clock::now();
  t1sum = duration_cast<duration<double>>(t1-t0);
  printf("GPU Matrix Addition took %f seconds.\n", t1sum.count());

  // Printout for debugging
   if (dx <= 6 && dy <= 6) {
        printf("\nCPU Matrix Addition Results: \n");
        PrintMatrix(h_check, dx, dy);
        printf("\nGPU Matrix Addition Results: \n");
        PrintMatrix(h_C, dx, dy);
  } 
  // Check for correctness
  MatrixVerification(h_check, h_C, dx, dy, VERIF_TOL);
  
  // Cleanup
  free(h_A);
  free(h_B);
  free(h_C);
  free(h_check);
  return 0;
}

