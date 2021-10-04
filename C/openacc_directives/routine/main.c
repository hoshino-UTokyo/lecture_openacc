#include <stdio.h>
#include <stdlib.h>

#include "func.h" // Check this file!

int main(int argc, char *argv[])
{
    int a[20] = {4,1,5,2,3,2,2,3,1,2,4,1,1,1,2,5,2,3,5,1};
    int b[20] = {5,5,2,3,2,3,1,4,1,1,3,5,3,2,4,2,2,3,2,3};
    int c[20] = {0};
    printf("array a:");
    for(int i = 0;i < 20;i++) printf("%3d",a[i]);
    printf("\n");
    printf("array b:");
    for(int i = 0;i < 20;i++) printf("%3d",b[i]);
    printf("\n");

#pragma acc kernels copyin(a,b) copyout(c)
#pragma acc loop independent
    for(int i = 0;i < 20;i++){
	c[i] = func(a[i],b[i]);
    }
    printf("array c:");
    for(int i = 0;i < 20;i++) printf("%3d",c[i]);
    printf("\n");

    return 0;
}
