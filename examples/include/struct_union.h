#include <stdio.h>
#include <stdlib.h>

#define ARY_LEN 6

// define a struct
typedef double data_t;

typedef struct {
  data_t x, y, z;
} point_t;

typedef union {
  float x, y, z;
} my_union;

void print_point(point_t *a);

void print_array(data_t a[], int n);