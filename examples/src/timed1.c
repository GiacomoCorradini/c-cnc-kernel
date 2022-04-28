#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // usleep
#include <time.h>

void *wait_thread(void){
  usleep(5000); // us second sleep time
  return NULL;
}

int main(int argc, char const *argv[]) {
  
  pthread_t pt1;
  double t0 = 0, t, dt;
  struct timespec ts;
  int rc, i;

  // get
  clock_gettime(CLOCK_MONOTONIC, &ts);
  t0 = ts.tv_sec + ts.tv_nsec / 1.0E9;

  fprintf(stdout, "n, dt\n");

  for(i = 0; i < 1000; i++){
    rc = pthread_create(&pt1, NULL, wait_thread, NULL);

    clock_gettime(CLOCK_MONOTONIC, &ts);
    t = ts.tv_sec + ts.tv_nsec / 1.0E9;
    dt = t - t0;
    t0 = t;

    fprintf(stdout, "%d, %f\n", i, dt);
    // lets do some work
    usleep(100);

    pthread_join(pt1, NULL);
  }
  
  return 0; 
}
