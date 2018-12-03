/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵����ƴ������  ƴд����
�汾�ţ�1.0.0
����ʱ�ڣ�2004-05-21
���ߣ��½��� Jami chen
��ƹ滮��
		����ʾƴд���ķ����ϣ����������ַ���
		1����ʾ����Ĵ�С��ƴд����Ŀ��������Ŀ������������
		   ���ʵ���С��ʹÿһ����Ŀ���ʵ��ļ�༴�ɣ���PLAN_1������
		2����ʾ�������С���㣬����ƴ����Ŀ�ĳ�����������ÿһ����Ŀ
		   ����Ĵ�С����PLAN_2������
�޸ļ�¼��
******************************************************/
#include <ewindows.h>
#include <ecomctrl.h>
#include "resource.h"
#include <softkey.h>
#include "pysearch.h"



/***************  ȫ���� ���壬 ���� *****************/

//#define PLAN_1
#define PLAN_2


const TCHAR classSpellArea[] = "SPELLAREA";


typedef struct SpellAreaStruct{
	SPELLLIST SpellList; // ��ǰ��ƴ��ƴд���
	int iCurSpellPage; // ��ǰƴд��ҳ����
	int iCurSel; // ��ǰѡ���ƴд

	COLORREF cl_Text;  // �ı���ɫ
	COLORREF cl_Bk;  // ������ɫ
	COLORREF cl_SelBk; // ѡ�񱳾���ɫ
	COLORREF cl_SelText; //ѡ���ı���ɫ

	HWND hOwner;
}SPELLAREA,*LPSPELLAREA;


#ifdef PLAN_1
#define SPACE_WIDTH  4
#endif
// ********************************************************************
// ��������
// ********************************************************************
ATOM RegisterSpellAreaWindow(HINSTANCE hInstance); // ע����д��������

static LRESULT CALLBACK SpellAreaWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DoPaint(HWND hWnd,HDC hdc);
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);

static void DoInsertKey(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetSelSpell(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoClearSpell(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDeleteKey(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetKeyNum(HWND hWnd,WPARAM wParam,LPARAM lParam);

static void ClearSpellArea(HWND hWnd);
static void ShowSpellArea(HWND hWnd,HDC hdc);

static int GetNewSelIndex(LPSPELLAREA lpSpellArea,LPTSTR lpOldSelSpell);

#ifdef PLAN_1
static void SetCurrentSel(HWND hWnd,LPSPELLAREA lpSpellArea, int iIndex, BOOL bShow);
static void AdjustWindowSize(HWND hWnd,LPSPELLAREA lpSpellArea);
static UINT GetStringWidth(HWND hWnd,LPTSTR lpString);
static int GetItemWidth(HWND hWnd,LPSPELLAREA lpSpellArea);
static int GetSpellIndex(HWND hWnd,LPSPELLAREA lpSpellArea,POINT point);
static void ShowSpellItem(HWND hWnd,HDC hdc,LPSPELLAREA lpSpellArea, int iIndex);
#endif

#ifdef PLAN_2

#endif

// ********************************************************************
// ������ATOM RegisterHandWriteAreaWindow(HINSTANCE hInstance)
// ������
//	IN hInstance - ʵ�����
// ����ֵ��
// 	��
// ����������ע����д��������
// ����: 
// ********************************************************************
ATOM RegisterSpellAreaWindow(HINSTANCE hInstance)
{
	WNDCLASS wcex;

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)SpellAreaWndProc;  // ��д���������̺���
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= sizeof(long);
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= NULL;//GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= classSpellArea;
	return RegisterClass(&wcex); // ��ϵͳƴд����
}

// ********************************************************************
// ������static LRESULT CALLBACK SpellAreaWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
//    IN message - ������Ϣ
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	��ͬ����Ϣ�в�ͬ�ķ���ֵ�����忴��Ϣ����
// ����������Ӧ�ó��򴰿ڹ��̺���
// ����: 
// ********************************************************************
static LRESULT CALLBACK SpellAreaWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_PAINT: // ������Ϣ
			hdc = BeginPaint(hWnd, &ps);
			DoPaint(hWnd,hdc);
			EndPaint(hWnd, &ps);
			break;

		case WM_LBUTTONDOWN: // ��������������
			return DoLButtonDown( hWnd, wParam,lParam );
		case WM_LBUTTONUP: // ��������������
			return DoLButtonUp( hWnd, wParam,lParam );
		case WM_MOUSEMOVE: // ��������ƶ�
			return DoMouseMove( hWnd, wParam,lParam );

		case WM_COMMAND: // ������Ϣ
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_WINDOWPOSCHANGING: // ������λ�øı�

			((LPWINDOWPOS)lParam)->flags |= SWP_NOACTIVATE;
			return 0;
		case WM_CREATE: // ������Ϣ
			return DoCreateWindow(hWnd,wParam,lParam);
		case WM_DESTROY: // �ƻ�����
			//PostQuitMessage(0);
			break;

		case SPM_INSERTKEY: // ����һ������ƴд����
			DoInsertKey(hWnd,wParam,lParam);
			break;
		case SPM_GETSELSPELL:  // �õ���ǰ��ѡ��ƴд
			return DoGetSelSpell(hWnd,wParam,lParam);

		case SPM_CLEARSPELL:	// ������е�ƴд
			DoClearSpell(hWnd,wParam,lParam);
			break;

		case SPM_DELETEKEY:  // ɾ��һ����
			DoDeleteKey(hWnd,wParam,lParam);
			break;
		case SPM_GETKEYNUM:  // �õ���ǰƴд�����м�����Ŀ
			return DoGetKeyNum(hWnd,wParam,lParam);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}


// ********************************************************************
// ������static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	����0���������������ڣ�����-1������ƻ�����*
// ����������Ӧ�ó���������Ϣ
// ����: 
// ********************************************************************
static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSPELLAREA lpSpellArea; 
	LPCREATESTRUCT lpcs ;

		lpcs = (LPCREATESTRUCT) lParam;

		lpSpellArea = (LPSPELLAREA)malloc(sizeof(SPELLAREA)); // ������д���ṹ����
		if (lpSpellArea == NULL)
			return -1; // ���ش�������ʧ��
		
		lpcs = (LPCREATESTRUCT) lParam;

		lpSpellArea->hOwner = (HWND)lpcs->lpCreateParams;
		// ���ÿؼ���ɫ

//		lpSpellArea->cl_Bk = RGB(231,242,255);
		lpSpellArea->cl_Bk = RGB(240,255,240);
		lpSpellArea->cl_Text = RGB(0,0,0);

		lpSpellArea->cl_SelBk = RGB(161,205,255);
		lpSpellArea->cl_SelText = RGB(0,0,0);
		lpSpellArea->iCurSel = 0;
		lpSpellArea->iCurSpellPage = 0;

		lpSpellArea->SpellList.iKeyNum = 0;
		lpSpellArea->SpellList.iSpellNum = 0;

		SetWindowLong(hWnd,0,(LONG)lpSpellArea); // ���ýṹ������

		ClearSpellArea(hWnd);  // �����д��
		return 0;
}

// ********************************************************************
// ������static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	����0
// ����������Ӧ�ó������±���Ϣ
// ����: 
// ********************************************************************
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
#ifdef PLAN_1
	LPSPELLAREA lpSpellArea; 
	POINT point;
	int iIndex;

		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // �õ���д���ṹ
		ASSERT(lpSpellArea);
	    
		// �õ��ʵ�λ��
		point.x=(short)LOWORD(lParam);
		point.y=(short)HIWORD(lParam);

		iIndex = GetSpellIndex(hWnd,lpSpellArea,point);

		if (iIndex == -1)
		{
			// û�е㵽�κ���Ч��ƴд��Ŀ
			return 0;
		}
		SetCurrentSel(hWnd,lpSpellArea, iIndex,TRUE); // ���õ�ǰ��ѡ��
#endif
		return 0;
}


// ********************************************************************
// ������static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	����0
// ����������Ӧ�ó�����̧����Ϣ
// ����: 
// ********************************************************************
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSPELLAREA lpSpellArea; 

		if (GetCapture()!=hWnd)
			return 0; // û��ץס���
		SetCapture(0); // �ͷ�ץס���

		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // �õ���д���ṹ
		ASSERT(lpSpellArea);
	    
		return 0;
}

// ********************************************************************
// ������static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	����0
// ����������Ӧ�ó������ƶ�����Ϣ
// ����: 
// ********************************************************************
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSPELLAREA lpSpellArea; 

		if (GetCapture()!=hWnd) 
			return 0; // û��ץס���

		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // �õ���д���ṹ
		ASSERT(lpSpellArea);
	    
		return 0;
}


// **************************************************
// ������static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- LPCTLCOLORSTRUCT��Ҫ���õ���ɫֵ
// ����ֵ����
// �������������ô�����ɫ������WM_SETCTLCOLOR��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPCTLCOLORSTRUCT lpCtlColor;
	LPSPELLAREA lpSpellArea; 

		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // �õ���д���ṹ
		ASSERT(lpSpellArea);
		
		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // �õ���ɫ�ṹָ��

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 { // �����ı���ɫ
			 lpSpellArea->cl_Text = lpCtlColor->cl_Text;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // �����ı�����ɫ
			 lpSpellArea->cl_Bk = lpCtlColor->cl_TextBk;
		 }
		return TRUE;
}
// **************************************************
// ������static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- LPCTLCOLORSTRUCT����ŵõ�����ɫֵ
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ���ǰ���ڵ���ɫ������WM_GETCTLCOLOR��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		
	LPCTLCOLORSTRUCT lpCtlColor;
	LPSPELLAREA lpSpellArea; 

		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // �õ���д���ṹ
		ASSERT(lpSpellArea);

		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // �õ���ɫ�ṹָ��

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 { // �õ��ı���ɫ
			 lpCtlColor->cl_Text = lpSpellArea->cl_Text ;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // �õ��ı�����ɫ
				lpCtlColor->cl_TextBk = lpSpellArea->cl_Bk ;
		 }
		 return TRUE;
}

// ********************************************************************
// ������static void DoPaint(HWND hWnd,HDC hdc)
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//    IN hdc  - �豸���
// ����ֵ��
//	
// ����������Ӧ�ó����� WM_PAINT ��Ϣ
// ����: 
// ********************************************************************
static void DoPaint(HWND hWnd,HDC hdc)
{
	RECT rect;

	
		GetClientRect(hWnd,&rect);
		ShowSpellArea(hWnd,hdc);
}


// ********************************************************************
// ������static void ClearSpellArea(HWND hWnd)
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
// ����ֵ��
//	
// �������������ƴд��
// ����: 
// ********************************************************************
static void ClearSpellArea(HWND hWnd)
{
	LPSPELLAREA lpSpellArea; 
	RECT rect;
	HBRUSH hBrush;
	HDC hdc;

		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // �õ���д���ṹ
		ASSERT(lpSpellArea);

		GetClientRect(hWnd,&rect);  // �õ����ھ���
		
		hdc = GetDC(hWnd); // �õ��豸���

		hBrush = CreateSolidBrush(lpSpellArea->cl_Bk); //��������ˢ
		FillRect(hdc,&rect,hBrush); // �ñ���ɫ����ڴ�DC
		DeleteObject(hBrush); //ɾ����ˢ

		ReleaseDC(hWnd,hdc); // �ͷž��

}


// ********************************************************************
// ������static void DoInsertKey(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN wParam -- ����
//  IN lParam -- KEYENTRY * , Ҫ����ļ���ָ��
// ����ֵ��
//	
// ��������������һ����
// ����: 
// ********************************************************************
static void DoInsertKey(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSPELLAREA lpSpellArea; 
	KEYENTRY *pKey;
	TCHAR lpOldSelSpell[MAX_SPELLNUM];
#ifdef PLAN_1
	int iIndex;
	BOOL bShow = TRUE;
#endif

//		RETAILMSG(1,(TEXT(" Enter DoInsertKey\r\n")));
		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // �õ���д���ṹ
		ASSERT(lpSpellArea);

		pKey = (KEYENTRY *)lParam;
		
#ifdef PLAN_1
		if (lpSpellArea->SpellList.iKeyNum == 0)
		{  // ��û���κμ�,��������״̬,����������ʾ
			//ShowWindow(hWnd,SW_SHOW); // ��ʾƴд����
//			SetWindowPos(hWnd,NULL,0,0,0,0,SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOSIZE);  // ���ı䴰�ڴ��򣬲��ƶ�����
			bShow = FALSE;
		}
#endif

		if (lpSpellArea->SpellList.iKeyNum == 0)  
		{  // ԭ����û���κ��ַ�
			strcpy(lpOldSelSpell,""); // �õ�ԭ����ѡ��ƴд
		}
		else
		{
			strcpy(lpOldSelSpell,lpSpellArea->SpellList.lpSpellList[lpSpellArea->iCurSel]); // �õ�ԭ����ѡ��ƴд
		}

		if (InsertKeyToSpell(&lpSpellArea->SpellList,pKey)) // ��һ�������뵽ƴд�б�
		{			
#ifdef PLAN_1
			if (lpSpellArea->SpellList.iKeyNum && bShow == FALSE)
			{  // ��û���κμ�,��������״̬,����������ʾ
				SetWindowPos(hWnd,NULL,0,0,0,0,SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOSIZE);  // ���ı䴰�ڴ��򣬲��ƶ�����
			}
			lpSpellArea->iCurSel = -1 ; //����û��ѡ��
			iIndex = GetNewSelIndex(lpSpellArea,lpOldSelSpell);
			SetCurrentSel(hWnd,lpSpellArea, iIndex,FALSE); // ���õ�һ��Ϊ��ǰ��ѡ��
			AdjustWindowSize(hWnd,lpSpellArea); // ���ݵ�ǰ��ƴд��Ŀ���µ������ڴ�С
#endif
#ifdef PLAN_2
			HDC hdc;

			//�����ﲻ��Ҫ��ʾ����Ϊ�������ڴ�С����Ҫ������ʾ
			hdc = GetDC(hWnd);  // �õ��豸���
			ShowSpellArea(hWnd,hdc);  // ��ʾƴд��

			ReleaseDC(hWnd,hdc); // �ͷ��豸���

			PostMessage(lpSpellArea->hOwner,SPN_SELCHANGE,(WPARAM)0,0);
#endif
		}

//		RETAILMSG(1,(TEXT(" Leave DoInsertKey\r\n")));
}


// ********************************************************************
// ������static LRESULT DoGetSelSpell(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN wParam -- INT ����Ĵ�С
//  OUT lParam -- LPTSTR , �������ƴд�Ļ���
// ����ֵ��
//	 �ɹ�����TRUE�����򷵻�FLASE
// �����������õ���ǰѡ���ƴд��
// ����: 
// ********************************************************************
static LRESULT DoGetSelSpell(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSPELLAREA lpSpellArea; 
	LPTSTR pSpell;
	int iMax_Length;

		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // �õ���д���ṹ
		ASSERT(lpSpellArea);

		iMax_Length = (int)wParam;
		pSpell = (LPTSTR)lParam;

		if (lpSpellArea->iCurSel < 0 || lpSpellArea->iCurSel >= lpSpellArea->SpellList.iSpellNum)
		{
			// ��Ч������������
			return FALSE;
		}
		
		if ((int)strlen(lpSpellArea->SpellList.lpSpellList[lpSpellArea->iCurSel]) >= iMax_Length)
		{
			// �����С̫С
			return FALSE;
		}
		
		strcpy(pSpell,lpSpellArea->SpellList.lpSpellList[lpSpellArea->iCurSel]);
		return TRUE;
}

// ********************************************************************
// ������static LRESULT DoClearSpell(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN wParam -- ����
//  IN lParam -- ����
// ����ֵ��
//	 �ɹ�����TRUE�����򷵻�FLASE
// ����������������е�ƴд��
// ����: 
// ********************************************************************
static LRESULT DoClearSpell(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSPELLAREA lpSpellArea; 
//	LPTSTR pSpell;
//	int iMax_Length;

//		RETAILMSG(1,(TEXT(" Enter DoClearSpell\r\n")));
		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // �õ���д���ṹ
		ASSERT(lpSpellArea);

		lpSpellArea->SpellList.iKeyNum = 0; // ��ǰ�ļ�����ĿΪ0
		lpSpellArea->SpellList.iSpellNum = 0;  // ��ǰ��ƴд���Ϊ0
#ifdef PLAN_1
		AdjustWindowSize(hWnd,lpSpellArea);  // �������ڵĴ�С
#endif
#ifdef PLAN_2
		{
			HDC hdc;

			//�����ﲻ��Ҫ��ʾ����Ϊ�������ڴ�С����Ҫ������ʾ
			hdc = GetDC(hWnd);  // �õ��豸���
			ShowSpellArea(hWnd,hdc);  // ��ʾƴд��

			ReleaseDC(hWnd,hdc); // �ͷ��豸���

			if (lpSpellArea->SpellList.iSpellNum )
				PostMessage(lpSpellArea->hOwner,SPN_SELCHANGE,(WPARAM)0,0);
			else
				SendMessage(lpSpellArea->hOwner,SPN_NODATA,(WPARAM)0,0);
		}
#endif

//		RETAILMSG(1,(TEXT(" Leave DoClearSpell\r\n")));
		return 0;
}

// ********************************************************************
// ������static LRESULT DoDeleteKey(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN wParam -- ����
//  IN lParam -- ����
// ����ֵ��
//	 �ɹ�����TRUE�����򷵻�FLASE
// ����������ɾ��һ����
// ����: 
// ********************************************************************
static LRESULT DoDeleteKey(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSPELLAREA lpSpellArea; 
#ifdef PLAN_1
	TCHAR lpOldSelSpell[MAX_SPELLNUM];
	int iIndex;
#endif
//		RETAILMSG(1,(TEXT(" Enter DoDeleteKey\r\n")));

		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // �õ���д���ṹ
		ASSERT(lpSpellArea);


#ifdef PLAN_1
		if (lpSpellArea->SpellList.iKeyNum == 0)  
		{  // ԭ����û���κ��ַ�
// !!! modify by jami chen in 2004.11.03
//			strcpy(lpOldSelSpell,""); // �õ�ԭ����ѡ��ƴд
			AdjustWindowSize(hWnd,lpSpellArea); //���µ������ڴ�С
			return 0;
// !!! modify End by jami chen in 2004.11.03
		}
		else
		{
			strcpy(lpOldSelSpell,lpSpellArea->SpellList.lpSpellList[lpSpellArea->iCurSel]); // �õ�ԭ����ѡ��ƴд
		}
#endif
#ifdef PLAN_2
		if (lpSpellArea->SpellList.iKeyNum == 0)  
		{  // ԭ����û���κ��ַ�
			SendMessage(lpSpellArea->hOwner,SPN_NODATA,(WPARAM)0,0);
			return 0;
		}
#endif
		lpSpellArea->SpellList.iKeyNum --; // ����һ����
		lpSpellArea->SpellList.pKey[lpSpellArea->SpellList.iKeyNum] = NULL; // ������һ����

		if (lpSpellArea->SpellList.iKeyNum)
		{  // ���м����ڣ���Ҫ����ƴд
			ReSpellList(&lpSpellArea->SpellList);

#ifdef PLAN_1
			iIndex = GetNewSelIndex(lpSpellArea,lpOldSelSpell);
			SetCurrentSel(hWnd,lpSpellArea, iIndex,FALSE); // ���õ�һ��Ϊ��ǰ��ѡ��
#endif
		}
		else
		{
			// �Ѿ�û�м���
			lpSpellArea->SpellList.iSpellNum = 0;  // ���ƴд��
		}
		
#ifdef PLAN_1
		AdjustWindowSize(hWnd,lpSpellArea); //���µ������ڴ�С
#endif
#ifdef PLAN_2
		{
			HDC hdc;

			//�����ﲻ��Ҫ��ʾ����Ϊ�������ڴ�С����Ҫ������ʾ
			hdc = GetDC(hWnd);  // �õ��豸���
			ShowSpellArea(hWnd,hdc);  // ��ʾƴд��

			ReleaseDC(hWnd,hdc); // �ͷ��豸���

//			PostMessage(lpSpellArea->hOwner,SPN_SELCHANGE,(WPARAM)0,0);
			if (lpSpellArea->SpellList.iSpellNum )
				PostMessage(lpSpellArea->hOwner,SPN_SELCHANGE,(WPARAM)0,0);
			else
				SendMessage(lpSpellArea->hOwner,SPN_NODATA,(WPARAM)0,0);
		}
#endif
//		RETAILMSG(1,(TEXT(" Leave DoDeleteKey\r\n")));
		return 0;
}
		
// ********************************************************************
// ������static LRESULT DoGetKeyNum(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN wParam -- ����
//  IN lParam -- ����
// ����ֵ��
//	 �ɹ����ص�ǰƴд�����м�����Ŀ�����򷵻�-1
// �����������õ���ǰƴд�����м�����Ŀ
// ����: 
// ********************************************************************
static LRESULT DoGetKeyNum(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSPELLAREA lpSpellArea; 

		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // �õ���д���ṹ
		ASSERT(lpSpellArea);

		return lpSpellArea->SpellList.iKeyNum;
}

#ifdef PLAN_1

// ********************************************************************
// ������static void SetCurrentSel(HWND hWnd,LPSPELLAREA lpSpellArea, int iIndex,BOOL bShow)
// ������
//	IN hWnd -- Ӧ�ó���Ĵ��ھ��
//  IN lpSpellArea -- ��ǰ��ƴд�ṹ
//  IN iIndex -- Ҫ����ǰѡ�����õ���ָ������
// ����ֵ��
//		��
// �������������õ�ǰ��ѡ��
// ����: 
// ********************************************************************
static void SetCurrentSel(HWND hWnd,LPSPELLAREA lpSpellArea, int iIndex,BOOL bShow)
{
//	HWND hParent;
	int iOldSel;
	HDC hdc;

		if (iIndex < 0 || iIndex >= lpSpellArea->SpellList.iSpellNum)
		{
			// ��Ч������������
			return;
		}
		iOldSel = lpSpellArea->iCurSel; // �õ�ԭ����ѡ��
		lpSpellArea->iCurSel = iIndex; // ���õ�ǰ��ѡ��

		if (bShow)
		{
			// ��ʾ��ǰ��ѡ����Ŀ
			hdc = GetDC(hWnd);  //�õ��豸���
			if (iOldSel != -1)
				ShowSpellItem(hWnd,hdc,lpSpellArea, iOldSel); // �ָ�ԭ����ѡ����ʾ
			ShowSpellItem(hWnd,hdc,lpSpellArea, lpSpellArea->iCurSel); // ��ʾ��ǰ��ѡ��
			ReleaseDC(hWnd,hdc);  // �ͷ��豸���
		}

//		hParent = GetParent(hWnd); // �õ������ھ��
		// ֪ͨ�����ڵ�ǰ��ѡ�����ı�
//		PostMessage(hParent,SPN_SELCHANGE,(WPARAM)iIndex,0);

		PostMessage(lpSpellArea->hOwner,SPN_SELCHANGE,(WPARAM)iIndex,0);
}


// ********************************************************************
// ������static void AdjustWindowSize(HWND hWnd,LPSPELLAREA lpSpellArea)
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN lpSpellArea -- ��ǰ��ƴд�ṹ
// ����ֵ��
//		��
// �������������ݵ�ǰ��ƴд��Ŀ���µ������ڴ�С
// ����: 
// ˵������Ŀ����ʾ����Ϊ
//		| (2 point) ITEM1 (2 point) ITEM2 ... ITEMn (2 point) |
//      |<--  Item 1 Area  (1) | (1) Item 2 ... Item n ---->  |
// ********************************************************************
static void AdjustWindowSize(HWND hWnd,LPSPELLAREA lpSpellArea)
{
	int iWindowWidth = 0;
	int iItemWidth = 0, iWindowHeight;
	RECT rect;
//	HWND hParent;

//		RETAILMSG(1,(TEXT(" Enter AdjustWindowSize\r\n")));
		iItemWidth = GetItemWidth(hWnd,lpSpellArea); // �õ���һ����Ŀ�Ŀ�ȣ�����Ѿ�û���κεļ����򷵻�0
//		RETAILMSG(1,(TEXT(" 1\r\n")));

		if (iItemWidth == 0)
		{
			// �Ѿ�û���κεļ������ش���
//			RETAILMSG(1,(TEXT(" 2\r\n")));
			ShowWindow(hWnd,SW_HIDE);

//			RETAILMSG(1,(TEXT(" 3\r\n")));
			//hParent = GetParent(hWnd); 
			// ֪ͨ����������Ѿ�û��������
//			RETAILMSG(1,(TEXT(" 4 = %x\r\n"),hParent));
			//SendMessage(hParent,SPN_NODATA,0,0);
//			RETAILMSG(1,(TEXT(" Leave AdjustWindowSize\r\n")));
			SendMessage(lpSpellArea->hOwner,SPN_NODATA,0,0);
			return ;
		}

//		RETAILMSG(1,(TEXT(" 6\r\n")));
		GetWindowRect(hWnd,&rect);  // �õ����ھ���
//		RETAILMSG(1,(TEXT(" 7\r\n")));

		iWindowHeight = rect.bottom - rect.top;  // ���ڸ߶Ȳ���

		iWindowWidth = (iItemWidth + SPACE_WIDTH ) * lpSpellArea->SpellList.iSpellNum + SPACE_WIDTH; // һ���� n +1 ����� 

		iWindowWidth += 2; // ���ڱ߿�

//		RETAILMSG(1,(TEXT(" 8\r\n")));
		SetWindowPos(hWnd,NULL,0,0,iWindowWidth,iWindowHeight,SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);  // ���ı䴰�ڴ��򣬲��ƶ�����
//		RETAILMSG(1,(TEXT(" Leave AdjustWindowSize\r\n")));
}

// **************************************************
// ������static UINT GetStringWidth(HWND hWnd,LPTSTR lpString)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpString -- ָ���ִ�
// 
// ����ֵ�������ִ��Ŀ��
// �����������õ��ִ��Ŀ�ȡ�
// ����: 
// **************************************************
static UINT GetStringWidth(HWND hWnd,LPTSTR lpString)
{
	HDC hdc;
    int lpASCIICodeWidthBuffer[128];
    int ChineseCharWidth;
	UINT iWidth = 0;

		if (lpString == NULL)
			return 0;
		hdc = GetDC(hWnd); // �õ��豸���
		if (GetCharWidth(hdc,0,0x7f,lpASCIICodeWidthBuffer)==0) // �õ�ASCII�ַ��Ŀ��
			goto GETSTRINGWIDTH_END;
		// get chinese character width
		if (GetCharWidth(hdc,0xb0a1,0xb0a1,&ChineseCharWidth)==0) // �õ����ֵĿ��
			goto GETSTRINGWIDTH_END;

		while(*lpString)
		{ // ����ÿһ���ַ��Ŀ�Ȳ����
			if (*lpString<0 || *lpString > 0x7f)
			{  // current Character is a chinese
				iWidth += ChineseCharWidth;
				lpString+=2;
			}
			else
			{  // current character is english or sambol
				iWidth +=lpASCIICodeWidthBuffer[*lpString];;
				lpString ++;
			}
		}
GETSTRINGWIDTH_END:
		ReleaseDC(hWnd,hdc); // �ͷ�DC
		return iWidth; // ���ؿ��
}

// ********************************************************************
// ������static void ShowSpellArea(HWND hWnd,HDC hdc)
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN hdc -- �豸���
// ����ֵ��
//	
// ������������ʾƴд��.
// ����: 
// ˵������Ŀ����ʾ����Ϊ
//		| (2 point) ITEM1 (2 point) ITEM2 ... ITEMn (2 point) |
//      |<--  Item 1 Area  (1) | (1) Item 2 ... Item n ---->  |
// ********************************************************************
static void ShowSpellArea(HWND hWnd,HDC hdc)
{
	LPSPELLAREA lpSpellArea; 
	RECT rect;
	int i;
	HBRUSH hBrush;

		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // �õ���д���ṹ
		ASSERT(lpSpellArea);

		GetClientRect(hWnd,&rect);  // �õ����ھ���

		// �������
		hBrush = CreateSolidBrush(lpSpellArea->cl_Bk);
		FillRect(hdc,&rect,hBrush);
		DeleteObject(hBrush);
		
		// ��ʼ������Ŀ
		for( i = 0; i < lpSpellArea->SpellList.iSpellNum ; i++)
		{
			ShowSpellItem(hWnd,hdc,lpSpellArea, i);
		}
}

// ********************************************************************
// ������static void ShowSpellItem(HWND hWnd,HDC hdc,LPSPELLAREA lpSpellArea)
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN hdc -- �豸���
//  IN lpSpellArea  -- ƴд�ṹ
//	IN iIndex  -- ��ǰҪ��ʾ����Ŀ����
// ����ֵ��
//	
// ������������ʾƴд��Ŀ.
// ����: 
// ********************************************************************
static void ShowSpellItem(HWND hWnd,HDC hdc,LPSPELLAREA lpSpellArea, int iIndex)
{
	RECT rect;
	int iItemWidth; 

		
		if (iIndex == lpSpellArea->iCurSel)
		{
			// ��ѡ����Ŀ
			SetTextColor(hdc,lpSpellArea->cl_SelText);
			SetBkColor(hdc,lpSpellArea->cl_SelBk);
		}
		else
		{
			// ����ѡ����Ŀ
			SetTextColor(hdc,lpSpellArea->cl_Text);
			SetBkColor(hdc,lpSpellArea->cl_Bk);
		}

		GetClientRect(hWnd,&rect);  // �õ����ھ���
		rect.left += SPACE_WIDTH ; // �ȿճ���ǰ��Ŀհ�

		iItemWidth = GetItemWidth(hWnd,lpSpellArea); // �õ���Ŀ�Ŀ��

		rect.left += iIndex  * (iItemWidth + SPACE_WIDTH);
		rect.right = rect.left + iItemWidth; // �õ���Ŀ���ұ߽�
		DrawText(hdc,lpSpellArea->SpellList.lpSpellList[iIndex],lpSpellArea->SpellList.iKeyNum,&rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE); // ������Ŀ�ı�

}
// ********************************************************************
// ������static int GetSpellIndex(HWND hWnd,LPSPELLAREA lpSpellArea,POINT point)
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN lpSpellArea -- ��ǰ��ƴд�ṹ
//  IN point -- ��ǰ�±ʵĵ�
// ����ֵ��
//		��
// �����������õ���ǰ�±ʵ�ƴд����
// ����: 
// ˵������Ŀ����ʾ����Ϊ
//		| (2 point) ITEM1 (2 point) ITEM2 ... ITEMn (2 point) |
//      |<--  Item 1 Area  (1) | (1) Item 2 ... Item n ---->  |
// ********************************************************************
static int GetSpellIndex(HWND hWnd,LPSPELLAREA lpSpellArea,POINT point)
{
	int i;
	int iItemWidth,iCurPos = 0; 


		iCurPos += SPACE_WIDTH ; // �ȼ�����ǰ��Ŀհ�

		iItemWidth = GetItemWidth(hWnd,lpSpellArea); // �õ���һ����Ŀ�Ŀ��
		
		// ��ʼ������Ŀ
		for( i = 0; i < lpSpellArea->SpellList.iSpellNum ; i++)
		{
			iCurPos += iItemWidth + SPACE_WIDTH; // �õ���ǰ��Ŀ���ұ߽�
			if (point.x < iCurPos)
			{
				// �±��ڵ�ǰ��Ŀ
				return i;
			}
		}
		return -1; // û���±����κ���Ŀ
}


// ********************************************************************
// ������static int GetItemWidth(HWND hWnd,LPSPELLAREA lpSpellArea)
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN lpSpellArea -- ��ǰ��ƴд�ṹ
// ����ֵ��
//		��
// �����������õ���ǰ��Ŀ�Ŀ��
// ����: 
// ********************************************************************
static int GetItemWidth(HWND hWnd,LPSPELLAREA lpSpellArea)
{
	int iItemWidth = 0;

		if (lpSpellArea->SpellList.iKeyNum == 0)		
		{
			// ��ǰ��ƴд��û���κ��ַ������ش���
			return 0; // ����
		}
		if (lpSpellArea->SpellList.iSpellNum == 0)		
		{
			// �����ǰ�м�����û��ƴд���������������������ݣ������ش���
			lpSpellArea->SpellList.iKeyNum = 0;
			return 0; // ����
		}

		iItemWidth = GetStringWidth(hWnd,lpSpellArea->SpellList.lpSpellList[0]); // �õ���һ����Ŀ�Ŀ��
		return iItemWidth;
}

// ********************************************************************
// ������static int GetNewSelIndex(LPSPELLAREA lpSpellArea,LPTSTR lpOldSelSpell)
// ������
//  IN lpSpellArea -- ��ǰ��ƴд�ṹ
//  IN lpOldSelSpell -- ԭ����ѡ��ƴд
// ����ֵ��
//		��
// �����������õ���ǰ���µ�ѡ��Ҫ���ҵ�һ����ԭ��ѡ������Ƶĵ�һ��ѡ��
// ����: 
// ********************************************************************
static int GetNewSelIndex(LPSPELLAREA lpSpellArea,LPTSTR lpOldSelSpell)
{
	int iCmplen ;
	int i;

		iCmplen = strlen(lpOldSelSpell); //�õ�ԭ��ƴд�ĳ���

		if (iCmplen == 0)
			return 0; // ԭ��û��ƴд�����õ�ǰѡ��Ϊ0

		if (iCmplen > lpSpellArea->SpellList.iKeyNum)
			iCmplen = lpSpellArea->SpellList.iKeyNum;  //��ǰƴд�ĳ��ȱ�ԭ����С

		for (i = 0; i < lpSpellArea->SpellList.iSpellNum ; i++)
		{
			if (strncmp(lpOldSelSpell,lpSpellArea->SpellList.lpSpellList[i],iCmplen) == 0) //��ԭ����ѡ��ƴд�Ƚ�
			{
				// ��ԭ����ѡ������ͬ���ַ���ʹ�õ�ǰ����Ϊ��ǰѡ��
				return i;
			}
		}

		return 0; //û���ҵ�ƥ����ִ������õ�ǰѡ��Ϊ0
}
#endif

#ifdef PLAN_2
// ********************************************************************
// ������static void ShowSpellArea(HWND hWnd,HDC hdc)
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN hdc -- �豸���
// ����ֵ��
//	
// ������������ʾƴд��.
// ����: 
// ˵������Ŀ����ʾ����Ϊ
//		| (2 point) ITEM1 (2 point) ITEM2 ... ITEMn (2 point) |
//      |<--  Item 1 Area  (1) | (1) Item 2 ... Item n ---->  |
// ********************************************************************
static void ShowSpellArea(HWND hWnd,HDC hdc)
{
	LPSPELLAREA lpSpellArea; 
	RECT rect;
//	int i;
	HBRUSH hBrush;

		lpSpellArea = (LPSPELLAREA)GetWindowLong(hWnd,0); // �õ���д���ṹ
		ASSERT(lpSpellArea);

		GetClientRect(hWnd,&rect);  // �õ����ھ���

		// �������
		hBrush = CreateSolidBrush(lpSpellArea->cl_Bk);
		FillRect(hdc,&rect,hBrush);
		DeleteObject(hBrush);
		
		if (lpSpellArea->SpellList.iSpellNum)
		{
			// ��ʼ������Ŀ
			SetTextColor(hdc,lpSpellArea->cl_Text);
			SetBkColor(hdc,lpSpellArea->cl_Bk);
			DrawText(hdc,lpSpellArea->SpellList.lpSpellList[0],lpSpellArea->SpellList.iKeyNum,&rect,DT_VCENTER | DT_SINGLELINE); // ������Ŀ�ı�
		}
}
#endif
