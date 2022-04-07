//   ____       _       _   
//  |  _ \ ___ (_)_ __ | |_ 
//  | |_) / _ \| | '_ \| __|
//  |  __/ (_) | | | | | |_ 
//  |_|   \___/|_|_| |_|\__|

#ifndef POINT_H
#define POINT_H

#include "defines.h"

// We are using a bit mask for encoding coordinates that are left undefined
// 0000 0000 => none set (0)
// 0000 0001 => x is set (1)
// 0000 0010 => y is set (2)
// 0000 0100 => z is set (3)
// 0000 0111 => xyz is set (7)

typedef struct {
  data_t x, y, z;
  uint8_t s;
} point_t;

// Create a point
point_t point_new();

// set coordinates
void point_x(point_t *p, data_t val);
void point_y(point_t *p, data_t val);
void point_z(point_t *p, data_t val);
void point_xyz(point_t *p, data_t x, data_t y, data_t z);

// Distance between two points
data_t point_dist(point_t *from, point_t *to);

// Projections
void point_delta(point_t *from, point_t *to, point_t *delta);

// Inspection
void point_inspect(point_t *p, char **desc);

// "Modal behaviour": a point may have undefined coordinates and if so it must be able to inherit undefined coordinates from the previous point
void print_modal(point_t *cur, point_t *prev);


#endif // POINT_H