/**
 * @file config.h
 * @brief (File brief)
 *
 * (File explanation)
 *
 * @author Takashi Shimokawabe
 * @date 2017/11/10 Created
 * @version 0.1.0
 *
 * $Header$ 
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <mpi.h>

#ifdef USE_FLOAT
typedef float FLOAT;
#define MPI_FLOAT_T MPI_FLOAT
#else
typedef double FLOAT;
#define MPI_FLOAT_T MPI_DOUBLE
#endif

struct Range {
    int length[2];
    int begin [2];
};

struct Constant {
    const FLOAT pi;
    const FLOAT c;
    const FLOAT m0;
    const FLOAT e0;
    const FLOAT z0;
};

extern struct Constant constant;

#endif /* CONFIG_H */


