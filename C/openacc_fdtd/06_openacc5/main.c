#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <math.h>
#include <openacc.h>
#include "config.h"
#include "setup.h"
#include "fdtd2d.h"
#include "fdtd2d_sources.h"
#include "output.h"

void set_object_er(const struct Range *whole,
                   FLOAT lx, FLOAT ly, FLOAT dx, FLOAT dy, int *obj, FLOAT *er);
double get_elapsed_time(const struct timeval *tv0, const struct timeval *tv1);
FLOAT get_dt(FLOAT dx, FLOAT dy);    

int main(int argc, char *argv[])
{
    int nprocs = 1;
    int rank   = 0;
    
    if (argc != 6) {
        if (rank == 0) {
            fprintf(stdout, "%s <nx> <ny> <nsubdomains> <nt> <nout>\n", argv[0]);
        }
        return 1;
    }

    const int ngpus = acc_get_num_devices(acc_device_nvidia);
    if (rank == 0) {
        fprintf(stdout, "num of GPUs = %d\n", ngpus);
    }
    const int gpuid = ngpus > 0 ? rank % ngpus : -1;
    if (gpuid >= 0) {
        acc_set_device_num(gpuid, acc_device_nvidia);
    }

    for (int r=0; r<nprocs; r++) {
        if (r != rank) continue;
        
        char hostname[128];
        gethostname(hostname, sizeof(hostname));
        fprintf(stdout, "Rank %d: hostname = %s, GPU num = %d\n", rank, hostname, gpuid);
        fflush(stdout);
    }

    const int mgn = 8;
    const int nsubdomains            = atoi(argv[3]);
    const struct Range inside_global = { { atoi(argv[1]), atoi(argv[2]) },
                                         { 0, 0 } };
    const struct Range whole_global  = { { inside_global.length[0] + 2*mgn + 1, inside_global.length[1] + 2*mgn + 1},
                                         { inside_global.begin[0]  - mgn      , inside_global.begin[1]  - mgn   } };
    
    const struct Range inside        = { { inside_global.length[0], inside_global.length[1]/nsubdomains },
                                         { 0, inside_global.length[1]/nsubdomains * rank } };
    const struct Range whole         = { { inside.length[0] + 2*mgn + 1, inside.length[1] + 2*mgn + 1},
                                         { inside.begin[0]  - mgn      , inside.begin[1]  - mgn   } };

    if (inside_global.length[1] != inside.length[1] * nsubdomains) {
        if (rank == 0) {
            fprintf(stdout, "Error: \n");
        }
        return 1;
    }

    // Setting for MPI comm
    const int rank_up   = rank != nprocs - 1 ? rank + 1 : MPI_PROC_NULL;
    const int rank_down = rank != 0          ? rank - 1 : MPI_PROC_NULL;

    const FLOAT wavelength = 500.0*1.0e-9; // m
    const FLOAT dx         = 10.0*1.0e-9;
    const FLOAT dy         = dx;
    const FLOAT dt         = get_dt(dx, dy);
    const FLOAT lx         = dx * inside_global.length[0];
    const FLOAT ly         = dy * inside_global.length[1];
    
    const int  nt          = atoi(argv[4]);
    const int  nout        = atoi(argv[5]);
    const bool output_file = nout <= 0 ? false : true;

    if (rank == 0) {
        fprintf(stdout, "Calculation condition\n");
        fprintf(stdout, "  nx_global     = %5d\n", inside_global.length[0]);
        fprintf(stdout, "  ny_global     = %5d\n", inside_global.length[1]);
        fprintf(stdout, "  nx            = %5d\n", inside.length[0]);
        fprintf(stdout, "  ny            = %5d\n", inside.length[1]);
        fprintf(stdout, "  nsubdomains   = %5d\n", nsubdomains);
        fprintf(stdout, "  mgn           = %5d\n", mgn);
        fprintf(stdout, "  lx            = %5e [m]\n", lx);
        fprintf(stdout, "  ly            = %5e [m]\n", ly);
        fprintf(stdout, "  dx            = %5e [m]\n", dx);
        fprintf(stdout, "  dy            = %5e [m]\n", dy);
        fprintf(stdout, "  dt            = %5e [sec]\n", dt);
        fprintf(stdout, "  wl            = %5e\n", wavelength);
        fprintf(stdout, "  nt            = %5d\n", nt);
        fprintf(stdout, "  nout          = %5d\n", nout);
        fprintf(stdout, "  output        = %5d\n", output_file);
        fprintf(stdout, "  sizeof(FLOAT) = %5d\n", sizeof(FLOAT));
    }
    
    const int    nelems      = whole.length[0] * whole.length[1];
    const int    nelems_x    = whole.length[0];
    const int    nelems_y    = whole.length[1];
    const size_t size        = sizeof(FLOAT)*nelems;
    const size_t size_x      = sizeof(FLOAT)*nelems_x;
    const size_t size_y      = sizeof(FLOAT)*nelems_y;
    const size_t size_global = sizeof(FLOAT)* whole_global.length[0] * whole_global.length[1];
    
    FLOAT *ex    = (FLOAT *)malloc(size);
    FLOAT *ey    = (FLOAT *)malloc(size);
    FLOAT *hz    = (FLOAT *)malloc(size);
    FLOAT *cexly = (FLOAT *)malloc(size);
    FLOAT *ceylx = (FLOAT *)malloc(size);
    FLOAT *chzlx = (FLOAT *)malloc(size);
    FLOAT *chzly = (FLOAT *)malloc(size);

    FLOAT *exy   = (FLOAT *)malloc(size);
    FLOAT *eyx   = (FLOAT *)malloc(size);
    FLOAT *hzx   = (FLOAT *)malloc(size);
    FLOAT *hzy   = (FLOAT *)malloc(size);
    
    FLOAT *cexy  = (FLOAT *)malloc(size_y);
    FLOAT *ceyx  = (FLOAT *)malloc(size_x);
    FLOAT *chzx  = (FLOAT *)malloc(size_x);
    FLOAT *chzy  = (FLOAT *)malloc(size_y);

    FLOAT *cexyl = (FLOAT *)malloc(size_y);
    FLOAT *ceyxl = (FLOAT *)malloc(size_x);
    FLOAT *chzxl = (FLOAT *)malloc(size_x);
    FLOAT *chzyl = (FLOAT *)malloc(size_y);
    
    int   *obj    = (int   *)malloc(sizeof(int)*nelems); // Objects
    FLOAT *er     = (FLOAT *)malloc(size);               // Relative Permittivity
    FLOAT *rer_ex = (FLOAT *)malloc(size);
    FLOAT *rer_ey = (FLOAT *)malloc(size);

    // For output
    FLOAT *ex_global = (FLOAT *)malloc(size_global);
    FLOAT *ey_global = (FLOAT *)malloc(size_global);
    FLOAT *hz_global = (FLOAT *)malloc(size_global);

    
    init_relative_permittivity(whole.length, 1.0, er); // vacuum
    init_object(whole.length, obj);

    // User-defined function
    set_object_er(&whole, lx, ly, dx, dy, obj, er);    


    init_vars(whole.length, ex, ey, hz);
    
    set_initial_condition(whole.length, dt, dx, dy, constant.e0, er, constant.m0, obj, 
			  cexly, ceylx, chzlx, chzly);
    
    init_pml_vars(whole.length, exy, eyx, hzx, hzy);
    set_pml_initial_condition(&whole, &inside, dt, dx, dy, constant.c, constant.e0, constant.m0,
			      cexy, ceyx, chzx, chzy, cexyl, ceyxl, chzxl, chzyl);
    set_pml_rer(whole.length, obj, er, rer_ex, rer_ey);
    
    init_vars(whole_global.length, ex_global, ey_global, hz_global);
    
    struct timeval tv0;
    struct timeval tv1;
    
    gettimeofday(&tv0, NULL);
    
    int icnt = 0;
    FLOAT time = 0.0;
    if (rank == 0) {
      fprintf(stdout, "icnt = %5d, time = %6.4e [sec]\n", icnt, time);
    }
    
    if (output_file) {
      
      const int rank_root  = 0;
      const int sendnelems = whole.length[0] * inside.length[1];
      const int src        = whole.length[0] * (inside.begin[1] - whole.begin[1]);
      const int dst        = whole.length[0] * (inside.begin[1] - whole.begin[1]);
      
      int i;
#pragma acc kernels
#pragma acc loop independent
      for(i = 0;i < sendnelems;i++){
	ex_global[dst+i] = ex[src+i];
	ey_global[dst+i] = ey[src+i];
	hz_global[dst+i] = hz[src+i];
      }
      
      if (rank == rank_root) {
	write_bmp(icnt, time, whole_global.length, dx, dy, ex_global, ey_global, hz_global);
      }
    }
    
    while (icnt < nt) {
      
      const int tag = 0;
      const int nhalo       = whole.length[0];
      const int inside_end1 = inside.begin[1] + inside.length[1];
      
      const int src_hz      = whole.length[0] * (inside_end1     - whole.begin[1] - 1);
      const int dst_hz      = whole.length[0] * (inside.begin[1] - whole.begin[1] - 1);
      
      calc_ex_ey(&whole, &inside, hz, cexly, ceylx, ex, ey);
      pml_boundary_ex(&whole, &inside, hz, cexy, cexyl, rer_ex, ex, exy);
      pml_boundary_ey(&whole, &inside, hz, ceyx, ceyxl, rer_ey, ey, eyx);
      
      
      const int j_in = 0;
      plane_wave_incidence(&whole, &inside, time, j_in, wavelength, ex, ey);
      time += 0.5*dt;
      
      
      const int src_ex      = whole.length[0] * (inside.begin[1] - whole.begin[1]);
      const int dst_ex      = whole.length[0] * (inside_end1     - whole.begin[1]);
      
      
      calc_hz(&whole, &inside, ey, ex, chzlx, chzly, hz);
      pml_boundary_hz(&whole, &inside, ey, ex, chzx, chzxl, chzy, chzyl, hz, hzx, hzy);
      time += 0.5*dt;
      
      icnt++;
      if (rank == 0 && icnt % 100 == 0) {
	fprintf(stdout, "icnt = %5d, time = %6.4e [sec]\n", icnt, time);
      }
      
      if (output_file && icnt % nout == 0) {
	
	const int rank_root  = 0;
	const int sendnelems = whole.length[0] * inside.length[1];
	const int src        = whole.length[0] * (inside.begin[1] - whole.begin[1]);
	const int dst        = whole.length[0] * (inside.begin[1] - whole.begin[1]);
	
	int i;
#pragma acc kernels
#pragma acc loop independent
	for(i = 0;i < sendnelems;i++){
	  ex_global[dst+i] = ex[src+i];
	  ey_global[dst+i] = ey[src+i];
	  hz_global[dst+i] = hz[src+i];
	}
        
	if (rank == rank_root) {
	  write_bmp(icnt, time, whole_global.length, dx, dy, ex_global, ey_global, hz_global);
	}
        
      }
    }
    
    gettimeofday(&tv1, NULL);
    
    const double elapsed_time = get_elapsed_time(&tv0, &tv1);
    if (rank == 0) {
      fprintf(stdout, "------------------------------\n");
      fprintf(stdout, "Domain      = %d x %d\n", inside_global.length[0], inside_global.length[1]);
      fprintf(stdout, "nsubdomains = %d\n", nsubdomains);
      fprintf(stdout, "GPU is used = %d\n", ngpus > 0);
      fprintf(stdout, "output_file = %d\n", output_file);
      fprintf(stdout, "Time        = %10.6f [sec]\n", elapsed_time);
      fprintf(stdout, "------------------------------\n");
    }
    
    free(ex);
    free(ey);
    free(hz);
    free(cexly);
    free(ceylx);
    free(chzlx);
    free(chzly);

    free(exy);
    free(eyx);
    free(hzx);
    free(hzy);

    free(cexy);
    free(ceyx);
    free(chzx);
    free(chzy);

    free(cexyl);
    free(ceyxl);
    free(chzxl);
    free(chzyl);
    
    free(obj);
    free(er);
    free(rer_ex);
    free(rer_ey);

    free(ex_global);
    free(ey_global);
    free(hz_global);
    
}

void set_object_er(const struct Range *whole,
                   FLOAT lx, FLOAT ly, FLOAT dx, FLOAT dy, int *obj, FLOAT *er)
{
    const FLOAT y0 = 0.5*ly;
    const FLOAT y1 = y0 + 0.1*ly;
    const FLOAT x0 = 0.45*lx;
    const FLOAT x1 = 0.55*lx;

    // Set relative permittivity
    const FLOAT relative_permittivity = 5.4;

    // Set media and objects
    for (int jj=0; jj<whole->length[1]; jj++) {
        for (int ii=0; ii<whole->length[0]; ii++) {
            const int ix = ii + jj*whole->length[0];
            const int i = ii + whole->begin[0];
            const int j = jj + whole->begin[1];

            const FLOAT x = (i+0.5) * dx;
            const FLOAT y = (j+0.5) * dy;

            if (y >= y0 && y <= y1 &&
                (x <= x0 || x >= x1)) {
                obj[ix] = 1;
            }
            
            if (y >= -0.25 * (x-lx) + y1) {
                er[ix] = relative_permittivity;
            }
                
        }
    }

}

FLOAT get_dt(FLOAT dx, FLOAT dy)
{
    const FLOAT c = constant.c;

    const FLOAT coef = 0.2;

    const FLOAT rdx = 1.0/dx;
    const FLOAT rdy = 1.0/dy;
    
    return coef * 1.0 / (c * sqrt(rdx*rdx + rdy*rdy));
}

double get_elapsed_time(const struct timeval *tv0, const struct timeval *tv1)
{
    return (double)(tv1->tv_sec - tv0->tv_sec) + (double)(tv1->tv_usec - tv0->tv_usec)*1.0e-6;
}



