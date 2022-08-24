/**
 * Read from file ini and calculate the parameters of a milling machine
 **/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../../src/defines.h"
#include "inic.h"
#include "math.h"

#define PI 3.1415

typedef struct
{
  data_t ps;   // specific energy
  data_t v;    // cutting speed
  data_t D;    // mill diameter
  data_t w;    // milling width
  data_t z;    // tool teeth
  data_t d;    // depth of cut
  data_t fz;   // feed per tooth
} read_filie_ini;

typedef struct
{
  data_t N;    // rotational speed
  data_t f;    // feedrate
  data_t MRR;  // MRR
  data_t p;    // mechanical power
} results;

results* computation(read_filie_ini* param);

int main(int argc, char const *argv[])
{
  int rc = 0;
  void *ini = NULL;
  const char* name = "milling.ini";
  read_filie_ini* param = (read_filie_ini *)calloc(1, sizeof(read_filie_ini));
  results* res = (results *)calloc(1, sizeof(results));

  ini = ini_init(name);
  if (!ini) {
      fprintf(stderr, "Could not open the ini file %s\n", name);
      return 1;
    }

  rc += ini_get_double(ini, "MILLING", "ps", &param->ps);
  rc += ini_get_double(ini, "MILLING", "v", &param->v);
  rc += ini_get_double(ini, "MILLING", "D", &param->D);
  rc += ini_get_double(ini, "MILLING", "w", &param->w);
  rc += ini_get_double(ini, "MILLING", "z", &param->z);
  rc += ini_get_double(ini, "MILLING", "d", &param->d);
  rc += ini_get_double(ini, "MILLING", "fz", &param->fz);

  if (rc > 0) {
      fprintf(stderr, "Missing/wrong %d config parameters\n", rc);
      return 1;
    }

  res = computation(param);

  fprintf(stdout, "The rotational speed is: %.0f rpm\n", res->N);
  fprintf(stdout, "The feedrate is: %.0f mm/min\n", res->f);
  fprintf(stdout, "The MRR is: %.0f mm^3/min\n", res->MRR);
  fprintf(stdout, "The mechanical power is: %.1f kW\n", res->p);

  free(param);
  free(res);

  return 0;
}

results* computation(read_filie_ini* param){
  results* res = (results *)calloc(1, sizeof(results));
  res->N = param->v / (PI * (param->D/1000));
  res->f = param->fz * param->z * res->N;
  res->MRR = param->w * param->d * res->f;
  res->p = res->MRR * (param->ps/1000000);
  return res;
}