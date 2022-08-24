// Write a function that counts the number of non-empty lines in a file. An empty line is a line that contains nothing
// except the newline character.
// The function must have exactly this signature:

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int count_lines(FILE *f);

#define MAX_LINE_LENGTH 1024

typedef enum {
  ERROR_SUCCESS=0,
  ERROR_ARGS,
  ERROR_READ,
  ERROR_WRITE,
  ERROR_OPEN
} errors_t;

int main(int argc, const char *argv[]) {

  int new_line = 0;

  if (argc != 2) {
    fprintf(stderr, "I need 2 arguments!\n");
    return ERROR_ARGS;
  }

  const char *filename = argv[1];

  FILE *f = fopen(filename, "r");
  char buffer[MAX_LINE_LENGTH];

  do{
    new_line++;
  } while(fgets(buffer, MAX_LINE_LENGTH, f));

  fprintf(stdout, "The number of empty lines are: %d\n", new_line);

  fclose(f);
  return ERROR_SUCCESS;
}