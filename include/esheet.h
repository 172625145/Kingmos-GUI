#ifndef __ESHEET_H
#define __ESHEET_H

#include "eframe.h"
#include "eassert.h"
#include "eptrlist.h"


//sheet style

#define SHS_READONLY            0x00000001



//sheet message

#define SHM_GETDATASTREAM       0X5000
#define SHM_SETDATASTREAM		0X5001

#define SHM_GETBLOCKDATA		0X5002
#define SHM_SETBLOCKDATA		0X5004

#define SHM_INSERTBLOCKDATA		0X5003
#define SHM_DELETEINVERTDATA	0X500C

#define SHM_DELETEINVERTBLOCK	0X5005
#define SHM_INSERTINVERTBLOCK	0X5006

#define SHM_GETINVERT			0X5007
#define SHM_SETINVERT			0X5008

#define SHM_GETFOCUS			0X5009
#define SHM_SETFOCUS			0X500A

#define SHM_GETDATASTREAMLENGTH 0x500B

#define SHM_SETMODIFIED         0x500D
#define SHM_GETMODIFIED         0x500E


#define SHM_SETREADONLY         0x500F

typedef void*  HCELLOBJ;


typedef struct 
{
   SHORT iCellType;
   SHORT nRow;
   SHORT nColumn;
   HCELLOBJ hObj;
}CELLDATASTRUCT, *LPCELLDATASTRUCT;


extern int PDA_SetScrollPos(HWND hWnd, int nBar,int nPos,BOOL bRedraw);
extern int PDA_GetScrollPos(HWND hWnd, int nBar);
extern BOOL PDA_SetScrollRange(HWND hWnd,int nBar,int nMinPos,int nMaxPos,BOOL bRedraw );

#endif //__ESHEET_H
