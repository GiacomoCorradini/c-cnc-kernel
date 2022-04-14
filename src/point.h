//   ____       _       _   
//  |  _ \ ___ (_)_ __ | |_ 
//  | |_) / _ \| | '_ \| __|
//  |  __/ (_) | | | | | |_ 
//  |_|   \___/|_|_| |_|\__|

#ifndef POINT_H
#define POINT_H

#include "defines.h"


// Object class defined as an OPAQUE STRUCT 
typedef struct point point_t;

//   _____                 _   _                 
//  |  ___|   _ _ __   ___| |_(_) ___  _ __  ___ 
//  | |_ | | | | '_ \ / __| __| |/ _ \| '_ \/ __|
//  |  _|| |_| | | | | (__| |_| | (_) | | | \__ \
//  |_|   \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
                                              

// Create a new point
point_t *point_new();

// Free the point
void point_free(point_t *p);

// set coordinates
void point_set_x(point_t *p, data_t x);
void point_set_y(point_t *p, data_t y);
void point_set_z(point_t *p, data_t z);
void point_set_xyz(point_t *p, data_t x, data_t y, data_t z);

// Distance between two points
data_t point_dist(const point_t *from, const point_t *to);

// Projections
void point_delta(const point_t *from, const point_t *to, point_t *delta);

// Inspection
// WARNING: desc is internally allocated, remember to free() it when done!
void point_inspect(const point_t *p, char **desc);

// "Modal behaviour": a point may have undefined coordinates and if so it must be able to inherit undefined coordinates from the previous point
void point_modal(const point_t *from, point_t *to);

// Getters
data_t point_x(const point_t *p);
data_t point_y(const point_t *p);
data_t point_z(const point_t *p);

#endif // POINT_H