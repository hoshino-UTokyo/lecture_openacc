

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "diffusion.h"
#include "misc.h"

int main(int argc, char *argv[])
{
    const int nx = 128;
    const int ny = nx;
    const int nz = nx;
    const int n  = nx*ny*nz;

    const float lx = 1.0;
    const float ly = 1.0;
    const float lz = 1.0;
    
    const float dx = lx/(float)nx;
    const float dy = ly/(float)ny;
    const float dz = lz/(float)nz;

    const float kappa = 0.1;
    const float dt    = 0.1*fmin(fmin(dx*dx, dy*dy), dz*dz)/kappa;

    const int   nt = 100000;
    double time = 0.0;
    int    icnt = 0;
    double flop = 0.0;
    double elapsed_time = 0.0;
    
    float *f  = (float *)malloc(sizeof(float)*n);
    float *fn = (float *)malloc(sizeof(float)*n);

    init(nx, ny, nz, dx, dy, dz, f);

    start_timer();
    
    for (; icnt<nt && time + 0.5*dt < 0.1; icnt++) {
        if (icnt % 100 == 0) fprintf(stdout, "time(%4d) = %7.5f\n", icnt, time);
            
        flop += diffusion3d(nx, ny, nz, dx, dy, dz, dt, kappa, f, fn);
        
        swap(&f, &fn);

        time += dt;
    }
    
    elapsed_time = get_elapsed_time();
    
    fprintf(stdout, "Time = %8.3f [sec]\n", elapsed_time);
    fprintf(stdout, "Performance= %7.2f [GFlops]\n",flop/elapsed_time*1.0e-09);
    
    const double ferr = accuracy(time, nx, ny, nz, dx, dy, dz, kappa, f);
    fprintf(stdout, "Error[%d][%d][%d] = %10.6e\n", nx, ny, nz, ferr);
    
    free(f);  f  = NULL;
    free(fn); fn = NULL;

    return 0;
}

