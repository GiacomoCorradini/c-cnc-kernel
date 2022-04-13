#include <stdio.h>
#include <stdlib.h>

// Function that print an array
void array_print(float *a, int n);

// First way to initialize an array of pointer
float* f1(int n);

// Second way to initialize an array of pointer
void f2(float *a, int n);

// Third way to initialize an array of pointer (BETTER!!)
void f3(float **a, int n);