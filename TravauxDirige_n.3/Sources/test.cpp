#include <omp.h>
#include <iostream>
#include <stdio.h>

int main()
{
#pragma omp parallel
    printf("region sah");
    return 0;
}