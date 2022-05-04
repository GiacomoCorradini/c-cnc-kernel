//   ____  _            _
//  | __ )| | ___   ___| | __
//  |  _ \| |/ _ \ / __| |/ /
//  | |_) | | (_) | (__|   <
//  |____/|_|\___/ \___|_|\_\

#ifndef BLOCK_H
#define BLOCK_H

#include "defines.h"
#include "machine.h"
#include "point.h"

//   _____
//  |_   _|   _ _ __   ___  ___
//    | || | | | '_ \ / _ \/ __|
//    | || |_| | |_) |  __/\__ \
//    |_| \__, | .__/ \___||___/
//        |___/|_|

// OPAQUE structure representing a G-code block
typedef struct block block_t;

// Define block type
typedef enum {
  RAPID = 0,
  LINE = 1,
  ARC_CW = 2,
  ARC_CCW = 3,
  NO_MOTION = 4
} block_type_t;

//   _____                 _   _
//  |  ___|   _ _ __   ___| |_(_) ___  _ __
//  | |_ | | | | '_ \ / __| __| |/ _ \| '_ \ 
//  |  _|| |_| | | | | (__| |_| | (_) | | | |
//  |_|   \__,_|_| |_|\___|\__|_|\___/|_| |_|

// LIFECYCLE ======================================================

block_t *block_new(const char *line, block_t *prev, machine_t *cfg);

void block_free(block_t *b);

// print the block in a file
void block_print(block_t *b, FILE *out);

// ALGORITHMS =====================================================

// parsing the G-code string, Retruns an nteger for success/failure
int block_parse(block_t *b);

// Evaluate the value of lambda at a certain time
// also return speed in the parameter v
data_t block_lambda(const block_t *b, data_t t, data_t *v);

// Interpolation -> projecting lambda in the three coordinates
point_t *block_interpolation(block_t *b, data_t lambda);

// GETTERS ========================================================

data_t block_length(const block_t *b);

data_t block_dtheta(const block_t *b);

data_t block_dt(const block_t *b);

data_t block_r(const block_t *b);

block_type_t block_type(const block_t *b);

char *block_line(const block_t *b);

size_t block_n(const block_t *b);

point_t *block_center(const block_t *b);

block_t *block_next(const block_t *b);


#endif // BLOCK_H