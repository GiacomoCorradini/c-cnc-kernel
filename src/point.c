//   ____       _       _   
//  |  _ \ ___ (_)_ __ | |_ 
//  | |_) / _ \| | '_ \| __|
//  |  __/ (_) | | | | | |_ 
//  |_|   \___/|_|_| |_|\__|

#include "point.h"

// We are using a bit mask for encoding coordinates that are left undefined
// 0000 0000 => none set (0)
// 0000 0001 => x   is set (1)
// 0000 0010 => y   is set (2)
// 0000 0100 => z   is set (3)
// 0000 0111 => xyz is set (7)

typedef struct point {
  data_t x, y, z;
  uint8_t s; // encoding which data is defined
} point_t;

// Create a new point
point_t *point_new() {
  point_t *p = (point_t *)calloc(1, sizeof(point_t)); // in C++ malloc/calloc need to be cast in the initialize type -> permit to be used with C++
  if(!p /* p == NULL */){
    perror("Error creating a point");
    exit(EXIT_FAILURE);
  }
  return p;
}

// Free the point
void point_free(point_t *p) {
  assert(p);
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
  assert(p);
  p->x = x;
  p->s = p->s | X_SET; // | bitwise boolean operator or
}

// xxxx xxxx Initial p->s valie (unknown)
// 0000 0010 Char value of 1: '\2'
// --------- bitwise or
// xxxx xx1x Result
void point_set_y(point_t *p, data_t y) {
  assert(p);
  p->y = y;
  p->s |= Y_SET; // | bitwise boolean operator or
}

// xxxx xxxx Initial p->s valie (unknown)
// 0000 0100 Char value of 1: '\4'
// --------- bitwise or
// xxxx x1xx Result
void point_set_z(point_t *p, data_t z) {
  assert(p);
  p->z = z;
  p->s |= Z_SET; // | bitwise boolean operator or
}

// xxxx xxxx Initial p->s valie (unknown)
// 0000 0111 Char value of 1: '\7'
// --------- bitwise or
// xxxx x111 Result
void point_set_xyz(point_t *p, data_t x, data_t y, data_t z) {
  assert(p);
  p->x = x;
  p->y = y;
  p->z = z;
  p->s |= ALL_SET; // | bitwise boolean operator or
}

// Getters
data_t point_x(const point_t *p) { assert(p); return p->x; }
data_t point_y(const point_t *p) { assert(p); return p->y; }
data_t point_z(const point_t *p) { assert(p); return p->z; }

// Distance between two points
data_t point_dist(const point_t *from, const point_t *to) {
  assert(from && to);
  return sqrt(
    pow(to->x - from->x, 2) +
    pow(to->y - from->y, 2) +
    pow(to->z - from->z, 2)
    );
}

// Projections
void point_delta(const point_t *from, const point_t *to, point_t *delta) {
  assert(from && to && delta);
  point_set_xyz(delta, to->x - from->x, to->y - from->y, to->z - from->z);

}

// "Modal behaviour": a point may have undefined coordinates and if so it must be able to inherit undefined coordinates from the previous point
void point_modal(const point_t *from, point_t *to) {
  assert(from && to);
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
#define FIELD_LENGTH 6

void point_inspect(const point_t *p, char **desc) {
  assert(p);
  char str_x[FIELD_LENGTH + 1], str_y[FIELD_LENGTH + 1], str_z[FIELD_LENGTH + 1];
  
  if(p->s & X_SET) {
    snprintf(str_x, sizeof(str_x), "%*.3f", FIELD_LENGTH, p->x);
  } else { // not defined
    snprintf(str_x, sizeof(str_x), "%*s", FIELD_LENGTH, "-");
  }


  if(p->s & Y_SET) {
    snprintf(str_y, sizeof(str_y), "%*.3f", FIELD_LENGTH, p->y);
  } else { // not defined
    snprintf(str_y, sizeof(str_y), "%*s", FIELD_LENGTH, "-");
  }

  if(p->s & Z_SET) {
    snprintf(str_z, sizeof(str_z), "%*.3f", FIELD_LENGTH, p->z);
  } else { // not defined
    snprintf(str_z, sizeof(str_z), "%*s", FIELD_LENGTH, "-");
  }

  if(asprintf(desc, "[%s %s %s]", str_x, str_y, str_z) == -1) {
    perror("Could not create point description string");
    exit(EXIT_FAILURE);
  }; // allocate and write into a new string
}

#undef FIELD_LENGTH

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

  // point_set_x(NULL, 100);

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