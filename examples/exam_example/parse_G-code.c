/**
 * Parse a G-code line and write it in the terminal
 **/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <sys/param.h> // include ssize_t

// Block object structure
typedef struct block {
  char *line;            // G-code line
  size_t n;              // block number
  int type;              // block type
  double x ,y, z;        // cartesian coordinate
  double i, j, r;        // center coordinates and radius (if it is an arc)
  double feedrate;       // feedrate
  double spindle;        // spindle rate
  size_t tool;           // tool number
} block_t;

int block_parse(block_t *b, char *line_k);
int block_set_fields(block_t *b, char cmd, char *arg);
void block_print(block_t *b);

int main(int argc, const char *argv[]) {

  ssize_t line_len = 0;
  char *line = NULL;
  size_t n = 0;
  block_t *b;

  if (argc != 2) {
    fprintf(stderr, "I need 2 arguments!\n");
    return EXIT_FAILURE;
  }

  const char *filename = argv[1];

  FILE *f = fopen(filename, "r");
  if (!f) {
    fprintf(stderr, "ERROR: cannot open the file %s\n", filename);
    return EXIT_FAILURE;
  }

  while ( (line_len = getline(&line, &n, f)) >= 0 ) {
    
    // remove trailing newline (\n) replacing it with a terminator
    if (line[line_len-1] == '\n') {
      line[line_len-1] = '\0'; 
    }
    if(!(b = (block_t *)calloc(1, sizeof(block_t)))) {
      fprintf(stderr, "ERROR: creating the block %s\n", line);
      return EXIT_FAILURE;
    }
    if (block_parse(b, line)) {
      fprintf(stderr, "ERROR: parsing the block %s\n", line);
      return EXIT_FAILURE;
    }
    block_print(b);
  }

  free(b);
  fclose(f);
  return 0;
}

// Parsing the G-code string. Returns an integer for success/failure
int block_parse(block_t *b, char *line_k) {
  assert(b);
  char *word, *line, *tofree;
  int rv = 0;

  tofree = line = strdup(line_k);
  if (!line) {
    perror("Could not allocate momory for tokenizing line");
    return 1;
  }
  // Tokenizing loop
  while ((word = strsep(&line, " ")) != NULL) {
    // word[0] is the command
    // word+1 is the pointer to the argument as a string
    rv += block_set_fields(b, toupper(word[0]), word + 1);
  }
  free(tofree);
  return rv;
}

// set fileds of the block
int block_set_fields(block_t *b, char cmd, char *arg) {
  assert(b && arg);
  switch (cmd)
  {
  case 'N':
    b->n = atol(arg);
    break;
  case 'G':
    b->type = atoi(arg);
    break;
  case 'X':
    b->x = atof(arg);
    break;
  case 'Y':
    b->y = atof(arg);
    break;
  case 'Z':
    b->z = atof(arg);
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
  case 'F':
    b->feedrate = atof(arg);
    break;
  case 'S':
    b->spindle = atof(arg);
    break; 
  case 'T':
    b->tool = atol(arg);
    break;
  default:
    fprintf(stderr, "ERROR: Usupported G-code command %c%s\n", cmd, arg);
    return 1;
    break;
  }
  return 0;
}

// print the block
void block_print(block_t *b) {
  fprintf(stdout, "N%03lu G%02d X%8.2f Y%8.2f Z%8.2f F%8.1f S%8.1f T%2lu\n", b->n, b->type, b->x, b->y, b->z, b->feedrate, b->spindle, b->tool);
}