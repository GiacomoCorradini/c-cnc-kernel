 //   ____  _            _    
//  | __ )| | ___   ___| | __
//  |  _ \| |/ _ \ / __| |/ /
//  | |_) | | (_) | (__|   < 
//  |____/|_|\___/ \___|_|\_\

#include "block.h"
#include <string.h>
#include <ctype.h>

//   ____            _                _   _             
//  |  _ \  ___  ___| | __ _ _ __ ___| |_(_) ___  _ __  
//  | | | |/ _ \/ __| |/ _` | '__/ _ \ __| |/ _ \| '_ \ 
//  | |_| |  __/ (__| | (_| | | |  __/ |_| | (_) | | | |
//  |____/ \___|\___|_|\__,_|_|  \___|\__|_|\___/|_| |_|
                                                     

// Struct for the velocity profile
typedef struct {
  data_t a, d;
  data_t f, l;
  data_t dt_1, dt_m, dt_2;
  data_t dt;
} block_profile_t;

typedef struct block
{
  char *line;
  block_type_t type;
  size_t n; // number of block
  size_t tool; // number of tool
  data_t feedrate; // nominal feed rate
  data_t spindle; // spindle rate
  point_t *target; // final position
  point_t *delta; 
  point_t *center;
  data_t length;
  data_t i, j, r; // parameter for the arch
  data_t theta0, dtheta;
  data_t acc; // actual acceleration
  machine_t *machine;
  block_profile_t *prof;
  struct block *prev;
  struct block *next;
} block_t;

// static = only visible in this file
static int block_set_fields(block_t *b, char cmd, char *arg);

static point_t *point_zero(block_t *b);

static void block_compute(block_t *b);

static void block_arc(block_t *b);



//   _____                 _   _             
//  |  ___|   _ _ __   ___| |_(_) ___  _ __  
//  | |_ | | | | '_ \ / __| __| |/ _ \| '_ \ 
//  |  _|| |_| | | | | (__| |_| | (_) | | | |
//  |_|   \__,_|_| |_|\___|\__|_|\___/|_| |_|
                                          
// Lifecycle
block_t *block_new(const char *line, block_t *prev, machine_t *cfg) {
  assert(line && cfg); // prev is NULL if it is the first block
  block_t *b = (block_t *)calloc(1, sizeof(block_t));
  if(!b){
    perror("Error creating a block");
    exit(EXIT_FAILURE);
  }

  if(prev){ // copy the memory from the previous block
    memcpy(b, prev, sizeof(block_t));
    b->prev = prev;
    prev->next = b;
  } else { // this is the first block
    // nothing to do
  }

  // non-modal g-code parameters: I, J, R
  b->i = b->j = b->r = 0.0;

  // fields to be calculated
  b->length = 0.0;
  b->target = point_new();
  b->delta = point_new();
  b->center = point_new();

  // 
  b->prof = (block_profile_t *)calloc(1, sizeof(block_profile_t));
  if(!b->prof) {
    perror("Error creating a profile structure");
    exit(EXIT_FAILURE);
  }

  b->machine = cfg;
  b->type = NO_MOTION;
  b->acc = machine_A(b->machine);
  b->line = (char *)malloc(strlen(line) + 1);
  strcpy(b->line, line);

  return b;
}

void block_free(block_t *b) {
  assert(b);
  if (b->line) free(b->line);
  if(b->prof) free(b->prof);
  point_free(b->target);
  point_free(b->center);
  point_free(b->delta);
  free(b);
  b = NULL;
}

// parsing the G-code string, Retruns an nteger for success/failure
int block_parse(block_t *b) {
  assert(b);
  char *word, *line, *tofree;
  int rv = 0;
  point_t *p0 = NULL;

  tofree = line = strdup(b->line);
  if(!line) {
    perror("Error copying line");
    exit(EXIT_FAILURE);
  }

  // tokenizing loop
  while ((word = strsep(&line, " ")) != NULL)
  {
    // word[0] is the command
    // word+1 is the pointer to the argument as a string
    rv += block_set_fields(b, toupper(word[0]), word + 1);
  }

  free(tofree);

  // inherit modal fields from the previous block
  p0 = point_zero(b);
  point_modal(p0, b->target);
  point_delta(p0, b->target, b->delta);
  b->length = point_dist(p0, b->target);

  // deal with motion block
  switch (b->type)
  {
  case LINE:
    /* calculate feed profile */
    block_compute(b);
    break;
  case ARC_CW:
  case ARC_CCW:
    /* calculate arc coordinates */
    block_arc(b);
    /* set coorect feedrate and acceleration */
    b->feedrate = MIN(b->feedrate, sqrt(machine_A(b->machine) * b->r) * 60);
    b->acc /= sqrt(2);
    /* calculate feed profile */
    block_compute(b);
    break; 
  default:
    break;
  }

  return rv;
}

// Evaluate the value of lambda at a certain time
data_t block_lambda(const block_t *b, data_t time) {

}

// Interpolation -> projecting lambda in the three coordinates
point_t *block_interpolation(block_t *b, data_t lambda) {

}

// print the block in a file
void block_print(const block_t *b, FILE *out) {
  assert(b);
  assert(out);
  
  char *start, *end;

  point_t *p0 = point_zero(b);

  point_inspect(p0, &start);
  point_inspect(b->target, &end);

  fprintf(out, "%03lu %s->%s F%7.1f S%7.1f T%2lu (%d)\n", b->n, start, end, b->feedrate, b->spindle, b->tool, b->type);

  free(end);
  free(start);
}

// Getters
data_t block_length(const block_t *b) { assert(b); return b->length;
}

data_t block_dtheta(const block_t *b) { assert(b); return b->dtheta;
}

point_t *block_center(const block_t *b) { assert(b); return b->center;
}

static int block_set_fields(block_t *b, char cmd, char *arg) {
  assert(b && arg);
  switch (cmd)
  {
  case 'N':
    b->n = atol(arg);
    break;
  
  case 'G':
    b->type = (block_type_t)atoi(arg);
    break;

  case 'X':
    point_set_x(b->target, atof(arg));
    break;

  case 'Y':
    point_set_y(b->target, atof(arg));
    break;

  case 'Z':
    point_set_z(b->target, atof(arg));
    break;
  
  case 'I':
    b->i = atof(arg);
    break;

  case 'J':
    b->j = atof(arg);
    break;

  case 'R':
    b->r = atof(arg);
    break;

  case 'S':
    b->spindle = atof(arg);
    break;

  case 'F':
    b->feedrate = atof(arg);
    break;

  case 'T':
    b->tool = atol(arg);
    break;

  default:
    fprintf(stderr, "PARSING ERROR: Unexpected G-code command %c%s\n", cmd, arg);
    return 1;
    break;
  }
  
  // cannot have R and I,J on the same block
  if(b->r && (b->i || b->j)) {
    fprintf(stderr, "PARSING ERROR: Cannot mix R and I,J\n");
    return 1;
  }

  return 0;

}

static point_t *point_zero(block_t *b){
  assert(b);
  point_t *p0 = NULL;
  if(b->prev == NULL){
    p0 = point_new();
    point_set_xyz(p0, 0, 0, 0);
  } else {
    p0 = b->prev->target;
  }
  return p0;
}

static data_t quantize(data_t t, data_t tq, data_t *dq){
  data_t q;
  q = ((size_t)(t / tq) + 1) * tq;
  *dq = q - t;
  return q;
}

// calculate the velocity profile
void block_compute(block_t *b) {
  assert(b);
  data_t A, a, d;
  data_t dt, dt_1, dt_2, dt_m, dq;
  data_t f_m, l;

  A = b->acc;
  f_m = b->feedrate / 60.0;
  l = b->length;
  dt_1 = f_m / A;
  dt_2 = dt_1;
  dt_m = l / f_m - (dt_1 + dt_2) / 2.0;
  if(dt_m > 0) { // trapezoidal profile
    dt = quantize(dt_1 + dt_m + dt_2, machine_tq(b->machine), &dq);
    dt_m += dq; 
    f_m = (2 * l) / (dt_1 + dt_2 + 2 * dt_m);
  } else { // triangular profile (short block)
    dt_1 = sqrt(2 * l / A);
    dt_2 = dt_1;
    dt = quantize(dt_1 + dt_2, machine_tq(b->machine), & dq);
    dt_m = 0;
    dt_2 += dq;
    f_m = 2 * l / (dt_1 + dt_2);
  }

  a = f_m / dt_1;
  d = -(f_m / dt_2);

  // set calculated values in block object
  b->prof->dt_1 = dt_1;
  b->prof->dt_2 = dt_2;
  b->prof->dt_m = dt_m;
  b->prof->a = a;
  b->prof->d = d;
  b->prof->f = f_m;
  b->prof->dt = dt;
  b->prof->l = l;



}

// calculate the arc coordinates
void block_arc(block_t *b) {

}

#ifdef BLOCK_MAIN

int main(){

  block_t *b1 = NULL, *b2 = NULL, *b3 = NULL;
  machine_t *cfg = machine_new(NULL);

  b1 = block_new("N10 G00 X90 Y90 Z100 T3", NULL, cfg);
  block_parse(b1);
  b2 = block_new("N20 G01 Y100 X100 F1000 S2000", b1, cfg);
  block_parse(b2);
  b3 = block_new("N30 G01 Y200", b2, cfg);
  block_parse(b3);


  block_print(b1, stdout);
  block_print(b2, stdout);
  block_print(b3, stdout);

  block_free(b1);
  block_free(b2);
  block_free(b3);

  return 0;
}

#endif // BLOCK_MAIN