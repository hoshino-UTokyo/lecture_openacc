#include <stdio.h>
#include <stdlib.h>

int main()
{
    const int n = 1000;
    
    float *a = malloc(n*sizeof(float));
    float *b = malloc(n*sizeof(float));
    float  c = 2.0;
    
    for (int i=0; i<n; i++) {
        a[i] = 10.0;
    }
#pragma acc data copyin(a[0:n]), copyout(b[0:n])
#pragma acc kernels 
#pragma acc loop independent
    for (int i=0; i<n; i++) {
        b[i] = a[i] + c;
    }
    
    double sum = 0;
    for (int i=0; i<n; i++) {
        sum += b[i];
    }
    fprintf(stdout, "%f\n", sum/n);

    free(a); free(b);
    
    return 0;
}

