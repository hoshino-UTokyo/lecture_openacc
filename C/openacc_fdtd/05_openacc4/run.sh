#!/bin/bash
#PJM -L rscgrp=lecture-a
#PJM -L gpu=1
#PJM --mpi proc=1
#PJM -L elapse=00:10:00
#PJM -g gt00




module load nvidia/21.3 ompi-cuda/4.1.1-11.2

mkdir -p sim_run
cd sim_run

nprocs=1
mpirun -np $nprocs ../run 512 512 $nprocs 5000 50
