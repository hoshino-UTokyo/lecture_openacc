/**
 * @file output.cc
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

#include "output.h"
#include <iostream>
#include <cmath>
#include "bitmap.h"

bool write_bmp(int icnt, FLOAT time, const int length[], FLOAT dx, FLOAT dy,
               const FLOAT *ex, const FLOAT *ey, const FLOAT *hz)
{
    const int lnx = length[0];
    const int lny = length[1];
    const int ln  = lnx * lny;

    char filename[64];
    sprintf(filename, "e%05d.bmp", icnt);

    const FLOAT max =  100.0;
    const FLOAT min = -100.0;
    const FLOAT dn  = 1.0/(max - min);

    FLOAT *p = new FLOAT[ln];

    for (int j=0; j<lny; j++) {
        for (int i=0; i<lnx; i++) {
            const int ix = j*lnx + i;

            const FLOAT f = (ex[ix] - min)*dn;
            //p[ix] = fmin(fmax(f + (FLOAT)(2.0/254.0), (FLOAT)0.0), (FLOAT)1.0-(FLOAT)(2.0/254));
            p[ix] = fmin(fmax(f, (FLOAT)(2.0/256.0)), (FLOAT)1.0-(FLOAT)(2.0/256.0));
        }
    }

    BitmapWriter writer(filename);
    BitmapPalette palette(BitmapPalette::SEISMIC);
    const int ret = writer.write_8bit(lnx, lny, p, palette);
    //RGBPalette palette(RGBPalette::DIFFERENCE);
    //const int ret = writer.write_rgb(lnx, lny, ex, min, max, palette);

    delete [] p; p = NULL;

    return (ret == lnx*lny) ? true : false;
}






