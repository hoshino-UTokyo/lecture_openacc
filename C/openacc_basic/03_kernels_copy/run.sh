#!/bin/bash
#PJM -L rscgrp=lecture-a
#PJM -L gpu=1
#PJM -L elapse=00:01:00
#PJM -g gt00

module load nvidia 

./run
