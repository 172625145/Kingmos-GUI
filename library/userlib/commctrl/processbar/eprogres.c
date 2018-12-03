/******************************************************
Copyright(c) ��Ȩ���У�1998-2005΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵����������������
�汾�ţ�1.0.0
����ʱ�ڣ�
���ߣ�
�޸ļ�¼��
******************************************************/

#include <ewindows.h>
#include <edef.h>

#include <eprogres.h>

typedef struct _tag_Progress_Data{
	int			iCurrent;  // �������ĵ�ǰλ��
	int			iIncrement; // ����
	PBRANGE		Pbr;   // ��Χ
	COLORREF    clBarColor;  // ǰ����ɫ
	COLORREF    clBackGround; // ������ɫ
}_PROGRESSDATA,*_LPPROGRESSDATA;


typedef struct _SCRLBAR_BRUSH
{
	HBRUSH hbrushVSBackground;
	HBRUSH hbrushVSThumb;
	HBRUSH hbrushHSBackground;
	HBRUSH hbrushHSThumb;
	HBRUSH hbrushCorner;
	HBRUSH hbrushUpArrow;
	HBRUSH hbrushDownArrow;
	HBRUSH hbrushLeftArrow;
	HBRUSH hbrushRightArrow;
}SCRLBAR_BRUSH, FAR * LPSCRLBAR_BRUSH;

static HBITMAP hbmpVSBackground = NULL;
static HBITMAP hbmpVSThumb= NULL;
static HBITMAP hbmpHSBackground = NULL;
static HBITMAP hbmpHSThumb= NULL;
static HBITMAP hbmpCorner= NULL;
static HBITMAP hbmpUpArrow= NULL;
static HBITMAP hbmpDownArrow= NULL;
static HBITMAP hbmpLeftArrow= NULL;
static HBITMAP hbmpRightArrow= NULL;

static SCRLBAR_BRUSH *lpscrl_brush = NULL;
static SCRLBAR_BRUSH scrlBrush;

//  �����������õ�ˢ��
static HBRUSH CreateProgressBrush( UINT id, HBITMAP *lphBitmap )
{
	//HBITMAP hBitmap;

	if( *lphBitmap == NULL )
		*lphBitmap = LoadImage( NULL, MAKEINTRESOURCE( id ), IMAGE_BITMAP, 0, 0, LR_SHARED );
	
	if( *lphBitmap )
	{
		LOGBRUSH lb;
		
		lb.lbColor = 0;
		lb.lbHatch = (LONG)*lphBitmap;
		lb.lbStyle = BS_PATTERN;
		return CreateBrushIndirect ( &lb ); 
	}
	return NULL;
}

static SCRLBAR_BRUSH * InitProgressBrush( ) //LPSCRLBAR_BRUSH lpscrlBrush )
{
	if( lpscrl_brush == NULL )
	{
	//if( hbmpVSBackground == NULL )
	//{ //��һ�Σ���ʼ��...
		scrlBrush.hbrushVSBackground = CreateProgressBrush( OBM_VSCROLLBAR_BACKGROUND, &hbmpVSBackground );
	//}
	
	//if( hbrushHSBackground == NULL )
	//{ //��һ�Σ���ʼ��...
		scrlBrush.hbrushHSBackground = CreateProgressBrush( OBM_HSCROLLBAR_BACKGROUND, &hbmpHSBackground );
	//}

	//if( hbrushVSThumb == NULL )
	//{ //��һ�Σ���ʼ��...
		scrlBrush.hbrushVSThumb = CreateProgressBrush( OBM_VPROGRESS_THUMB, &hbmpVSThumb );
	//}

	//if( hbrushHSThumb == NULL )
	//{ //��һ�Σ���ʼ��...
		scrlBrush.hbrushHSThumb = CreateProgressBrush( OBM_HPROGRESS_THUMB, &hbmpHSThumb );
	//}
	
	//if( hbrushCorner == NULL )
	//{
		//scrlBrush.hbrushCorner = CreateProgressBrush( OBM_SCROLLBAR_CORNER, &hbmpCorner );
	//}
	//if( hbrushUpArrow == NULL )
	//{
		scrlBrush.hbrushUpArrow = CreateProgressBrush( OBM_VSCROLLBAR_UPARROW, &hbmpUpArrow );
	//}
	//if( hbrushDownArrow == NULL )
	//{
		scrlBrush.hbrushDownArrow = CreateProgressBrush( OBM_VSCROLLBAR_DOWNARROW, &hbmpDownArrow );
	//}
	//if( hbrushLeftArrow == NULL )
	//{
		scrlBrush.hbrushLeftArrow = CreateProgressBrush( OBM_HSCROLLBAR_LEFTARROW, &hbmpLeftArrow );
	//}
	//if( hbrushRightArrow == NULL )
	//{
		scrlBrush.hbrushRightArrow = CreateProgressBrush( OBM_HSCROLLBAR_RIGHTARROW, &hbmpRightArrow );
	//}
		lpscrl_brush = &scrlBrush;
	}
		
	return lpscrl_brush;
}

static BOOL DeinitProgressBrush( )//LPSCRLBAR_BRUSH lpscrlBrush )
{
	if( lpscrl_brush )
	{
		if( lpscrl_brush->hbrushVSBackground )
		{
			DeleteObject( lpscrl_brush->hbrushVSBackground );
			DeleteObject( hbmpVSBackground );
		}
		if( lpscrl_brush->hbrushHSBackground )
		{
			DeleteObject( lpscrl_brush->hbrushHSBackground );
			DeleteObject( hbmpHSBackground  );
		}
		if( lpscrl_brush->hbrushVSThumb )
		{
			DeleteObject( lpscrl_brush->hbrushVSThumb );
			DeleteObject( hbmpVSThumb );
		}
		if( lpscrl_brush->hbrushHSThumb )
		{
			DeleteObject( lpscrl_brush->hbrushHSThumb );
			DeleteObject( hbmpHSThumb );
		}
		if( lpscrl_brush->hbrushCorner )
		{
			DeleteObject( lpscrl_brush->hbrushCorner );
			DeleteObject( hbmpCorner );
		}
		if( lpscrl_brush->hbrushUpArrow )
		{
			DeleteObject( lpscrl_brush->hbrushUpArrow );
			DeleteObject( hbmpUpArrow );
		}
		if( lpscrl_brush->hbrushDownArrow )
		{
			DeleteObject( lpscrl_brush->hbrushDownArrow );
			DeleteObject( hbmpDownArrow );
		}
		if( lpscrl_brush->hbrushLeftArrow )
		{
			DeleteObject( lpscrl_brush->hbrushLeftArrow );
			DeleteObject( hbmpLeftArrow );
		}
		if( lpscrl_brush->hbrushRightArrow )
		{
			DeleteObject( lpscrl_brush->hbrushRightArrow );
			DeleteObject(  hbmpRightArrow );
		}
		lpscrl_brush = NULL;
	}
}

static void FillScrollRect( HDC hdc, RECT * lprc, HBRUSH hBrush )
{
	SelectObject( hdc, hBrush );
	SetBrushOrgEx( hdc, lprc->left, lprc->top, NULL );
	BitBlt( hdc, lprc->left, lprc->top, lprc->right - lprc->left, lprc->bottom - lprc->top, NULL, 0, 0, PATCOPY );
}

// **************************************************
// ������static void PaintSmoothProgress(HWND hWnd,HDC hdc,RECT rt,int iPercent,BOOL isNomal)
// ������
// 	IN hWnd  -- ���ھ��
// 	IN hdc  -- �豸���
// 	IN rt  -- RECT Ҫ���Ƶľ��δ�С
// 	IN iPercent  -- ��ǰ�İٷֱ�
// 	IN isNormal  -- �Ƿ�����״̬���ƣ��� -- ˮƽ������ƣ��� -- ��ֱ�������
// ����ֵ����
// �������������ƹ⻬������
// ����: 
// ************************************************/
static void PaintSmoothProgress(HWND hWnd,HDC hdc,RECT rt,int iPercent,BOOL isNomal)
{
	
	HBRUSH				hBrush;
	_LPPROGRESSDATA			_ppgd;
	RECT rectDraw;
	
	_ppgd = (_LPPROGRESSDATA)GetWindowLong(hWnd,GWL_USERDATA);  // �õ�����������
	ASSERT(_ppgd!=NULL);
	
	rectDraw = rt;
	//		FillRect(hdc,&rt,GetStockObject(WHITE_BRUSH));
	if(isNomal==TRUE)
	{   // ˮƽ������
		// ���ƽ���
		//�屳��
		RECT rcThumb;
		FillScrollRect( hdc, &rt, lpscrl_brush->hbrushHSBackground );

		//hBrush=CreateSolidBrush(_ppgd->clBarColor);
		//FillRect(hdc,&rectDraw,hBrush);
		//DeleteObject(hBrush);
		// ��ͷ
		rectDraw.right = rectDraw.left + 1;
		FillScrollRect( hdc, &rectDraw, lpscrl_brush->hbrushLeftArrow );

		//thumb
		rectDraw.left = rectDraw.right;
		rectDraw.right=rectDraw.left+(iPercent*(rt.right-rt.left-2))/100;
		rcThumb = rectDraw;
		rcThumb.top++;
		rcThumb.bottom--;
		FillScrollRect( hdc, &rcThumb, lpscrl_brush->hbrushHSThumb );
		
		// ����ʣ��ı���
		rectDraw.left = rectDraw.right;
		rectDraw.right=rt.right-1;
		//hBrush=CreateSolidBrush(_ppgd->clBackGround);
		//FillRect(hdc,&rectDraw,hBrush);
		//DeleteObject(hBrush);
		//FillScrollRect( hdc, &rectDraw, lpscrl_brush->hbrushHSBackground );

		//β
		rectDraw.left = rectDraw.right;
		rectDraw.right = rt.right;

		FillScrollRect( hdc, &rectDraw, lpscrl_brush->hbrushRightArrow );


	}
	else
	{  // ��ֱ������
		// ���ƽ���
		RECT rcThumb;
		//�屳��
		FillScrollRect( hdc, &rt, lpscrl_brush->hbrushVSBackground );

		
		// ��ͷ
		rectDraw.bottom = rectDraw.top + 1;
		FillScrollRect( hdc, &rectDraw, lpscrl_brush->hbrushUpArrow );
		
		//rectDraw.top = rectDraw.bottom;
		rectDraw.top=rectDraw.top+((100-iPercent)*(rt.bottom-rt.top-2))/100;
		rectDraw.bottom = rt.bottom - 1;

		rcThumb = rectDraw;
		rcThumb.left++;
		rcThumb.right--;
		FillScrollRect( hdc, &rcThumb, lpscrl_brush->hbrushVSThumb );

		//hBrush=CreateSolidBrush(_ppgd->clBarColor);
		//FillRect(hdc,&rt,hBrush);
		//DeleteObject(hBrush);
		
		// ����ʣ��ı���
		rectDraw.bottom = rectDraw.top;
		rectDraw.top=rt.top;
		
		//hBrush=CreateSolidBrush(_ppgd->clBackGround);
		//FillRect(hdc,&rt,hBrush);
		//DeleteObject(hBrush);
		rectDraw.top = rt.bottom - 1;;
		rectDraw.bottom = rt.bottom;

		FillScrollRect( hdc, &rectDraw, lpscrl_brush->hbrushRightArrow );

	}
}
// **************************************************
// ������static void PaintNomalProgress(HDC hdc,RECT rt,int iPercent,int iInc,BOOL isNomal)
// ������
// 	IN hdc  -- �豸���
// 	IN rt  -- RECT Ҫ���Ƶľ��δ�С
// 	IN iPercent  -- ��ǰ�İٷֱ�
// 	IN iInc -- ÿһ���������ֵ
// 	IN isNormal  -- �Ƿ�����״̬���ƣ��� -- ˮƽ������ƣ��� -- ��ֱ�������
// ����ֵ�� ��
// ��������������ͨ�ý�������Ϊһ������ʾ��
// ����: 
// **************************************************
static void PaintNomalProgress(HDC hdc,RECT rt,int iPercent,int iInc,BOOL isNomal)
{
	HBRUSH				hBrush;
	int					i;
	RECT				rtBox;
	
	// ���Ʊ���
	FillRect(hdc,&rt,GetStockObject(WHITE_BRUSH));
	memcpy(&rtBox,&rt,sizeof(RECT));
	if(isNomal==TRUE)
	{  // ˮƽ������
		// ���ƽ���
		rt.right=rt.left+(iPercent*(rt.right-rt.left))/100;
		hBrush=CreateSolidBrush(CL_DARKBLUE);
		rtBox.right=rt.left;
		for(i=0;i<rt.right-iInc;i+=iInc)
		{// ����һ������
			rtBox.left=rtBox.right+1;
			rtBox.right=rtBox.left+iInc-1;
			FillRect(hdc,&rtBox,hBrush);
		}
		DeleteObject(hBrush);
	}
	else
	{  // ��ֱ������
		// ���ƽ���
		rt.top=rt.top+((100-iPercent)*(rt.bottom-rt.top))/100;
		hBrush=CreateSolidBrush(CL_DARKBLUE);
		rtBox.bottom=rt.top;
		for(i=0;i<rt.bottom-iInc;i+=iInc)
		{// ����һ������
			rtBox.top=rtBox.bottom+1;
			rtBox.bottom=rtBox.top+iInc-1;
			FillRect(hdc,&rtBox,hBrush);
		}
		DeleteObject(hBrush);
	}
}
// **************************************************
// ������static void OnPaintProgress(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// ����ֵ����
// �������������ƽ�����
// ����: 
// **************************************************
static void OnPaintProgress(HWND hWnd)
{
	_LPPROGRESSDATA			_ppgd;
	int						iPercent;
	DWORD					style;
	RECT					rt;
	HDC						hdc;
	PAINTSTRUCT				ps;
	hdc = BeginPaint( hWnd, &ps );
	style=GetWindowLong(hWnd,GWL_STYLE);
	_ppgd=(_LPPROGRESSDATA)GetWindowLong(hWnd,GWL_USERDATA);  // �õ�����������
	ASSERT(_ppgd!=NULL);
	GetClientRect(hWnd,&rt);
	iPercent=(_ppgd->iCurrent*100)/(_ppgd->Pbr.iHigh-_ppgd->Pbr.iLow); // �õ��ٷֱ�
	if(style&PBS_SMOOTH)
	{ // ��ʾ�⻬������
		if(style&PBS_VERTICAL)
			PaintSmoothProgress(hWnd,hdc,rt,iPercent,FALSE); // ��ʾ��ֱ�⻬������
		else
			PaintSmoothProgress(hWnd,hdc,rt,iPercent,TRUE);  // ��ʾˮƽ�⻬������
	}
	else
	{ // ��ʾ��״������
		if(style&PBS_VERTICAL)
			PaintNomalProgress(hdc,rt,iPercent,_ppgd->iIncrement,FALSE); // ��ʾ��ֱ��״������
		else
			PaintNomalProgress(hdc,rt,iPercent,_ppgd->iIncrement,TRUE); // ��ʾˮƽ��״������
	}
	EndPaint( hWnd, &ps );
	return ;
}
// **************************************************
// ������static BOOL OnCreateProgress(HWND hWnd)
// ������
// 	IN hWnd --  ���ھ��
// ����ֵ�� �ɹ�����TRUE�����򷵻�FALSE��
// ��������������������
// ����: 
// **************************************************
static BOOL OnCreateProgress(HWND hWnd)
{
	_LPPROGRESSDATA			_ppgd;
	_ppgd=(_LPPROGRESSDATA)malloc(sizeof(_PROGRESSDATA)); // ����������ṹ
	ASSERT(_ppgd!=NULL);
	memset( _ppgd, 0, sizeof(_PROGRESSDATA) );
	if(_ppgd==NULL)
		return FALSE;
	// ��ʼ������
	_ppgd->iIncrement=10;
	_ppgd->iCurrent=0;
	_ppgd->Pbr.iLow=0;
	_ppgd->Pbr.iHigh=100;
//	_ppgd->clBarColor = CL_DARKBLUE;
	_ppgd->clBarColor = GetSysColor(COLOR_HIGHLIGHT);
//	_ppgd->clBackGround = CL_WHITE;
	_ppgd->clBackGround = GetSysColor(COLOR_WINDOW);

	SetWindowLong(hWnd,GWL_USERDATA,(long)_ppgd); // ���ô�������
	return TRUE;
}
// **************************************************
// ������static int DoSetRange(HWND hWnd,int nMinRange,int nMaxRange)
// ������
// 	IN hWnd -- ���ھ��
// 	IN nMinRange -- ��Χ��Сֵ
// 	IN nMaxRange -- ��Χ���ֵ
// ����ֵ�����ؾɵķ�Χ LOWORD -- ��Сֵ�� HIWORD -- ���ֵ
// �������������ý�������Χ������PBM_SETRANGE��Ϣ��
// ����: 
// **************************************************
static int DoSetRange(HWND hWnd,int nMinRange,int nMaxRange)
{
	_LPPROGRESSDATA			_ppgd;
	DWORD					dOld;
	RECT					rt;
	_ppgd=(_LPPROGRESSDATA)GetWindowLong(hWnd,GWL_USERDATA);  // �õ�����������
	ASSERT(_ppgd!=NULL);
	dOld=MAKELONG(_ppgd->Pbr.iLow,_ppgd->Pbr.iHigh);
	// ���ý������ķ�Χ
	_ppgd->Pbr.iLow=nMinRange; // ��Сֵ
	_ppgd->Pbr.iHigh=nMaxRange; // ���ֵ
	_ppgd->iCurrent=0; // ��ǰֵΪ0
	GetClientRect(hWnd,&rt);
	InvalidateRect(hWnd,&rt,TRUE); // ��Ч����
	return dOld;
}
// **************************************************
// ������static int DoSetPos(HWND hWnd,int nNewPos)
// ������
// 	IN hWnd -- ���ھ��
// 	IN nNewPos -- Ҫ���õ��µĵ�ǰֵ
// ����ֵ�����ؾɵ�ֵ
// �������������õ�ǰλ�� ������PBM_SETPOS��Ϣ
// ����: 
// **************************************************
static int DoSetPos(HWND hWnd,int nNewPos)
{
	_LPPROGRESSDATA			_ppgd;
	RECT					rt;
	int						iOld;
	_ppgd=(_LPPROGRESSDATA)GetWindowLong(hWnd,GWL_USERDATA);  // �õ�����������
	ASSERT(_ppgd!=NULL);
	if((nNewPos<_ppgd->Pbr.iLow)||(nNewPos>_ppgd->Pbr.iHigh))
		return 0; // Ҫ���õ�ֵ��Ч
	iOld=_ppgd->iCurrent;
	_ppgd->iCurrent=nNewPos; // �����µĵ�ǰֵ
	GetClientRect(hWnd,&rt);
	InvalidateRect(hWnd,&rt,TRUE); // ��Ч����
	return iOld; // ���ؾɵ�ֵ
}
// **************************************************
// ������static int DoDeltaPos(HWND hWnd,int nIncrement)
// ������
// 	IN hWnd -- ���ھ��
// 	IN nIncrement -- Ҫ���ӵ�ֵ
// ����ֵ�����ؾɵĵ�ǰֵ
// �����������ڵ�ǰֵ����ָ��������ֵ������PBM_DELTAPOS��Ϣ��
// ����: 
// **************************************************
static int DoDeltaPos(HWND hWnd,int nIncrement)
{
	_LPPROGRESSDATA			_ppgd;
	RECT					rt;
	int						iOldCurrent;
	_ppgd=(_LPPROGRESSDATA)GetWindowLong(hWnd,GWL_USERDATA);  // �õ�����������
	ASSERT(_ppgd!=NULL);
	
	iOldCurrent = _ppgd->iCurrent; // �õ���ǰ��ֵ
	if(((_ppgd->iCurrent+nIncrement)<_ppgd->Pbr.iLow)||((_ppgd->iCurrent+nIncrement)>_ppgd->Pbr.iHigh))
		return 0; // ����������������Ч
	_ppgd->iCurrent+=nIncrement; // ����Ҫ���ӵ�ֵ
	GetClientRect(hWnd,&rt); 
	InvalidateRect(hWnd,&rt,TRUE); // ��Ч����
	return iOldCurrent;
}
// **************************************************
// ������static int DoSetStep(HWND hWnd,int nStepInc)
// ������
// 	IN hWnd -- ���ھ��
// 	IN nStepInc -- ����
// ����ֵ������ԭ���Ĳ�����
// ��������������ÿһ��������ֵ������PBM_SETSTEP��Ϣ��
// ����: 
// **************************************************
static int DoSetStep(HWND hWnd,int nStepInc)
{
	_LPPROGRESSDATA			_ppgd;
	RECT					rt;
	int						iInc;
	_ppgd=(_LPPROGRESSDATA)GetWindowLong(hWnd,GWL_USERDATA); // �õ�����������
	ASSERT(_ppgd!=NULL);
	iInc=_ppgd->iIncrement; // ��������
	_ppgd->iIncrement=nStepInc; // �����µ�����
	GetClientRect(hWnd,&rt);
	InvalidateRect(hWnd,&rt,TRUE); // ��Ч����
	return iInc;
}
// **************************************************
// ������static int DoStepIt(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// ����ֵ������ԭ����λ��
// ��������������һ������������PBM_STEPIT��Ϣ
// ����: 
// **************************************************
static int DoStepIt(HWND hWnd)
{
	_LPPROGRESSDATA			_ppgd;
	RECT					rt;
	_ppgd=(_LPPROGRESSDATA)GetWindowLong(hWnd,GWL_USERDATA);  // �õ�����������
	ASSERT(_ppgd!=NULL);
	if(((_ppgd->iCurrent+_ppgd->iIncrement)<_ppgd->Pbr.iLow)||((_ppgd->iCurrent+_ppgd->iIncrement)>_ppgd->Pbr.iHigh))
		return 0; // ����һ����������Ч
	_ppgd->iCurrent+=_ppgd->iIncrement; // ����һ����������Ч
	GetClientRect(hWnd,&rt);
	InvalidateRect(hWnd,&rt,TRUE); // ��Ч����
	return (_ppgd->iCurrent-_ppgd->iIncrement); // ����ԭ���ĵ�ǰֵ
}
// **************************************************
// ������static int DoGetRange(HWND hWnd,BOOL fWhitchLimit,PPBRANGE ppBRange)
// ������
// 	IN hWnd -- ���ھ��
// 	IN fWhitchLimit -- ָ��Ҫ������һ������ֵ��
// 						TRUE -- ������Сֵ��
// 						FALSE -- �������ֵ��
// 	OUT ppBRange -- Ҫ���ص�ǰ�������ķ��أ����ΪNULL ����ֻ����fWhitchLimitָ����ֵ��
// ����ֵ������fWhitchLimitָ��������ֵ��
// �����������õ���ǰ�Ľ������ķ�Χ������PBM_GETRANGE��Ϣ��
// ����: 
// **************************************************
static int DoGetRange(HWND hWnd,BOOL fWhitchLimit,PPBRANGE ppBRange)
{
	_LPPROGRESSDATA			_ppgd;
	_ppgd=(_LPPROGRESSDATA)GetWindowLong(hWnd,GWL_USERDATA);  // �õ�����������
	ASSERT(_ppgd!=NULL);
	ppBRange->iHigh=_ppgd->Pbr.iHigh; // �������ֵ
	ppBRange->iLow=_ppgd->Pbr.iLow; // ������Сֵ
	if (fWhitchLimit == TRUE)
		return _ppgd->Pbr.iLow; // ������Сֵ
	else
		return _ppgd->Pbr.iHigh; // �������ֵ
}
// **************************************************
// ������static UINT DoGetPos(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// ����ֵ�����ؽ������ĵ�ǰֵ��
// �����������õ��������ĵ�ǰֵ������PBM_GETPOS��Ϣ��
// ����: 
// **************************************************
static UINT DoGetPos(HWND hWnd)
{
	_LPPROGRESSDATA			_ppgd;
	_ppgd=(_LPPROGRESSDATA)GetWindowLong(hWnd,GWL_USERDATA);  // �õ�����������
	ASSERT(_ppgd!=NULL);
	return _ppgd->iCurrent; // ���ص�ǰֵ
}
// **************************************************
// ������static LRESULT SetBarColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN  hWnd -- ���ھ��
// 	IN  wParam -- ����
// 	IN  lParam -- COLORREF Ҫ���õ���ɫ��
// ����ֵ������ԭ������ɫ��
// ��������������BAR����ɫ������PBM_SETBARCOLOR��Ϣ
// ����: 
// **************************************************
static LRESULT SetBarColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	_LPPROGRESSDATA			_ppgd;
	COLORREF oldColor;

		_ppgd=(_LPPROGRESSDATA)GetWindowLong(hWnd,GWL_USERDATA);  // �õ�����������
		ASSERT(_ppgd!=NULL);
		oldColor = _ppgd->clBarColor;
		_ppgd->clBarColor = (COLORREF)lParam; // ���ý���������ɫ
		return oldColor;
}
// **************************************************
// ������static LRESULT SetBKColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN  hWnd -- ���ھ��
// 	IN  wParam -- ����
// 	IN  lParam -- COLORREF Ҫ���õı�����ɫ��
// ����ֵ������ԭ�еı�����ɫ��
// ��������������BAR�ı�����ɫ������PBM_SETBKCOLOR��Ϣ��
// ����: 
// **************************************************
static LRESULT SetBKColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	_LPPROGRESSDATA			_ppgd;
	COLORREF oldColor;

		_ppgd=(_LPPROGRESSDATA)GetWindowLong(hWnd,GWL_USERDATA);  // �õ�����������
		ASSERT(_ppgd!=NULL);
		oldColor = _ppgd->clBackGround; // �õ�ԭ���ı���ɫ
		_ppgd->clBackGround = (COLORREF)lParam; // ���ñ���ɫ
		return oldColor; // ����ԭ������ɫ
}

// **************************************************
// ������static void OnDestroyProgress(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// ����ֵ����
// �����������ƻ�������������WM_DESTROY��Ϣ��
// ����: 
// **************************************************
static void OnDestroyProgress(HWND hWnd)
{
	_LPPROGRESSDATA			_ppgd;
	_ppgd=(_LPPROGRESSDATA)GetWindowLong(hWnd,GWL_USERDATA);  // �õ�����������
	ASSERT(_ppgd!=NULL);
	free(_ppgd); // �ͷ����ݽṹ
}
// **************************************************
// ������static LRESULT CALLBACK ProgressProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
// 	IN  hWnd -- ���ھ��
// 	IN  message -- Ҫ�������Ϣ
// 	IN  wParam -- ��Ϣ����
// 	IN  lParam -- ��Ϣ����
// ����ֵ��������Ϣ�Ĵ�������
// ���������������������Ϣ��
// ����: 
// **************************************************
static LRESULT CALLBACK	ProgressProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_PAINT:  // ���ƽ�����
			OnPaintProgress(hWnd);
			return 0;
		case PBM_SETRANGE: // ���÷�Χ
			return DoSetRange(hWnd,LOWORD(lParam),HIWORD(lParam));
		case PBM_SETPOS: // ���õ�ǰλ��
			return DoSetPos(hWnd,wParam);
		case PBM_DELTAPOS: // ����һ������
			return DoDeltaPos(hWnd,wParam);
		case PBM_SETSTEP: // ����һ��
			return DoSetStep(hWnd,wParam);
		case PBM_STEPIT: // ��������
			return DoStepIt(hWnd);
	//	case PBM_SETRANGE32:
		case PBM_GETRANGE: // �õ���Χ
			return DoGetRange(hWnd,(BOOL)wParam,(PPBRANGE)lParam);
		case PBM_GETPOS: // �õ���ǰλ��
			return DoGetPos(hWnd);
		case PBM_SETBARCOLOR: // ���ý�������ɫ
			return SetBarColor(hWnd,wParam,lParam);
		case PBM_SETBKCOLOR: // ���ñ���ɫ
			return SetBKColor(hWnd,wParam,lParam);
		case WM_DESTROY: // �ƻ�����
			OnDestroyProgress(hWnd);
			return 0;
		case WM_CREATE: // ����������
			if(OnCreateProgress(hWnd)==FALSE)
				return -1;
			return 0;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}



// **************************************************
// ������ATOM RegisterProgressClass(HINSTANCE hInstance)
// ������
// 	IN hInstance -- ʵ�����
// ����ֵ������ע������
// ����������ע���������������
// ����: 
// **************************************************
ATOM RegisterProgressClass(HINSTANCE hInstance)
{
	WNDCLASS    wc;

	InitProgressBrush();

	wc.style			= CS_HREDRAW | CS_VREDRAW|CS_DBLCLKS;
	wc.lpfnWndProc		= (WNDPROC)ProgressProc; // ���ù��̺���
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance; // ����ʵ�����
	wc.hIcon			= NULL;
	wc.hCursor			= NULL;
	wc.hbrBackground	= GetStockObject( WHITE_BRUSH );
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= classPROGRESS; // ���ý�����������



	return RegisterClass(&wc);
}
