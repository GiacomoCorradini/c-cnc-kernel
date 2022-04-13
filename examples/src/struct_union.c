#include "struct_union.h"

void print_point(point_t *a) { printf("(%f %f %f)\n", a->x, a->y, a->z); }

void print_array(data_t a[], int n) {
  printf("[");

  for (int i = 0; i < n; i++) {
    printf(" %f ", a[i]);
  }
  printf("]\n");
}