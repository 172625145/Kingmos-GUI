#ifndef __TOUCH_H

#define __TOUCH_H

//采样状态
#define	SAMPLE_VALID            0x01

#define SAMPLE_UP                  0
#define SAMPLE_DOWN			    0x02
#define SAMPLE_PREVIOUS_DOWN    0x04
#define SAMPLE_IGNORE           0x08

//较正状态
#define	CALIBRATION_INACIVE		0x00
#define	CALIBRATION_WAITING		0x01
#define CALIBRATION_DOWN		0x02
#define	CALIBRATION_VALID		0x04
#define	CALIBRATION_ABORTED		0x08

extern const UINT idTouchIrq;
extern const UINT idTouchChangedIrq;
extern const UINT uMinCalCount;

// 设备相关界面
BOOL TouchPddEnable( void );
VOID TouchPddDisable( void );
void TouchPddDestroy( void );
BOOL TouchPddCreate( void );
void TouchPddPowerHandle( BOOL bPowerOff );
VOID TouchPddGetPoint( int *pUnCalx, int *pUnCaly, DWORD *pdwFlag );


#endif // __TOUCH_H
