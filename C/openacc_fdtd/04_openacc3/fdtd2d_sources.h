/**
 * @file fdtd2d_sources.h
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

#ifndef FDTD2D_SOURCES_H
#define FDTD2D_SOURCES_H

#include <stdio.h>
#include "config.h"

void plane_wave_incidence(const struct Range *whole, const struct Range *inside, 
                          FLOAT time, int jpos, FLOAT wavelength, FLOAT *ex, FLOAT *ey);

#endif /* FDTD2D_SOURCES_H */


