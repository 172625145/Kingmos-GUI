#ifndef __SMARTPEN_H
#define __SMARTPEN_H

typedef struct _POSDATA
{
	unsigned char x;
	unsigned char y;
}POSDATA, * LPPOSDATA;

typedef const POSDATA * LPCPOSDATA;

#define PENAPI

#define HWRF_ENGLISH		0x00000001
#define HWRF_NUM			0x00000002
#define HWRF_ASCII_MASK		0x000000ff
#define HWR_CHINESE			0x00000800

#define HWRF_NULL_END       0x00010000

#define MAX_POSDATA_COUNT 1024

int PENAPI HWR_Recognize(
						LPCPOSDATA lpPos, 
						unsigned int nPosCount, // <= MAX_POSDATA_COUNT 
						WORD * lpwCode,
						unsigned int nGetCodeNum,
						unsigned int flag
						);
int PENAPI HWR_Init( LPCTSTR  );
void PENAPI HWR_Deinit( void );
void PENAPI HWR_SetUserInput( WORD wCode );


#endif   //__SMARTPEN_H
