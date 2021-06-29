#!/bin/bash
#PJM -L rscgrp=lecture-a
#PJM -L gpu=1
#PJM -L elapse=00:10:00
#PJM -g gt00

cd $PJM_O_WORKDIR

. /etc/profile.d/modules.sh
module load nvidia/21.3 intel/2021.2.0

./run
