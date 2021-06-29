/**
 * @file config.c
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

#include "config.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

#define pi (M_PI)
#define c  (2.99792458 *1.0e+8) // m/s
#define m0 (4.0*pi *1.0e-7)
#define e0 (1.0/(m0*c*c))
#define z0 (m0*c) // = sqrt(m0/e0)

struct Constant constant = { pi, c, m0, e0, z0 };



