/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：定义与FAT文件系统本身有关的定义(数据在设备上的存放格式).
版本号：1.0.0
开发时期：2003-4-18
作者：魏海龙
修改记录：
******************************************************/
#ifndef	_FAT_FORMAT_H_
#define	_FAT_FORMAT_H_
#pragma pack(push,1)


#define		FAT_CLUSTER_BAD  0x0FFFFFF7  //You should not worry about use only one DWORD value in FAT16,FAT12,
										 //because it's high bits will be cut when writing it to the disk.
#define		FAT_CLUSTER_EOC	 0x0FFFFFFF
#define		FAT_CLUSTER_EOC16	 0xFFFF
#define		FAT_CLUSTER_EOC12    0x0FFF

#define		MAX_SHORT_NAME_PAHTH		80  //including NULL
#define		MAX_LONG_NAME_PATH			260 //including NULL
#define		MAX_LONG_NAME				255 //not include NULL

//#define		INVALID_CLUSTER(n)  ((n)==-1 || (n)==FAT_CLUSTER_EOC)

// special character allowed in short name ....			$ % ' - _ @ ~ ` ! ( ) { } ^ # &
// these six are allowed in long name but short name.   + , ; = [ ]

// Leading and trailing space should be ignored.

//Note: the fist data cluster is cluster 2. so, we should plus 2 before caculate the location.
//#define		FIRST_SECTOR_OF_CLUSTER(pVol,N)  ( (((N)-2) * pVol->fat_info.dwSecPerCluster) + pVol->dwFirstDataSec )
#define		FIRST_SECTOR_OF_CLUSTER(pVol,N)  ( (((N)-pVol->dwRootDirClusterNum -2) * pVol->fat_info.dwSecPerCluster) + pVol->dwFirstDataSec )

#define		START_SETOR_OF_CLUSTER(end,pVol,N)		\
	if( (N) >= (pVol->dwRootDirClusterNum +2)){	\
		end=( (((N)- pVol->dwRootDirClusterNum -2) * pVol->fat_info.dwSecPerCluster) + pVol->dwFirstDataSec );	\
	}else if( (N)<	pVol->dwRootDirClusterNum ){						\
		end= pVol->dwRootDirSec+(N)*pVol->fat_info.dwSecPerCluster;		\
	}else{																\
		end=-1;															\
	}																	

//#define		FIRST_SECTOR_OF_CLUSTER(pVol,N)  ( (((N)-pVol->dwRootDirClusterNum -2) * pVol->fat_info.dwSecPerCluster) + pVol->dwFirstDataSec )

//Note:为了在FAT16中处理方便，我将FAT16中 root directory区域也计入系统cluster，这样，在计算cluster的第一个sector以及
//		计算cluster在FAT表中的位置时要考虑这个问题（减去一个差值，FAT32是不会有的,is 0).



struct _BPB_STRUCT{

	BYTE  cBS_JumpRoot[3]; // where is the boot code located.
	char  cBS_OemName[8];	//the name of the FSD driver,
	WORD  wBPB_BytePerSec; //It takes on only the value: 512, 1024, 2048 or 4096.
	BYTE  nPBP_SecPerClus;//it must be the power of 2. so it can be 1,2,4,8,16,32,64,128.
								 //but note, the cluster size should not greater than 32K.
	WORD  wPBP_RsvdSecCnt;//reserved sector count.
	BYTE  nPBP_NumFATs;
	WORD  wPBP_RootEntryCount;
	WORD  wPBP_TotalSector16;
	BYTE  nPBP_MediaType;
	WORD  wPBP_FATSize16;// size of one FAT.
	WORD  wPBP_SectorsPerTrack;
	WORD  wPBP_NumHeads;
	DWORD  dwPBP_HiddSector;
	DWORD  dwPBP_TotalSector32;

}_PACKED_;

typedef	struct _BPB_STRUCT 	BPB_STRUCT, *PBPB_STRUCT;

struct _FAT16STRUCT{ //this structure starts from offset 36.

	BYTE nBS_DrvNum;
	BYTE cBS_Reserved1;
	BYTE nBS_BootSig;
	DWORD dwBS_VolId;
	char cBS_VolLab[11];
	char cBS_FilSysType[8];

}_PACKED_;

typedef	struct _FAT16STRUCT	FAT16STRUCT,*PFAT16STRUCT;


struct _FAT32STRUCT{ //this structure starts from offset 36.

	DWORD dwBPB_FATSize32;
	WORD wBPB_ExtFlags;
	WORD wBPB_FATVersion;
	DWORD dwBPB_RootCluster; //the number of first cluster in root directory.
	WORD wBPB_FSInfo;
	WORD wBPB_BackupBootSector;
	BYTE cBPB_Reserved[12];
	BYTE nBS_DrvNum;
	BYTE cBS_Reserved1;
	BYTE nBS_BootSig;
	DWORD dwBS_VolId;
	char cBS_VolLab[11];
	char cBS_FilSysType[8];

}_PACKED_;
typedef	struct _FAT32STRUCT	FAT32STRUCT,*PFAT32STRUCT;

struct FSINFO{

	DWORD dwLeadSig;
	char  cReserved1[480];
	DWORD	dwStructSig;
	DWORD   dwFreeCount;
	DWORD	dwNextFree;
	char	cReserved2[12];
	DWORD   dwTrailSig;

}_PACKED_;
typedef	struct FSINFO	FSINFO,*PFSINFO;

typedef struct DSKSZTOSECPERCLUS {
		DWORD dwDiskSize;
		BYTE  cSecPerClusVal;
}DSKSZTOSECPERCLUS;

//Now, define the directory entry.

#define	FILE_ATTR_READ_ONLY		0x01
#define	FILE_ATTR_HIDDEN			0x02
#define	FILE_ATTR_SYSTEM			0x04
#define	FILE_ATTR_VOLUME_ID		0x08
#define	FILE_ATTR_DIRECTORY		0x10
#define	FILE_ATTR_ARCHIVE		0x20
#define	FILE_ATTR_LONG_NAME		(FILE_ATTR_READ_ONLY |FILE_ATTR_HIDDEN | FILE_ATTR_SYSTEM | FILE_ATTR_VOLUME_ID)

#define	DIR_ENTRY_FREE			0xE5
#define	DIR_ENTRY_EOF			0x00
#define	DIR_ENTRY_E5			0x05


// Invalid character for dir name.
//0x22, 0x2A, 0x2B, 0x2C, 0x2E, 0x2F, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x5B, 0x5C, 0x5D,and 0x7C.
struct	FAT_DATE{
	
	unsigned short  nDay:  5 ;
	unsigned short  nMonth:4;
	unsigned short  nYearCount:7; //Year count from 1980.

}_PACKED_;

typedef struct	FAT_DATE	FAT_DATE, *PFAT_DATE;


struct	FAT_TIME{

	unsigned short  nTwiceSecond :5; //two seconds count.
	unsigned short  nMinute		:6;
	unsigned short  nHour		:5;

}_PACKED_;
typedef struct	FAT_TIME	FAT_TIME,*PFAT_TIME;


struct	_FAT_FOLDER_ENTRY{
	
	UCHAR	cShortName[11];
	UCHAR	cAttr;
	UCHAR	cRsved;
	UCHAR	cTimeTenth;// tenth of a second. valid value is  0-199.
	FAT_TIME	mTimeCreated;
	FAT_DATE	mDateCreated;
	FAT_DATE	mLastAccessDate;
	WORD	  wFirstClusterHigh;
	FAT_TIME	mTimeLastWrite;
	FAT_DATE	mDateLastWrite;
	WORD	wFirstClusterLow;
	DWORD	dwFileSize; //for a dirctory, this value is always 0.

}_PACKED_;

typedef struct	_FAT_FOLDER_ENTRY	DIR_ENTRY, *PDIR_ENTRY;


#define	LAST_LONG_ENTRY	0x40
#define	MAX_LONG_NAME_ENTRY	0x20

#define	NAME_SIZE_ONE_ENTRY		26
#define	NAME_SIZE_WORD	13

struct _LONG_NAME_ENTRY{

	UCHAR	cNameOrder;
	UCHAR	cLongName1[10];
	UCHAR	cAttr;
	UCHAR	cType;
	UCHAR	cChecksum;
	UCHAR	cLongName2[12];
	WORD	wFirstClusterLow;
	UCHAR	cLongName3[4];

}_PACKED_;

typedef	struct _LONG_NAME_ENTRY	LONG_NAME_ENTRY,*PLONG_NAME_ENTRY;


void  ParseFileTime( PFILETIME pFileTime,  PFAT_DATE pDate, PFAT_TIME pTime);



#pragma pack(pop)
#endif
