/**
 * Calculate the area of a poligon
 **/

#include "stdio.h"
#include "stdlib.h"
#include "math.h"

typedef struct {
int n;         // number of sides
double *x, *y; // vectors of x and y coordinates
} polygon_t;

// 1. Create a new polygon instance
polygon_t *polygon_new(int n);

// 2. Add the vertex (x, y) at position i; returns next position i+1
int polygon_add_corner(polygon_t *p, int i, double x, double y);

// 3. Calculate the polygon area
double polygon_area(polygon_t *p);

int main(int argc, char const *argv[]) {
  polygon_t *p = polygon_new(4);
  printf("%d\n", p->n);
  int i = 0;
  i = polygon_add_corner(p, i, -1, -1);
  i = polygon_add_corner(p, i, -1, 1);
  i = polygon_add_corner(p, i, 1, 1);
  i = polygon_add_corner(p, i, 1, -1);
  for(int j = 0; j < p->n; j++){
    printf("pos %d, (%.3f,%.3f)\n", j, p->x[j], p->y[j]);
  }
  printf("Area: %f\n", polygon_area(p));
  return 0;
}

polygon_t *polygon_new(int n){
  //double x[n];
  //double y[n];
  polygon_t *p = (polygon_t *)calloc(1,sizeof(polygon_t));
  if (!p) {
    perror("Error creating a polygon");
    exit(EXIT_FAILURE);
  }
  p->n = n;
  //p->x = x;
  p->x = (double *)calloc(n,sizeof(double));
  //p->y = y;
  p->y = (double *)calloc(n,sizeof(double));
  return p;
}

int polygon_add_corner(polygon_t *p, int i, double x, double y){
  p->x[i] = x;
  p->y[i] = y;
  return i+1;
}

double polygon_area(polygon_t *p){
  double A, sum = 0, tmp;
  for(int j = 0; j < p->n - 1; j++){
    tmp = (p->x[j] * p->y[j+1]) - (p->x[j+1] * p->y[j]);
    if(j == 0) tmp *= 2;
    sum += tmp;
  }
  A = fabs(sum) / 2;
  return A;
}