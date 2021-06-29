/**
 * @file output.h
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

#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdio.h>
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

bool write_bmp(int icnt, FLOAT time, const int length[], FLOAT dx, FLOAT dy,
               const FLOAT *ex, const FLOAT *ey, const FLOAT *hz);


#ifdef __cplusplus
}
#endif
    
#endif /* OUTPUT_H */


