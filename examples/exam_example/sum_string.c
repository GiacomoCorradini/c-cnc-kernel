/**
 * Sum two strings
 **/

#include "stdio.h"
#include "string.h"

char* sum_string(char* str1, char* str2);

int main(int argc, char const *argv[])
{
  char str1[30];
  char str2[30];
  char* dest;
  strcpy(str1, "Hello ");
  strcpy(str2, "World");

  dest = sum_string(str1, str2);
  printf("%s\n", dest);
  
  return 0;
}

char* sum_string(char* str1, char* str2){
  char* dest;
  dest = strcat(str1, str2);
  return dest;
}
