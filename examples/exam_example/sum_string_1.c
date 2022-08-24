/**
 * Sum two strings
 **/

#define _GNU_SOURCE
#include "stdio.h"
#include "string.h"

void sum_string(char** dest, char* str1, char* str2);

int main(int argc, char const *argv[])
{
  char str1[] = {"Hello "};
  char str2[] = {"World"};
  char* dest;

  sum_string(&dest, str1, str2);
  printf("%s\n", dest);
  
  return 0;
}

void sum_string(char** dest, char* str1, char* str2){
  char *sum;
  sum = strcat(str1, str2);
  asprintf(dest, "%s", sum);
}