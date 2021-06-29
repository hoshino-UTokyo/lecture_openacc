/**
 * @file setup.h
 * @brief (File brief)
 *
 * (File explanation)
 *
 * @author Takashi Shimokawabe
 * @date 2017/11/09 Created
 * @version 0.1.0
 *
 * $Header$ 
 */

#ifndef SETUP_H
#define SETUP_H

#include <stdio.h>
#include "config.h"

void init_relative_permittivity(const int length[], FLOAT relative_permittivity, FLOAT *er);
void init_object(const int length[], int *obj);

void init_vars(const int length[], FLOAT *ex, FLOAT *ey, FLOAT *hz);
void set_initial_condition(const int length[], FLOAT dt, FLOAT dx, FLOAT dy,
                           FLOAT e0, const FLOAT *er, FLOAT m0, const int *obj,
                           FLOAT *cexly, FLOAT *ceylx, FLOAT *chzlx, FLOAT *chzly);

void init_pml_vars(const int length[], FLOAT *exy, FLOAT *eyx, FLOAT *hzx, FLOAT *hzy);
void set_pml_initial_condition(const struct Range *whole, const struct Range *inside,
                               FLOAT dt, FLOAT dx, FLOAT dy,
                               FLOAT c, FLOAT e0, FLOAT m0, 
                               FLOAT *cexy, FLOAT *ceyx, FLOAT *chzx, FLOAT *chzy,
                               FLOAT *cexyl, FLOAT *ceyxl, FLOAT *chzxl, FLOAT *chzyl);
void set_pml_rer(const int length[], const int *obj, const FLOAT *er, FLOAT *rer_ex, FLOAT *rer_ey);

#endif /* SETUP_H */


