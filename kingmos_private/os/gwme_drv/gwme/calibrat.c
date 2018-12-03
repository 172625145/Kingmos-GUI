/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：校正
版本号：1.0.0
开发时期：1999
作者：ln
修改记录：
******************************************************/


#include <ewindows.h>

#include "largenum.h"
#include "touchcal.h"

#define MAX_POINT_ERROR 5

typedef struct _MATRIX33{
    PLARGENUM   pa11, pa12, pa13;
    PLARGENUM   pa21, pa22, pa23;
    PLARGENUM   pa31, pa32, pa33;
}MATRIX33, *PMATRIX33;

typedef struct _CALIBRATION_PARAMETER{
    int   a1;
    int   b1;
    int   c1;
    int   a2;
    int   b2;
    int   c2;
    int   delta;
}CALIBRATION_PARAMETER, *PCALIBRATION_PARAMETER;

static BOOL bCalibrated = FALSE;
static CALIBRATION_PARAMETER    v_CalcParam = { -21128, 98, 82379318, -614, -26855, 110951931, 132300 };
static void SaveToReg( VOID );

static BOOL
ErrorAnalysis(
    int   cCalibrationPoints,     //
    int   *pScreenXBuffer,        //
    int   *pScreenYBuffer,        //
    int   *pUncalXBuffer,         //
    int   *pUncalYBuffer          //
    );

static VOID
ComputeMatrix33(
    PLARGENUM   pResult,
    PMATRIX33   pMatrix
    );

static VOID
ComputeMatrix33(
    PLARGENUM   pResult,
    PMATRIX33   pMatrix
    )
{
    LARGENUM    lnTemp;

    LargeNumMult(pMatrix->pa11, pMatrix->pa22, &lnTemp);
    LargeNumMult(pMatrix->pa33, &lnTemp, pResult);
    LargeNumMult(pMatrix->pa21, pMatrix->pa32, &lnTemp);
    LargeNumMult(pMatrix->pa13, &lnTemp, &lnTemp);
    LargeNumAdd(pResult, &lnTemp, pResult);
    LargeNumMult(pMatrix->pa12, pMatrix->pa23, &lnTemp);
    LargeNumMult(pMatrix->pa31, &lnTemp, &lnTemp);
    LargeNumAdd(pResult, &lnTemp, pResult);
    LargeNumMult(pMatrix->pa13, pMatrix->pa22, &lnTemp);
    LargeNumMult(pMatrix->pa31, &lnTemp, &lnTemp);
    LargeNumSub(pResult, &lnTemp, pResult);
    LargeNumMult(pMatrix->pa12, pMatrix->pa21, &lnTemp);
    LargeNumMult(pMatrix->pa33, &lnTemp, &lnTemp);
    LargeNumSub(pResult, &lnTemp, pResult);
    LargeNumMult(pMatrix->pa23, pMatrix->pa32, &lnTemp);
    LargeNumMult(pMatrix->pa11, &lnTemp, &lnTemp);
    LargeNumSub(pResult, &lnTemp, pResult);
}

static BOOL
ErrorAnalysis(
    int   cCalibrationPoints,     //
    int   *pScreenXBuffer,        //
    int   *pScreenYBuffer,        //
    int   *pUncalXBuffer,         //
    int   *pUncalYBuffer          //
    )
{
    int     i;
    ULONG maxErr, err;
    int   x,y;
    int   dx,dy;
    UINT  errThreshold = MAX_POINT_ERROR;  // Can be overridden by registry entry
//    UINT  status, ValType, ValLen;
 
    maxErr = 0;
    for(i=0; i<cCalibrationPoints; i++){
        TouchPanelCalibrateAPoint(  
			pUncalXBuffer[i],
			pUncalYBuffer[i],
			&x,
			&y
			);
        x /= 4;
        y /= 4;
		
		dx = x - pScreenXBuffer[i];
        dy = y - pScreenYBuffer[i];
        err = dx * dx + dy * dy;
        if(err > maxErr){
            maxErr = err;
        }
    }
    EdbgOutputDebugString("Maximum error (square of Euclidean distance in screen units) = %u\r\n", maxErr);

    if( maxErr < (errThreshold * errThreshold) ){
       return TRUE;
    } else {
      EdbgOutputDebugString("Maximum error %u exceeds calibration threshold %u\r\n", maxErr, errThreshold);
       return FALSE;
    }
}

//call by calibration ap
BOOL
TouchPanelSetCalibration(
    int   cCalibrationPoints,     //
    int   *pScreenXBuffer,        //
    int   *pScreenYBuffer,        //
    int   *pUncalXBuffer,         //
    int   *pUncalYBuffer          //
    )
{
    LARGENUM    a11;
    LARGENUM    a21, a22;
    LARGENUM    a31, a32, a33;
    LARGENUM    b11, b12, b13;
    LARGENUM    b21, b22, b23;
    LARGENUM    lnScreenX;
    LARGENUM    lnScreenY;
    LARGENUM    lnTouchX;
    LARGENUM    lnTouchY;
    LARGENUM    lnTemp;
    LARGENUM    delta;
    LARGENUM    a1, b1, c1;
    LARGENUM    a2, b2, c2;
    MATRIX33    Matrix;
    int       cShift;
    int       minShift;
    int         i;


    if ( cCalibrationPoints == 0 )
    {
		bCalibrated = FALSE;
        return TRUE;
    }

    LargeNumSet(&a11, 0);
    LargeNumSet(&a21, 0);
    LargeNumSet(&a31, 0);
    LargeNumSet(&a22, 0);
    LargeNumSet(&a32, 0);
    LargeNumSet(&a33, cCalibrationPoints);
    LargeNumSet(&b11, 0);
    LargeNumSet(&b12, 0);
    LargeNumSet(&b13, 0);
    LargeNumSet(&b21, 0);
    LargeNumSet(&b22, 0);
    LargeNumSet(&b23, 0);
    for(i=0; i<cCalibrationPoints; i++){
        LargeNumSet(&lnTouchX, pUncalXBuffer[i]);
        LargeNumSet(&lnTouchY, pUncalYBuffer[i]);
        LargeNumSet(&lnScreenX, pScreenXBuffer[i]);
        LargeNumSet(&lnScreenY, pScreenYBuffer[i]);
        LargeNumMult(&lnTouchX, &lnTouchX, &lnTemp);
        LargeNumAdd(&a11, &lnTemp, &a11);
        LargeNumMult(&lnTouchX, &lnTouchY, &lnTemp);
        LargeNumAdd(&a21, &lnTemp, &a21);
        LargeNumAdd(&a31, &lnTouchX, &a31);
        LargeNumMult(&lnTouchY, &lnTouchY, &lnTemp);
        LargeNumAdd(&a22, &lnTemp, &a22);
        LargeNumAdd(&a32, &lnTouchY, &a32);
        LargeNumMult(&lnTouchX, &lnScreenX, &lnTemp);
        LargeNumAdd(&b11, &lnTemp, &b11);
        LargeNumMult(&lnTouchY, &lnScreenX, &lnTemp);
        LargeNumAdd(&b12, &lnTemp, &b12);
        LargeNumAdd(&b13, &lnScreenX, &b13);
        LargeNumMult(&lnTouchX, &lnScreenY, &lnTemp);
        LargeNumAdd(&b21, &lnTemp, &b21);
        LargeNumMult(&lnTouchY, &lnScreenY, &lnTemp);
        LargeNumAdd(&b22, &lnTemp, &b22);
        LargeNumAdd(&b23, &lnScreenY, &b23);
    }

    Matrix.pa11 = &a11;
    Matrix.pa21 = &a21;
    Matrix.pa31 = &a31;
    Matrix.pa12 = &a21;
    Matrix.pa22 = &a22;
    Matrix.pa32 = &a32;
    Matrix.pa13 = &a31;
    Matrix.pa23 = &a32;
    Matrix.pa33 = &a33;
    ComputeMatrix33(&delta, &Matrix);

    Matrix.pa11 = &b11;
    Matrix.pa21 = &b12;
    Matrix.pa31 = &b13;
    ComputeMatrix33(&a1, &Matrix);

    Matrix.pa11 = &a11;
    Matrix.pa21 = &a21;
    Matrix.pa31 = &a31;
    Matrix.pa12 = &b11;
    Matrix.pa22 = &b12;
    Matrix.pa32 = &b13;
    ComputeMatrix33(&b1, &Matrix);

    Matrix.pa12 = &a21;
    Matrix.pa22 = &a22;
    Matrix.pa32 = &a32;
    Matrix.pa13 = &b11;
    Matrix.pa23 = &b12;
    Matrix.pa33 = &b13;
    ComputeMatrix33(&c1, &Matrix);

    Matrix.pa13 = &a31;
    Matrix.pa23 = &a32;
    Matrix.pa33 = &a33;
    Matrix.pa11 = &b21;
    Matrix.pa21 = &b22;
    Matrix.pa31 = &b23;
    ComputeMatrix33(&a2, &Matrix);

    Matrix.pa11 = &a11;
    Matrix.pa21 = &a21;
    Matrix.pa31 = &a31;
    Matrix.pa12 = &b21;
    Matrix.pa22 = &b22;
    Matrix.pa32 = &b23;
    ComputeMatrix33(&b2, &Matrix);

    Matrix.pa12 = &a21;
    Matrix.pa22 = &a22;
    Matrix.pa32 = &a32;
    Matrix.pa13 = &b21;
    Matrix.pa23 = &b22;
    Matrix.pa33 = &b23;
    ComputeMatrix33(&c2, &Matrix);

    {
        LARGENUM    halfDelta;

        if(IsLargeNumNegative(&delta)){
            LargeNumDivint32(&delta, -2, &halfDelta);
        } else {
            LargeNumDivint32(&delta, 2, &halfDelta);  //
        }
        LargeNumAdd(&c1, &halfDelta, &c1);
        LargeNumAdd(&c2, &halfDelta, &c2);
    }

    //
	minShift = 0;
    cShift = LargeNumBits(&a1) - MAX_COEFF_PRECISION;
    if(cShift > minShift){
        minShift = cShift;
    }
    cShift = LargeNumBits(&b1) - MAX_COEFF_PRECISION;
    if(cShift > minShift){
        minShift = cShift;
    }
    cShift = LargeNumBits(&a2) - MAX_COEFF_PRECISION;
    if(cShift > minShift){
        minShift = cShift;
    }
    cShift = LargeNumBits(&b2) - MAX_COEFF_PRECISION;
    if(cShift > minShift){
        minShift = cShift;
    }
    cShift = LargeNumBits(&c1) - MAX_TERM_PRECISION;
    if(cShift > minShift){
        minShift = cShift;
    }
    cShift = LargeNumBits(&c2) - MAX_TERM_PRECISION;
    if(cShift > minShift){
        minShift = cShift;
    }
    cShift = LargeNumBits(&delta) - 31;
    if(cShift > minShift){
        minShift = cShift;
    }

    //

    if(minShift){
        LargeNumRAShift(&a1, minShift);
        LargeNumRAShift(&a2, minShift);
        LargeNumRAShift(&b1, minShift);
        LargeNumRAShift(&b2, minShift);
        LargeNumRAShift(&c1, minShift);
        LargeNumRAShift(&c2, minShift);
        LargeNumRAShift(&delta, minShift);
    }

    // 校正参数
    v_CalcParam.a1      = a1.u.s32.u[0];
    v_CalcParam.b1      = b1.u.s32.u[0];
    v_CalcParam.c1      = c1.u.s32.u[0];
    v_CalcParam.a2      = a2.u.s32.u[0];
    v_CalcParam.b2      = b2.u.s32.u[0];
    v_CalcParam.c2      = c2.u.s32.u[0];
    v_CalcParam.delta   = delta.u.s32.u[0];

     // 判断delta 是否为零，delta将会成为除数
    if( ! v_CalcParam.delta )
    {
        v_CalcParam.delta = 1;
        bCalibrated = FALSE;
    }
    else
	{
		RETAILMSG( 1, ( "TouchPanelSetCalibration: ok,v_CalcParam(a1=%d,b1=%d,c1=%d,a2=%d,b2=%d,c2=%d,delta=%d).\r\n", v_CalcParam.a1,v_CalcParam.b1,v_CalcParam.c1,v_CalcParam.a2,v_CalcParam.b2,v_CalcParam.c2,v_CalcParam.delta ) );
        bCalibrated = TRUE;
		SaveToReg();
	}

    return ErrorAnalysis(
                    cCalibrationPoints,
                    pScreenXBuffer,
                    pScreenYBuffer,
                    pUncalXBuffer,
                    pUncalYBuffer
                );
}
// call by toucher
void
TouchPanelCalibrateAPoint(
    int   xUnCalibrate,     //原始的x坐标
    int   yUnCalibrate,     //原始的y坐标
    int   *pCalX,     //转换后的x坐标
    int   *pCalY      //转换后的y坐标
    )
{
    int   x, y;

    if ( !bCalibrated ){
        *pCalX = xUnCalibrate;
        *pCalY = yUnCalibrate;
        return;
    }

    //
    // *4 提供附加精度
    //
	x = (v_CalcParam.a1 * xUnCalibrate + v_CalcParam.b1 * yUnCalibrate +
         v_CalcParam.c1) * 4 / v_CalcParam.delta;
    y = (v_CalcParam.a2 * xUnCalibrate + v_CalcParam.b2 * yUnCalibrate +
         v_CalcParam.c2) * 4 / v_CalcParam.delta;
    if ( x < 0 ){
        x = 0;
    }

    if ( y < 0 ){
        y = 0;
    }

    *pCalX = x ;
    *pCalY = y;
}

    
void _InitCalibration( VOID )
{
	HKEY hSubKey;
	DWORD dwType;
	DWORD dwSize;
	char  buf[128+1];

    if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_HARDWARE_ROOT, "DeviceMap\\Touch", 0, KEY_ALL_ACCESS, &hSubKey ) )
	{
		dwSize = sizeof(buf)-1;
		if( ERROR_SUCCESS == RegQueryValueEx( hSubKey, "CalibrationData", NULL, &dwType, buf, &dwSize ) )
		{
			CALIBRATION_PARAMETER cp;
			if( dwSize )
			{
				int n = sizeof(CALIBRATION_PARAMETER) / sizeof(int);
				int i;
				int *lpcp = (int*)&cp;
				char * lp, *lps;
				buf[dwSize] = 0;
				lp = buf;
				for( i = 0; i < n; i++ )
				{
					lps = strchr( lp, ',' );
					if( lps )
					{
						*lps = 0;
						*lpcp++ = atoi( lp );
						lp = lps + 1;
					}
					else
					{	//
						if( i == 6 && *lp )
						{  //最后一个
							*lpcp++ = atoi( lp );
							i++;
						}
						break;
					}
				}
				if( i == n )
				{
					v_CalcParam = cp;
					bCalibrated = TRUE;
					RETAILMSG( 1, ( "_InitCalibration: ok,v_CalcParam(a1=%d,b1=%d,c1=%d,a2=%d,b2=%d,c2=%d,delta=%d).\r\n", v_CalcParam.a1,v_CalcParam.b1,v_CalcParam.c1,v_CalcParam.a2,v_CalcParam.b2,v_CalcParam.c2,v_CalcParam.delta ) );					
				}
			}
		}
		RegCloseKey( hSubKey );
	}
}

void _DeinitCalibration( VOID )
{
}

static VOID SaveToReg( VOID )
{
	HKEY hSubKey;
	DWORD dwType;
	char  buf[128+1];

    //if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_HARDWARE_ROOT, "DeviceMap\\Touch", 0, 0, &hSubKey ) )
    if( ERROR_SUCCESS == RegCreateKeyEx( HKEY_HARDWARE_ROOT, 
										 "DeviceMap\\Touch", 
										 0,
										 0,
										 0,
										 KEY_ALL_ACCESS,
										 NULL,
										 &hSubKey,
										 NULL ) )
	{
		buf[0];
		dwType = REG_SZ;
		sprintf( buf, "%d,%d,%d,%d,%d,%d,%d", v_CalcParam.a1, v_CalcParam.b1, v_CalcParam.c1, v_CalcParam.a2, v_CalcParam.b2, v_CalcParam.c2, v_CalcParam.delta );
		if( ERROR_SUCCESS == RegSetValueEx( hSubKey, "CalibrationData", 0, dwType, buf, strlen(buf) + 1 ) )
		{
		}
		RegCloseKey( hSubKey );
	}
}
