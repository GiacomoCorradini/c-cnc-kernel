/**
 * You have to write a C function with the signature
 * double mean(double *v, int len)
 * that takes as arguments a vector of double and its size and returns the arithmetic mean.
 */

#include "stdio.h"

double mean(double *v, int len) {
    int i = 0;
    double mean = 0;
    for (i = 0; i < len; i++) {
        mean += v[i];
    }
    mean /= len;
    return mean;
}

int main(int argc, char const *argv[])
{
    double v[4] = {1, 2, 3, 4};
    printf("%f\n", mean(v, 4));
    return 0;
}
