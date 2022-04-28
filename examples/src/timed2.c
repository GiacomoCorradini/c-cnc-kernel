#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // usleep
#include <time.h>
#include <sys/time.h>

void handler(void) {}

int main(int argc, char const *argv[])
{
  int i = 0;
  double t0 = 0, t, dt;
  struct timespec ts;
  struct itimerval itv;

  // configure our time
  itv.it_interval.tv_sec = 0;
  itv.it_interval.tv_usec = 5000; // 50 ms
  itv.it_value.tv_sec = 0;
  itv.it_value.tv_usec = 5000;

  // get the signal
  signal(SIGALRM, handler);

  // set the timer
  if(setitimer(ITIMER_REAL, &itv, NULL)) {
    perror("I was not able to set the timer!");
  }

  // Csv header
  fprintf(stdout, "n, dt\n");

  clock_gettime(CLOCK_MONOTONIC, &ts);
  t0 = ts.tv_sec + ts.tv_nsec / 1.0E9;

  for (i = 0; i < 1000; i++)
  {
    sleep(1);

    clock_gettime(CLOCK_MONOTONIC, &ts);
    t = ts.tv_sec + ts.tv_nsec / 1.0E9;
    dt = t -t0;
    t0 = t;

    fprintf(stdout, "%d %f\n", i, dt);
  }
  

  return 0;
}


