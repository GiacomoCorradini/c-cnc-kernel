#include "include_guards.h"

void print_args(int argc, const char *argv[]){
  printf("Number of args: #%i\n", argc);
  for(int i = 0; i < argc; i++){
    printf("Args #%i is: %s\n", i, argv[i]);
  }
}

int main(int argc, const char *argv[]){

  print_args(argc, argv);

  if(argc == 1){
    printf("We don't have enough arguments\n");
    return 0;
  }

  double a = atof(argv[1]);

  printf("Symbol test is %s\n", TEST);  
  printf("Symbol TWICE(%f) %f\n", a, TWICE(a));  

  return 0;

}