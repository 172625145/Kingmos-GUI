/******************************************************
Copyright(c) 版权所有，1998-2004微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：bot.h bulk only transport protocol
版本号：1.0.0
开发时期：2001-07-02
作者：tanphy
修改记录：
******************************************************/
#ifndef __BOT_H
#define __BOT_H

typedef struct _CBW{
	DWORD dCBWSignature;
	DWORD dCBWTag;
	DWORD dCBWDataTransferLength;
	BYTE bmCBWFlags;
	BYTE bCBWLUN:4;
	BYTE reserve:4;
	BYTE bCBWCBLength:5;
	BYTE reserve1:3;
	BYTE bCBWCB[16];
}_PACKED_ CBW, *PCBW;

typedef struct _CSW{
	DWORD dCSWSignature;
	DWORD dCSWTag;
	DWORD dCSWDataResidue;
	BYTE bCSWStatus;
}_PACKED_ CSW, *PCSW;

#define BLOCK_SIZE 512l
#define WAIT_CBW 1
#define SEND_BYTE_STATE 2
#define RECIVE_BYTE_STATE 3
#define SEND_CSW 4
#define SEND_BLOCK_STATE 5
#define RECIVE_BLOCK_STATE 6
#define UNKOWNSTATE 7

#define CBWSIGNATURE (0x43425355)
#define CSWSIGNATURE (0x53425355)

void TransferDataStart(BYTE *pTransferBuffer, DWORD dwTransferLen, BOOL bDirection);
void WaitForCBWStart();

void TransferData(BYTE *pTransferBuffer, DWORD dwTransferLen, DWORD dwDifference, BOOL bDirection);
void TransferDataDone(PCBW pCBW, DWORD dwResult);
void TransferBlock(DWORD dwBlockStart, DWORD dwBytes, DWORD dwDifference, BOOL bDirection);
BOOL CaptureRamDsk();

#endif
