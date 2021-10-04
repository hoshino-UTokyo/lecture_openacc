#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    unsigned int data[20] = {0,1,5,2,3,2,2,3,1,0,4,1,1,1,2,5,2,3,0,1};
    unsigned int histgram[6] = {0};
    printf("input data:");
    for(int i = 0;i < 20;i++) printf("%d ",data[i]);
    printf("\n");

#pragma acc kernels copyin(data) copyout(histgram)
#pragma acc loop independent
    for(int j = 0;j < 6;j++){
#pragma acc loop independent
	for(int i = 0;i < 20;i++){
	    if(data[i] == j){
#pragma acc atomic 
		histgram[j]++;
	    }
	}
    }
    printf("Count numbers\n");
    for(int j = 0;j < 6;j++) printf("Number of (%d): %d\n",j,histgram[j]);

    return 0;
}
