/****************************************************************************
 *
 *		Copyright (C) 2002-2004	YAMAHA CORPORATION. All rights reserved.
 *
 *		Module		: masound.h
 *
 *		Description	: MA Sound Player API
 *
 *		Version		: 3.0.8 	2004.01.30
 *
 ****************************************************************************/

#ifndef __MASOUND_H__
#define __MASOUND_H__

#define MASMW_CNVID_MMF				(1)		/* SMAF/MA-1/MA-2/MA-3/MA-5 */
#define MASMW_CNVID_PHR				(2)		/* SMAF/Phrase L1/L2 */
#define	MASMW_CNVID_RMD				(3)		/* Realtime MIDI */
#define	MASMW_CNVID_AUD				(4)		/* SMAF/Audio */
#define	MASMW_CNVID_MID				(5)		/* SMF */
#define	MASMW_CNVID_HVS				(9)		/* HV Script */
#define	MASMW_CNVID_WAV				(11)	/* WAVE */

#define MASMW_SUCCESS				(0)		/* success 								*/
#define MASMW_ERROR					(-1)	/* error								*/
#define MASMW_ERROR_ARGUMENT		(-2)	/* error of arguments					*/
#define MASMW_ERROR_RESOURCE_OVER	(-3)	/* over specified resources				*/
#define MASMW_ERROR_ID				(-4)	/* error id number 						*/
#define MASMW_ERROR_TIMEOUT			(-5)	/* timeout		 						*/
#define MASMW_ERROR_SOFTRESET		(-6)	/* error of soft reset for MA-5			*/

#define MASMW_ERROR_FILE			(-16)	/* file error							*/
#define MASMW_ERROR_CONTENTS_CLASS	(-17)	/* SMAF Contents Class shows can't play */
#define MASMW_ERROR_CONTENTS_TYPE	(-18)	/* SMAF Contents Type shows can't play	*/
#define MASMW_ERROR_CHUNK_SIZE		(-19)	/* illegal SMAF Chunk Size value		*/
#define MASMW_ERROR_CHUNK			(-20)	/* illegal SMAF Track Chunk value		*/
#define MASMW_ERROR_UNMATCHED_TAG	(-21)	/* unmathced specified TAG 				*/
#define MASMW_ERROR_SHORT_LENGTH	(-22)	/* short sequence 						*/
#define MASMW_ERROR_LONG_LENGTH		(-23)	/* long sequence 						*/
#define	MASMW_ERROR_UNSUPPORTED		(-24)	/* unsupported format					*/
#define MASMW_ERROR_NO_INFORMATION	(-25)	/* no specified information				*/
#define MASMW_ERROR_HV_CONFLICT		(-26)	/* conflict about HV resource			*/

#define MASMW_ERROR_SMF_FORMAT		(-50)	/* invalid format type != 0/1			*/
#define MASMW_ERROR_SMF_TRACKNUM	(-51)	/* invalid number of tracks				*/
#define MASMW_ERROR_SMF_TIMEUNIT	(-52)	/* invalid time unit					*/
#define MASMW_ERROR_SMF_CMD			(-53)	/* invalid command byte					*/


#define MASMW_STATE_IDLE			(0)		/* idle state */
#define MASMW_STATE_LOADED			(1)		/* loaded state */
#define MASMW_STATE_OPENED			(2)		/* opened state */
#define MASMW_STATE_READY			(3)		/* ready state */
#define MASMW_STATE_PLAYING			(4)		/* playing state */
#define	MASMW_STATE_PAUSE			(5)		/* pause state */

#define MASMW_SET_VOLUME			(0)		/* set volume */
#define MASMW_SET_SPEED				(1)		/* set speed */
#define MASMW_SET_KEYCONTROL		(2)		/* set key control */
#define MASMW_GET_POSITION			(4)		/* get position */
#define MASMW_GET_LENGTH			(5)		/* get length */
#define MASMW_GET_STATE				(6)		/* get state */
#define MASMW_SEND_MIDIMSG			(7)		/* send midi message */
#define MASMW_SEND_SYSEXMIDIMSG		(8)		/* send sys.ex. midi message */
#define MASMW_GET_CONTENTSDATA		(10)	/* get contents data */
#define MASMW_GET_PHRASELIST		(11)	/* get phrase list */
#define MASMW_SET_STARTPOINT		(12)	/* set start point */
#define MASMW_SET_ENDPOINT			(13)	/* set end point */
#define MASMW_SET_PANPOT			(14)	/* set Panpot */
#define MASMW_GET_LEDSTATUS			(15)	/* get LED status */
#define MASMW_GET_VIBSTATUS			(16)	/* get VIB status */
#define MASMW_SET_EVENTNOTE			(17)	/* set event note */
#define MASMW_GET_LOADINFO			(19)	/* get load information */
#define MASMW_SET_LOADINFO			(20)	/* set load information */
#define MASMW_SET_HVVOICE			(22)	/* set voice parameter for HV */
#define MASMW_SET_HVPROGRAMNO		(23)	/* set program no. for HV */
#define MASMW_GET_HVCONVERTSIZE		(25)	/* get position of HV-Script */
#define MASMW_SET_SPEEDWIDE			(26)	/* set speed (wide range) */
#define MASMW_SET_REPEAT			(27)	/* set number of repeat */
#define MASMW_GET_CONTROL_VAL		(29)	/* Get control value  */
#define MASMW_SET_CB_INTERVAL		(30)	/* Set calback function interval */
#define MASMW_SET_HVVOLUME			(31)	/* set volume for HV-Script */
#define MASMW_GET_HVCHANNEL			(32)	/* get channel for HV-Script */
#define MASMW_SET_AUDIO_FORMAT		(40)	/* set format for streaming audio */
#define MASMW_GET_READPOINT			(41)	/* get read point for streaming audio */
#define MASMW_GET_WRITEPOINT		(42)	/* get writable point for streaming audio */
#define MASMW_SET_NOTIFYPOSITION	(43)	/* set notificate position for streaming audio */

/* Device Control */
#define MASMW_PWM_DIGITAL			(0)		/* power management (digital) */
#define MASMW_PWM_ANALOG			(1)		/* power management (analog) */
#define MASMW_EQ_VOLUME				(2)		/* eq volume */
#define MASMW_HP_VOLUME				(3)		/* hp volume */
#define MASMW_SP_VOLUME				(4)		/* sp volume */
#define MASMW_LED_MASTER			(5)		/* LED master select */
#define MASMW_LED_BLINK				(6)		/* LED blink setting */
#define MASMW_LED_DIRECT			(7)		/* LED direct control */
#define MASMW_MTR_MASTER			(8)		/* MOTOR master select */
#define MASMW_MTR_BLINK				(9)		/* MOTOR blink setting */
#define MASMW_MTR_DIRECT			(10)	/* MOTOR direct control */
#define MASMW_GET_PLLOUT			(11)	/* get PLL out */
#define MASMW_HW_INITIALIZE			(13)	/* initialize HW */
#define MASMW_WRITE_IMREG			(17)	/* write into intermediate registers */
#define MASMW_READ_IMREG			(18)	/* read intermediate registers */
#define MASMW_GET_DEVICETYPE		(20)	/* get device type */

/* Device Control Ex */
#define MASMW_SET_DIGITALAUDIO			(0)
#define MASMW_SET_DA_VOLUME				(1)
#define MASMW_SET_EFFECT_LINK			(2)
#define MASMW_SET_SURROUND_PARAM		(3)
#define MASMW_SET_SURROUND_CTRL			(4)
#define MASMW_SET_EQUALIZER				(5)
#define MASMW_SET_POWERDOWN				(6)
#define MASMW_SET_RXIN_VOLUME			(7)
#define MASMW_SET_EXTIN_VOLUME			(8)
#define MASMW_SET_HPOUT					(9)
#define MASMW_SET_SPOUT					(10)
#define MASMW_SET_TXOUT					(11)
#define MASMW_SET_EXTOUT				(12)
#define MASMW_SET_HPL_VOLUME			(13)
#define MASMW_SET_HPR_VOLUME			(14)
#define MASMW_SET_BASSBOOST				(15)
#define MASMW_SET_EQ_VOLUME				(16)
#define MASMW_SET_SP_VOLUME				(17)
#define MASMW_SET_TXOUT_VOLUME			(18)
#define MASMW_SET_EXTOUT_VOLUME			(19)
#define MASMW_SET_LED_SEQUENCE			(20)

/* device type */
#define MASMW_DEVICETYPE_MA5		(1)
#define MASMW_DEVICETYPE_MA5S		(2)
#define MASMW_DEVICETYPE_MA5I		(3)
#define MASMW_DEVICETYPE_MA5SI		(4)

/* callback ID */
#define MASMW_NOTIFYPOSITION		(125)	/* notify position for streaming audio */
#define MASMW_REPEAT				(126)	/* repeat */
#define MASMW_END_OF_SEQUENCE		(127)	/* end of sequence */
#define MASMW_RMD_INTERVAL_CB		(17)	/* interval timer */

struct _MASMW_MIDIMSG
{
	unsigned char *	msg;					/* pointer to MIDI message */
	unsigned long	size;					/* size of MIDI message */
};

struct _MASMW_CONTENTSINFO
{
	unsigned short	code;					/* code type */
	unsigned char	tag[2];					/* tag name */
	unsigned char *	buf;					/* pointer to read buffer */
	unsigned long	size;					/* size of read buffer */
};

struct _MASMW_PHRASELIST
{
	unsigned char	tag[2];					/* tag name */
	unsigned long	start;					/* start point */
	unsigned long	stop;					/* stop point */
};

struct _MASMW_EVENTNOTE
{
	unsigned char 	ch;						/* channel number */
	unsigned char	note;					/* note number */
};


struct _MASMW_HVVOICEPARAM
{
	unsigned char 	prgno;					/* Program No */
	unsigned char *	buf;					/* pointer to dictionary buffer */
	unsigned long	size;					/* size of dictionary buffer */
};

struct _MASMW_GETCTL
{
	unsigned char	bControl;				/* contorl number */
	unsigned char 	bCh;					/* channel number */
};

struct _MASMW_HVSCRIPT
{
	unsigned char *	buf;					/* pointer to HV-Script buffer */
	unsigned long	size;					/* size of HV-Script buffer */
};

struct _MASMW_AUDIO_FORMAT
{
	unsigned long dFormat;					/* format type */
	unsigned long dChFlag;					/* mono or stereo */
	unsigned long dFs;						/* sampling frequency */
};

struct _MASMW_AUDIO_NOTIFYPOSITION
{
	unsigned long dNum;
	unsigned long dPoint0;
	unsigned long dPoint1;
	unsigned long dPoint2;
	unsigned long dPoint3;
};


typedef struct _MASMW_MIDIMSG				MASMW_MIDIMSG, *PMASMW_MIDIMSG;
typedef struct _MASMW_CONTENTSINFO			MASMW_CONTENTSINFO, *PMASMW_CONTENTSINFO;
typedef struct _MASMW_PHRASELIST			MASMW_PHRASELIST, *PMASMW_PHRASELIST;
typedef struct _MASMW_EVENTNOTE				MASMW_EVENTNOTE, *PMASMW_EVENTNOTE;
typedef struct _MASMW_HVVOICEPARAM			MASMW_HVVOICEPARAM, *PMASMW_HVVOICEPARAM;
typedef struct _MASMW_GETCTL				MASMW_GETCTL, *PMASMW_GETCTL;
typedef struct _MASMW_HVSCRIPT				MASMW_HVSCRIPT, *PMASMW_HVSCRIPT;
typedef struct _MASMW_AUDIO_FORMAT			MASMW_AUDIO_FORMAT, *PMASMW_AUDIO_FORMAT;
typedef struct _MASMW_AUDIO_NOTIFYPOSITION	MASMW_AUDIO_NOTIFYPOSITION, *PMASMW_AUDIO_NOTIFYPOSITION;

/* struct definitions for MaSound_DeviceControlEx */
struct _MASMW_SURROUND_PARAM
{
	unsigned char 	*param;					/* pointer to DVX parameters */
	unsigned long 	size;					/* size of DVX parameter file */
};

struct _MASMW_EQUALIZER_PARAM
{
	unsigned char	sel;					/* equalizer number */
	unsigned short	ceq0;					/* parameter 0 */
	unsigned short	ceq1;					/* parameter 1 */
	unsigned short	ceq2;					/* parameter 2 */
	unsigned short	ceq3;					/* parameter 3 */
	unsigned short	ceq4;					/* parameter 4 */
};

struct _MASMW_LED_SEQ
{
	unsigned char	ctrl;					/* sequence step */
	unsigned char	led0;					/* sequence for LED0 */
	unsigned char	led1;					/* sequence for LED1 */
	unsigned char	led2;					/* sequence for LED2 */
};

typedef struct _MASMW_SURROUND_PARAM		MASMW_SURROUND_PARAM, *PMASMW_SURROUND_PARAM;
typedef struct _MASMW_EQUALIZER_PARAM		MASMW_EQUALIZER_PARAM, *PMASMW_EQUALIZER_PARAM;
typedef struct _MASMW_LED_SEQ				MASMW_LED_SEQ, *PMASMW_LED_SEQ;


#if defined(__cplusplus)
extern "C" {
#endif

signed long	MaSound_Initialize		( void );
signed long	MaSound_Terminate		( void );
signed long	MaSound_DeviceControl	( unsigned char 	p1,
									  unsigned char 	p2,
									  unsigned char 	p3,
									  unsigned char 	p4 );
signed long	MaSound_DeviceControlEx	( unsigned long 	p1,
									  unsigned long 	p2,
									  void *		 	p3 );
signed long	MaSound_Create			( unsigned char		srm_id );
signed long	MaSound_Load			( signed long 		func_id,
									  unsigned char *	file_ptr,
									  unsigned long		file_size,
									  unsigned char		mode,
									  signed long (* func)(unsigned char id),
									  void * 			ext_args );
signed long	MaSound_Open			( signed long 		func_id,
									  signed long		file_id,
									  unsigned short 	open_mode,
									  void * 			ext_args );
signed long	MaSound_Control			( signed long 		func_id,
									  signed long		file_id,
									  unsigned char		ctrl_num,
									  void * 			prm,
									  void * 			ext_args );
signed long	MaSound_Standby			( signed long 		func_id,
									  signed long		file_id,
									  void *			ext_args );
signed long	MaSound_Seek			( signed long 		func_id,
									  signed long		file_id,
									  unsigned long		pos,
									  unsigned char		flag,
									  void * 			ext_args );
signed long	MaSound_Start			( signed long 		func_id,
									  signed long		file_id,
									  unsigned short	play_mode,
									  void * 			ext_args );
signed long	MaSound_Pause			( signed long		func_id,
									  signed long		file_id,
									  void * 			ext_args );
signed long	MaSound_Restart			( signed long		func_id,
									  signed long		file_id,
									  void * 			ext_args );
signed long	MaSound_Stop			( signed long		func_id,
									  signed long		file_id,
									  void * 			ext_args );
signed long	MaSound_Close			( signed long		func_id,
									  signed long		file_id,
									  void *			ext_args );
signed long	MaSound_Unload			( signed long		func_id,
									  signed long		file_id,
									  void *			ext_args );
signed long	MaSound_Delete			( signed long 		func_id );

/*-------------------------------------------------------------------------*/
/* Phrase I/F                                                              */
/*-------------------------------------------------------------------------*/
#define	MAX_PHRASE_CHANNEL	(4)

typedef enum	_tagIdStatus {
	ID_STATUS_UNKNOWN	= 0,
	ID_STATUS_NODATA,
	ID_STATUS_READY,
	ID_STATUS_PLAY,
	ID_STATUS_ENDING,
	ID_STATUS_PAUSE,
	ID_STATUS_LINKSLAVE
} IDSTATUS;

struct	event {
	int	ch;
	int	mode;
};

struct info {
	long	MakerID;
	int		DeviceID;
	int		VersionID;
	int		MaxVoice;
	int		MaxChannel;
	int		SupportSMAF;
	long	Latency;
};

int				Phrase_Initialize	(void);
int				Phrase_Terminate	(void);
int				Phrase_GetInfo		(struct info* dat);
int				Phrase_CheckData	(unsigned char* data, unsigned long len);
int				Phrase_SetData		(int ch, unsigned char* data, unsigned long len, int check);
int				Phrase_Seek			(int ch, long pos);
int				Phrase_Play			(int ch, int loop);
int				Phrase_Stop			(int ch);
int				Phrase_Pause		(int ch);
int				Phrase_Restart		(int ch);
int				Phrase_Kill			(void);
void			Phrase_SetVolume	(int ch, int vol);
int				Phrase_GetVolume	(int ch);
void			Phrase_SetPanpot	(int ch, int vol);
int				Phrase_GetPanpot	(int ch);
int				Phrase_GetStatus	(int ch);
long			Phrase_GetPosition	(int ch);
long			Phrase_GetLength	(int ch);
int				Phrase_RemoveData	(int ch);
int				Phrase_SetEvHandler	(void (* func)(struct event*));
int				Phrase_SetLink		(int ch, unsigned long slave);
unsigned long	Phrase_GetLink		(int ch);


#if defined(__cplusplus)
}
#endif

#endif /*__MASOUND_H__*/
