/**
 * @file setup.cc
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

#include <math.h>
#include "setup.h"

void init_relative_permittivity(const int length[], FLOAT relative_permittivity, FLOAT *er)
{
    const int n = length[0]*length[1];
    for (int i=0; i<n; i++) {
        er[i] = relative_permittivity;
    }
}

void init_object(const int length[], int *obj)
{
    const int n = length[0]*length[1];
    for (int i=0; i<n; i++) {
        obj[i] = 0;
    }
}

void init_vars(const int length[], FLOAT *ex, FLOAT *ey, FLOAT *hz)
{
    const int n = length[0]*length[1];

    for (int i=0; i<n; i++) {
        ex[i] = 0.0;
        ey[i] = 0.0;
        hz[i] = 0.0;
    }
}

void set_initial_condition(const int length[], FLOAT dt, FLOAT dx, FLOAT dy,
                           FLOAT e0, const FLOAT *er, FLOAT m0, const int *obj,
                           FLOAT *cexly, FLOAT *ceylx, FLOAT *chzlx, FLOAT *chzly)
{
    for (int j=0; j<length[1]; j++) {
        for (int i=0; i<length[0]; i++) {
            const int ix = i + j*length[0];
            const int jm = i + (j-1)*length[0];
            const int im = (i-1) + j*length[0];

            const FLOAT er_ex = j != 0 ? 0.5*(er[ix] + er[jm]) : er[ix];
            const FLOAT er_ey = i != 0 ? 0.5*(er[ix] + er[im]) : er[ix];
            
            cexly[ix] = dt/(e0*er_ex*dy);
            ceylx[ix] = dt/(e0*er_ey*dx);
            chzlx[ix] = dt/(m0*dx);
            chzly[ix] = dt/(m0*dy);

            if (obj[ix]) {
                cexly[ix] = 0.0;
                ceylx[ix] = 0.0;
                chzlx[ix] = 0.0;
                chzly[ix] = 0.0;
            }
            if (j != 0 && obj[jm]) {
                cexly[ix] = 0.0;
            }
            if (i != 0 && obj[im]) {
                ceylx[ix] = 0.0;
            }

        }
    }
}

void init_pml_vars(const int length[], FLOAT *exy, FLOAT *eyx, FLOAT *hzx, FLOAT *hzy)
{
    const int n = length[0]*length[1];
    for (int i=0; i<n; i++) {
        exy[i] = 0.0;
        eyx[i] = 0.0;
        hzx[i] = 0.0;
        hzy[i] = 0.0;
    }
}



void set_pml_conf(const struct Range *whole, const struct Range *inside,
                  unsigned int axis, FLOAT dt, FLOAT ds, int offset,
                  FLOAT c, FLOAT e0, FLOAT em0, FLOAT *cs, FLOAT *csl)
{
    // offset = 0 or 1

    const int begin = inside->begin[axis];
    const int end   = inside->begin[axis] + inside->length[axis];

    const int   mgn       = inside->begin[axis] - whole->begin[axis];
    const FLOAT r0        = 1.0*10e-12;
    const FLOAT m         = 3.0;
    const FLOAT pmlec_max = - (m+1.0)*e0*c / (2.0*mgn*ds)*log(fabs(r0));
    
    for (int ii=0; ii<whole->length[axis]; ii++) {

        const int   i = ii + whole->begin[axis];
        const FLOAT x = i + offset * 0.5;
        
        const FLOAT pmlec = i < begin         ? pmlec_max * pow((begin - x)/mgn, m) :
                            i > end - offset  ? pmlec_max * pow((x - end  )/mgn, m) :
                                                0.0;

        const FLOAT a = pmlec * dt / (2.0*e0);
        cs [ii] = (1.0 - a)/(1.0 + a);
        csl[ii] = dt / (em0 * ds) / (1.0 + a);
    }

}

void set_pml_initial_condition(const struct Range *whole, const struct Range *inside,
                               FLOAT dt, FLOAT dx, FLOAT dy,
                               FLOAT c, FLOAT e0, FLOAT m0, 
                               FLOAT *cexy, FLOAT *ceyx, FLOAT *chzx, FLOAT *chzy,
                               FLOAT *cexyl, FLOAT *ceyxl, FLOAT *chzxl, FLOAT *chzyl)
{
    const unsigned int axis_x = 0;
    const unsigned int axis_y = 1;
    const int offset_e = 0;
    const int offset_h = 1;
    
    set_pml_conf(whole, inside, axis_y, dt, dy, offset_e, c, e0, e0, cexy, cexyl);
    set_pml_conf(whole, inside, axis_x, dt, dx, offset_e, c, e0, e0, ceyx, ceyxl);
    
    set_pml_conf(whole, inside, axis_x, dt, dx, offset_h, c, e0, m0, chzx, chzxl);
    set_pml_conf(whole, inside, axis_y, dt, dy, offset_h, c, e0, m0, chzy, chzyl);
}


void set_pml_rer(const int length[], const int *obj, const FLOAT *er, FLOAT *rer_ex, FLOAT *rer_ey)
{
    for (int j=0; j<length[1]; j++) {
        for (int i=0; i<length[0]; i++) {
            const int ix = i + j*length[0];
            const int jm = i + (j-1)*length[0];
            const int im = (i-1) + j*length[0];

            const FLOAT er_ex = j != 0 ? 0.5*(er[ix] + er[jm]) : er[ix];
            const FLOAT er_ey = i != 0 ? 0.5*(er[ix] + er[im]) : er[ix];

            const int obj_ex = j != 0 ? (obj[ix] || obj[jm]) : obj[ix];
            const int obj_ey = i != 0 ? (obj[ix] || obj[im]) : obj[ix];
            
            rer_ex[ix] = obj_ex == 0 ? 1.0/er_ex : 0.0;
            rer_ey[ix] = obj_ey == 0 ? 1.0/er_ey : 0.0;
            //rer_ey[ix] = 1.0/er_ey;
        }
    }    
}

