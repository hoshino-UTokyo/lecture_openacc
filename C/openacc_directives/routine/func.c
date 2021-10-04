#include <stdio.h>
#include <stdlib.h>
#include "func.h" // Check this file!

#pragma acc routine seq
int func(int a, int b){
    return a * b;
}
