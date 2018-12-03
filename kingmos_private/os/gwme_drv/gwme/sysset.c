/******************************************************
Copyright(c) ��Ȩ���У�1998-2005΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵����gwmeϵͳ����
�汾�ţ�3.0.0
����ʱ�ڣ�1999  
���ߣ�����
�޸ļ�¼��
******************************************************/

#include <eframe.h>
#include <eassert.h>
#include <gwmeobj.h>

// like GetSystemMetrics
#define CAPTION_HEIGHT 20
#define UNDEF_VALUE -1
#define SYS_INDEX_NUM  ( sizeof(iSysSettings) / sizeof(int) )

//Ĭ�ϵ�ϵͳ��������
static int iSysSettings[] = {
                   UNDEF_VALUE,//#define SM_CXSCREEN             0
                   UNDEF_VALUE,//#define SM_CYSCREEN             1
                   13,//#define SM_CXVSCROLL            2
                   13,//#define SM_CYHSCROLL            3
                   CAPTION_HEIGHT,//#define SM_CYCAPTION            4
                   1,//#define SM_CXBORDER             5
                   1,//#define SM_CYBORDER             6
                   2,//#define SM_CXDLGFRAME           7
                   2,//#define SM_CYDLGFRAME           8
                   2,//#define SM_CXFRAME              9
                   2,//#define SM_CYFRAME              10
                   32,//#define SM_CXICON               11
                   32,//#define SM_CYICON               12
                   32,//#define SM_CXCURSOR             13
                   32,//#define SM_CYCURSOR             14
                   18,//#define SM_CYMENU               15
                   UNDEF_VALUE,//#define SM_CXFULLSCREEN         16
                   UNDEF_VALUE,//#define SM_CYFULLSCREEN         17
                   13,//#define SM_CYVSCROLL            18
                   13,//#define SM_CXHSCROLL            19
                   0,//#define SM_DEBUG                20
                   40,//#define SM_CXMIN                21
                   40,//#define SM_CYMIN                22
                   17,//#define SM_CXSIZE               23
                   CAPTION_HEIGHT,//#define SM_CYSIZE               24
                   40,//#define SM_CXMINTRACK           25
                   40,//#define SM_CYMINTRACK           26
                   32,//#define SM_CXICONSPACING        27
                   32,//#define SM_CYICONSPACING        28
                   FALSE,//#define SM_PENWINDOWS           29
                   UNDEF_VALUE,//#define SM_CXMAXTRACK           30
                   UNDEF_VALUE,//#define SM_CYMAXTRACK           31
                   UNDEF_VALUE,//#define SM_CXMAXIMIZED          32
                   UNDEF_VALUE,//#define SM_CYMAXIMIZED          33
                   FALSE,//#define SM_NETWORK              34
                   16,//#define SM_CXSMICON             35
                   16,//#define SM_CYSMICON             36
                   13,//#define SM_CYVTHUMB             37
                   13,//#define SM_CXHTHUMB             38
                   UNDEF_VALUE,//#define SM_CXTOUCH              39
                   UNDEF_VALUE,//#define SM_CYTOUCH              40
                   UNDEF_VALUE,//#define SM_XTOUCHOFF            41
                   UNDEF_VALUE,//#define SM_YTOUCHOFF            42
//#ifdef ARM_CPU
  //                 (800 - 240) / 2,//0,//#define SM_XVIEW                43
    //               (600 - 320) / 2 - 35,//0,//#define SM_YVIEW                44
//#else
				   0,
				   0,
				   0,		//SM_CXSYSFONT
				   0,		//SM_CYSYSFONT
//#endif
				   0,	//SM_XCENTER              47		//�Ի������ X
				   0,	//SM_YCENTER              48		//�Ի������ Y
				   0,	//SM_CXCENTER             49		//�Ի������ CX
				   0,   //SM_CYCENTER             50		//�Ի������ CY					
				   0,	//SM_CYLISTITEM           51		//����List itemĬ�ϸ߶�
//				   0,   //SM_BITS_PERPEL          52        //ϵͳĬ�ϵ����ص��ʽ
};

#define CONTROL_FACE RGB( 0xf0, 0xf0, 0xf0 )
//Ĭ�ϵ�ϵͳ��ɫ����
static DWORD dwSysColor[SYS_COLOR_NUM] = {
//  COLOR_3DDKSHADOW            0
//  Dark shadow for three-dimensional display elements. 
//  ���-������ά��ʾ����Ӱ����
    RGB( 0x80, 0x80, 0x80 ),//CL_BLACK,

//  COLOR_3DFACE                1 
//  COLOR_BTNFACE               1
//  Face color for three-dimensional display elements. 
//  ����-������ά��ʾ�����沿��
    CONTROL_FACE,
    
//  COLOR_3DHILIGHT             2
//  COLOR_3DHIGHLIGHT           2
//  COLOR_BTNHILIGHT            2  
//  COLOR_BTNHIGHLIGHT          2
//  Highlight color for three-dimensional display elements 
//  (for edges facing the light source.) 
//  ����-������ά��ʾ�ĸ������� 
	RGB( 0xff, 0xff, 0xff ),
	
//  COLOR_3DLIGHT               3
//  Light color for three-dimensional display elements 
//  (for edges facing the light source.) 
//  ��-������ά��ʾ�������� 
    RGB( 0xf8, 0xf8, 0xf8 ),//RGB( 0xf0, 0xf0, 0xf0 ),

//  COLOR_3DSHADOW              4
//  COLOR_BTNSHADOW Shadow color for three-dimensional display elements 
//  (for edges facing away from the light source). 
//  ��-������ά��ʾ�ĺڲ��� 
    CL_BLACK,

//  COLOR_ACTIVEBORDER    5  
//  Active window border. 
//  ����ڵı߽���ɫ
	CL_BLACK,

//  COLOR_ACTIVECAPTION         6
//  Active window title bar. 
//  ����ڵı�������ɫ	
    RGB( 0xc0, 0xc0, 0xc0 ),

//  COLOR_APPWORKSPACE          7
//  Background color of multiple document interface (MDI) applications. 
//  ���ĵ�����ڵı���
	CL_WHITE,

//  COLOR_BACKGROUND            8
//  COLOR_DESKTOP               8
//  Desktop. 
//  ���汳��
	CL_WHITE,

//  COLOR_BTNTEXT               9
//  Text on push buttons. 
//  ��ť�ı�
    CL_BLACK,

//  COLOR_CAPTIONTEXT           10
//  Text in caption, size box, and scroll bar arrow box. 
//  �������ı���ɫ
	CL_BLACK,

//  COLOR_GRAYTEXT              11
//  Grayed (disabled) text. This color is set to 0 
//  if the current display driver does not support a solid gray color. 
//  ���ı� �� ʧЧ����	
    CL_DARKGRAY,

//  COLOR_HIGHLIGHT             12 
//  Item(s) selected in a control. 
//  ����-�ڿؼ��ﱻѡ�����Ŀ����
    CL_BLUE,
	
//  COLOR_HIGHLIGHTTEXT         13
//  Text of item(s) selected in a control. 
//  ����-�ڿؼ��ﱻѡ�����Ŀ���ı�    
    CL_YELLOW,

//  COLOR_INACTIVEBORDER        14
//  Inactive window border. 
//  �ǻ�߽�
    CL_BLACK,

//  COLOR_INACTIVECAPTION       15
//  Inactive window caption. 
//  �ǻ������
	RGB( 0xe0, 0xe0, 0xe0 ),

//  COLOR_INACTIVECAPTIONTEXT   16
//  Color of text in an inactive caption. 
//  �ǻ���������ı�
    CL_DARKGRAY, 

//  COLOR_INFOBK                17
//  Background color for ToolTip controls. 
//      
    CL_YELLOW,

//  COLOR_INFOTEXT              18
//  Text color for ToolTip controls. 
//      
    CL_BLACK,

//  COLOR_MENU                  19 
//  Menu background. 
//  �˵�����    
    CL_WHITE,

//  COLOR_MENUTEXT              20
//  Text in menus. 
//  �˵��ı�
    CL_BLACK,

//  COLOR_SCROLLBAR             21
//  Scroll bar gray area. 
//  ������
    RGB( 0x00, 0x66, 0xff ),

//  COLOR_STATIC                22
//  Background color for static controls and dialogs. 
//  ��̬�ؼ���Ի���ı���
    CONTROL_FACE,

//  COLOR_STATICTEXT            23
//  Text color for static controls
//  ��̬�ؼ��ı�
    CL_BLACK,

//  COLOR_WINDOW                24
//  Window background. 
//  ���ڱ���    
    CL_WHITE,

//  COLOR_WINDOWFRAME           25
//  Window frame. 
//  ���ڿ��    
    CL_BLACK,
	
//  COLOR_WINDOWTEXT            26
//  Text in windows.
//  �����ı�
    CL_BLACK,
	//scrollbar thumb
	CONTROL_FACE			
};

//Ĭ�ϵ�ϵͳˢ��
static HBRUSH hSysColorBrush[SYS_COLOR_NUM];


// **************************************************
// ������int WINAPI WinSys_SetMetrics( int nIndex, int iNewValue )
// ������
// 	IN nIndex - ����ֵ����ʾ��һ��������
// 	IN iNewValue - �µĶ���ֵ
// ����ֵ��
//	����ɹ��������ϵ�ֵ�����򣬷���0
// ����������
//	���ö���
// ����: 
//	ϵͳAPI
// ************************************************


int WINAPI WinSys_SetMetrics( int nIndex, int iNewValue )
{
	int v = 0;
	if( (UINT)nIndex < SYS_INDEX_NUM )
	{
		v = iSysSettings[nIndex];
		iSysSettings[nIndex] = iNewValue;
	}
	else
	{
		WARNMSG( 1, ("error at WinSys_SetMetrics: invalid index value=%d.\r\n", nIndex ) );
    	SetLastError( ERROR_INVALID_PARAMETER );		
	}
	return v;
}

// **************************************************
// ������int WINAPI WinSys_GetMetrics( int nIndex )
// ������
// 	IN nIndex - ����ֵ����ʾ��һ��������
// ����ֵ��
//	����ɹ��������ϵ�ֵ�����򣬷���0
// ����������
//	�õ�����	
// ����: 
//	ϵͳAPI
// ************************************************

int WINAPI WinSys_GetMetrics( int nIndex )
{
	if( (UINT)nIndex < SYS_INDEX_NUM )
	{
		return iSysSettings[nIndex];
	}
	WARNMSG( 1, ("error at WinSys_GetMetrics:invalid index value=%d.\r\n", nIndex ) );
	SetLastError( ERROR_INVALID_PARAMETER );
	return 0;
}


// **************************************************
// ������DWORD WINAPI WinSys_GetColor(int nIndex)
// ������
// 	IN nIndex - ����ֵ����ʾ��һ����ɫ��
// ����ֵ��
//	����ɹ��������ϵ�ֵ�����򣬷���0
// ����������
//	�õ�ϵͳ��ɫ
// ����: 
//	ϵͳAPI
// ************************************************

DWORD WINAPI WinSys_GetColor( int nIndex)
{
	if( (UINT)nIndex < SYS_COLOR_NUM )
		return dwSysColor[nIndex];
	else
	{
	    WARNMSG( 1, ("error at WinSys_GetColor:invalid index value=%d.\r\n", nIndex ) );
    	SetLastError( ERROR_INVALID_PARAMETER );
	}
	return 0;
}

// **************************************************
// ������BOOL WINAPI WinSys_SetColors(
//				  int cElements, 
//				  const int *lpaElements, 
//                  const COLORREF *lpaRgbValues )
// ������
// 	IN cElements - ��Ҫ���õ���ɫ����
// 	IN lpaElements - ��Ҫ���õ���ɫ������
// 	IN lpaRgbValues - COLORREFָ�룬��Ҫ���õ���ɫֵ
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�����µ�ϵͳ��ɫ
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI WinSys_SetColors(
				  int cElements, 
				  const int *lpaElements, 
                  const COLORREF *lpaRgbValues )
{
	extern BOOL _SetSysSolidBrushColor( HBRUSH hBrush , COLORREF color );
	int i;

	for( i = 0; i < cElements; i++ )
	{
	    if( (UINT)*lpaElements  < SYS_COLOR_NUM )
		{
		    dwSysColor[*lpaElements] = *lpaRgbValues;
			_SetSysSolidBrushColor( hSysColorBrush[*lpaElements], *lpaRgbValues );
		}
		else
		{
	        WARNMSG( 1, ("error at WinSys_SetColors:invalid index(%d) value=%d.\r\n", i, *lpaElements ) );
    	    SetLastError( ERROR_INVALID_PARAMETER );
    	    break;
		}
		lpaElements++;
		lpaRgbValues++;
	}
	return i;
}

// **************************************************
// ������HBRUSH WINAPI WinSys_GetColorBrush( int nIndex )
// ������
// 	IN nIndex - ����ֵ����ʾ��һ����ɫˢ�ӣ�
// ����ֵ��
//	����ɹ�������ˢ�Ӿ�������򣬷���NULL
// ����������
//	�õ�ϵͳˢ��
// ����: 
//	ϵͳAPI
// ************************************************

HBRUSH WINAPI WinSys_GetColorBrush( int nIndex )
{
	if( (UINT)nIndex < SYS_COLOR_NUM )
		return hSysColorBrush[nIndex];
	else
	{
        WARNMSG( 1, ("error at WinSys_GetColorBrush:invalid index value=%d.\r\n", nIndex ) );
	    SetLastError( ERROR_INVALID_PARAMETER );
	}
	return NULL;
}

// **************************************************
// ������BOOL _InitSysSetting( void )
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��ʼ��ϵͳ����
// ����: 
//	��gwmeϵͳ����ʱ���ص��øú���
// ************************************************

extern HBRUSH _CreateSysBrush( COLORREF color, UINT uiStyle );
extern HBRUSH _DeleteSysSolidBrush( HBRUSH hBrush );

BOOL _InitSysSetting( void )
{
	extern BOOL _GetSysFontSize( LPSIZE lpSize );
	SIZE size;
	int i;
	//��ʼ��ϵͳˢ��
	for( i = 0; i < SYS_COLOR_NUM; i++ )
	{
		hSysColorBrush[i] = _CreateSysBrush( WinSys_GetColor( i ), BS_SOLID );
		if( hSysColorBrush[i] == NULL )
			return FALSE;
	}
	//��ʼ��ϵͳ����
	iSysSettings[SM_CXMAXTRACK] = lpDisplayBitmap->bmWidth;
	iSysSettings[SM_CYMAXTRACK] = lpDisplayBitmap->bmHeight;
	iSysSettings[SM_CXMAXIMIZED] = lpDisplayBitmap->bmWidth;
	iSysSettings[SM_CYMAXIMIZED] = lpDisplayBitmap->bmHeight;
	iSysSettings[SM_CXTOUCH] = lpDisplayBitmap->bmWidth;
	iSysSettings[SM_CYTOUCH] = lpDisplayBitmap->bmHeight;
	iSysSettings[SM_XTOUCHOFF] = 0;
    iSysSettings[SM_YTOUCHOFF] = 0;
	iSysSettings[SM_CXFULLSCREEN] = lpDisplayBitmap->bmWidth;
	iSysSettings[SM_CYFULLSCREEN] = lpDisplayBitmap->bmHeight;
	iSysSettings[SM_CXSCREEN] = lpDisplayBitmap->bmWidth;
	iSysSettings[SM_CYSCREEN] = lpDisplayBitmap->bmHeight;

	iSysSettings[SM_CXCENTER] = lpDisplayBitmap->bmWidth;
	iSysSettings[SM_CYCENTER] = lpDisplayBitmap->bmHeight;

	if( _GetSysFontSize( &size ) )
	{
		iSysSettings[SM_CYCAPTION] = size.cy + 4;
		iSysSettings[SM_CYSIZE] = iSysSettings[SM_CYCAPTION];
		iSysSettings[SM_CXSIZE] = size.cx * 2 + 1;
		iSysSettings[SM_CYMENU] = size.cy + 2;
		iSysSettings[SM_CXSYSFONT] = size.cx;
		iSysSettings[SM_CYSYSFONT] = size.cy;
		iSysSettings[SM_CYLISTITEM] = size.cy + 4;
		
	}

	//
	return TRUE;
}

// **************************************************
// ������void _DeInitSysColorBrush( void )
// ������
//    ��
// ����ֵ��
//	��
// ����������
//	��gwmeϵͳ�˳�ʱ,�ص��øú���
// ����: 
// ************************************************

void _DeInitSysColorBrush( void )
{
	int i;
	//�ⲿ�ִ�����Բ�ȥ������Ϊgwmeϵͳ�˳�ʱҲ����������ϵͳ��Ҫ�ص�
	//��˲���Ҫȥ��_DeleteSysSolidBrush�Ĵ���
	//��Ϊ�˴���������ԣ�����-�ͷţ��Լ���ϵͳ���ܵ�Ӱ��΢��������ұ����ô��롣
	for( i = 0; i < SYS_COLOR_NUM; i++ )
	{
		_DeleteSysSolidBrush( hSysColorBrush[i] );
	}
}

