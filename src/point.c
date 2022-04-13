//   ____       _       _   
//  |  _ \ ___ (_)_ __ | |_ 
//  | |_) / _ \| | '_ \| __|
//  |  __/ (_) | | | | | |_ 
//  |_|   \___/|_|_| |_|\__|

#include "point.h"

// We are using a bit mask for encoding coordinates that are left undefined
// 0000 0000 => none set (0)
// 0000 0001 => x is set (1)
// 0000 0010 => y is set (2)
// 0000 0100 => z is set (3)
// 0000 0111 => xyz is set (7)

typedef struct point {
  data_t x, y, z;
  uint8_t s; // encoding which data is defined
} point_t;

// Create a new point
point_t *point_new() {
  point_t *p = (point_t *)calloc(1, sizeof(point_t)); // in C++ malloc/calloc need to be cast in the initialize type -> permit to be used with C++
  return p;
}

// Free the point
void point_free(point_t *p) {
  free(p); // free the memory in order to be avaiable
  p = NULL; // set p to NULL
}

// set coordinates

#define X_SET '\1'
#define Y_SET '\2'
#define Z_SET '\4'
#define ALL_SET '\7'

// xxxx xxxx Initial p->s valie (unknown)
// 0000 0001 Char value of 1: '\1'
// --------- bitwise or
// xxxx xxx1 Result
void point_set_x(point_t *p, data_t x) {
  p->x = x;
  p->s = p->s | X_SET; // | bitwise boolean operator or
}

// xxxx xxxx Initial p->s valie (unknown)
// 0000 0010 Char value of 1: '\2'
// --------- bitwise or
// xxxx xx1x Result
void point_set_y(point_t *p, data_t y) {
  p->y = y;
  p->s |= Y_SET; // | bitwise boolean operator or
}

// xxxx xxxx Initial p->s valie (unknown)
// 0000 0100 Char value of 1: '\4'
// --------- bitwise or
// xxxx x1xx Result
void point_set_z(point_t *p, data_t z) {
  p->z = z;
  p->s |= Z_SET; // | bitwise boolean operator or
}

// xxxx xxxx Initial p->s valie (unknown)
// 0000 0111 Char value of 1: '\7'
// --------- bitwise or
// xxxx x111 Result
void point_set_xyz(point_t *p, data_t x, data_t y, data_t z) {
  p->x = x;
  p->y = y;
  p->z = z;
  p->s |= ALL_SET; // | bitwise boolean operator or
}

// Getters
data_t point_x(point_t *p) { return p->x; }
data_t point_y(point_t *p) { return p->y; }
data_t point_z(point_t *p) { return p->z; }

// Distance between two points
data_t point_dist(point_t *from, point_t *to) {
  return sqrt(
    pow(to->x - from->x, 2) +
    pow(to->y - from->y, 2) +
    pow(to->z - from->z, 2)
    );
}

// Projections
void point_delta(point_t *from, point_t *to, point_t *delta) {
  point_set_xyz(delta, to->x - from->x, to->y - from->y, to->z - from->z);

}

// "Modal behaviour": a point may have undefined coordinates and if so it must be able to inherit undefined coordinates from the previous point
void point_modal(point_t *from, point_t *to) {
  if(!(to-> s & X_SET) && (from->s & X_SET)) {
    point_set_x(to, from->x);
  }
  if(!(to-> s & Y_SET) && (from->s & Y_SET)) {
    point_set_y(to, from->y);
  }
  if(!(to-> s & Z_SET) && (from->s & Z_SET)) {
    point_set_z(to, from->z);
  }
}

// Inspection
void point_inspect(point_t *p, char **desc) {
  char str_x[9], str_y[9], str_z[9];
  
  if(p->s & X_SET) {
    sprintf(str_x, "%8.3f", p->x);
  } else { // not defined
    sprintf(str_x, "%8s", "-");
  }


  if(p->s & Y_SET) {
    sprintf(str_y, "%8.3f", p->y);
  } else { // not defined
    sprintf(str_y, "%8s", "-");
  }

  if(p->s & Z_SET) {
    sprintf(str_z, "%8.3f", p->z);
  } else { // not defined
    sprintf(str_z, "%8s", "-");
  }

  asprintf(desc, "[%s %s %s]", str_x, str_y, str_z); // allocate and write into a new string
}

//   _____ _____ ____ _____ 
//  |_   _| ____/ ___|_   _|
//    | | |  _| \___ \ | |  
//    | | | |___ ___) || |  
//    |_| |_____|____/ |_|  

#ifdef POINT_MAIN

int main(){
  point_t *p1, *p2, *p3;
  char *desc;

  p1 = point_new();
  p2 = point_new();
  p3 = point_new();

  point_set_xyz(p1, 0, 0, 0);
  point_set_x(p2, 100);
  point_set_y(p2, 100);
  point_inspect(p2, &desc);

  printf("Initial:     %s\n", desc);
  point_modal(p1,p2);
  point_inspect(p2, &desc);
  printf("After modal: %s\n", desc);

  printf("Distance:     %f\n", point_dist(p1, p2));

  point_delta(p1, p2, p3);
  point_inspect(p3, &desc);
  printf("Projections:  %s\n", desc);

  point_free(p1);
  point_free(p2);
  point_free(p3);
  free(desc);

  return 0;
}

#endif //POINT_MAIN