/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __TOUCHP_H
#define __TOUCHP_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

#define TWO_DIMENSIONAL_CALIBRATION 1

#define ADC_PRECISION       12
#define MAX_TERM_PRECISION  27

#define MAX_COEFF_PRECISION (MAX_TERM_PRECISION - ADC_PRECISION)

BOOL
TouchPanelSetCalibration(
    int   cCalibrationPoints,
    int   *pScreenXBuffer,
    int   *pScreenYBuffer,
    int   *pUncalXBuffer,
    int   *pUncalYBuffer
    );

void
TouchPanelCalibrateAPoint(
    int   UncalX,
    int   UncalY,
    int   *pCalX,
    int   *pCalY 
    );

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  //#ifndef   _TOUCHP_H_




