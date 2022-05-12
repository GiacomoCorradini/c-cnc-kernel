#include "../defines.h"
#include "../machine.h"
#include "../program.h"
#include "../block.h"
#include "../point.h"

// define a macro
// "..."" means multiple arguments -> they passes as "__VA_ARGS__" to the function
#define eprintf(...) fprintf(stderr, __VA_ARGS__)

int main(int argc, char const *argv[]) {

  point_t *sp = NULL; // good idea initialize a point to NULL
  block_t *b = NULL;
  program_t *p = NULL;
  data_t t, tq, lambda, f;
  machine_t *machine = machine_new("settings.ini");
  if (!machine) {
    eprintf("Error creating machine instance\n");
    exit(EXIT_FAILURE);
  }

  tq = machine_tq(machine);

  p = program_new(argv[1]); // we assume to pass the name of the program as first argument given to the command

  if(!p) {
    eprintf("Could not create program, exiting. \n");
    exit(EXIT_FAILURE);
  }

  if(program_parse(p, machine) == EXIT_FAILURE){
    eprintf("Could not parse program in %s, exiting.\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  program_print(p, stderr);

  // main loop
  printf("n, lambda, s, f, x, y, z\n");
  while ((b = program_next(p))) {
    if(block_type(b) == RAPID || block_type(b) > ARC_CCW){
      continue;
    }
    eprintf("Interpolating the block %s\n", block_line(b));
    
    //interpolation loop
    for (t = 0; t <= block_dt(b); t += tq) {
      lambda = block_lambda(b, t, &f);
      sp = block_interpolation(b, lambda);
      if(!sp) continue;
      printf("%lu,%f,%f,%f,%f,%f,%f,%f\n", block_n(b), t, lambda, lambda * block_length(b), f, point_x(sp), point_y(sp), point_z(sp));
      //point_free(sp);
      machine_sync(machine);
    }
  }

  machine_free(machine);
  program_free(p);
  return 0;
}
