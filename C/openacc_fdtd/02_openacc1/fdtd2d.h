/**
 * @file fdtd2d.h
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

#ifndef FDTD2D_H
#define FDTD2D_H

#include <stdio.h>
#include "config.h"

void calc_ex_ey(const struct Range *whole, const struct Range *inside,
                const FLOAT *hz, const FLOAT *cexly, const FLOAT *ceylx, FLOAT *ex, FLOAT *ey);
void calc_hz(const struct Range *whole, const struct Range *inside,
             const FLOAT *ey, const FLOAT *ex, const FLOAT *chzlx, const FLOAT *chzly, FLOAT *hz);

void pml_boundary_ex(const struct Range *whole, const struct Range *inside,
                     const FLOAT *hz, const FLOAT *cexy, const FLOAT *cexyl, const FLOAT *rer_ex,
                     FLOAT *ex, FLOAT *exy);
void pml_boundary_ey(const struct Range *whole, const struct Range *inside,
                     const FLOAT *hz, const FLOAT *ceyx, const FLOAT *ceyxl, const FLOAT *rer_ey,
                     FLOAT *ey, FLOAT *eyx);
void pml_boundary_hz(const struct Range *whole, const struct Range *inside,
                     const FLOAT *ey, const FLOAT *ex,
                     const FLOAT *chzx, const FLOAT *chzxl, const FLOAT *chzy, const FLOAT *chzyl,
                     FLOAT *hz, FLOAT *hzx, FLOAT *hzy);

#endif /* FDTD2D_H */


