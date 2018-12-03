/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/

/**************************************************
�ļ�˵����SMIL���������ı�
�汾�ţ�1.0.0.456
����ʱ�ڣ�2004-04-30
���ߣ��½��� JAMI
�޸ļ�¼��
**************************************************/
#include "esmilbrowser.h"
#include "smilstruct.h"
//#include "DebugWnd.h"
#include "parsesmil.h"
#include "smiltag.h"


// **************************************************
// ��������
// **************************************************
#define MAX_ATTRCONTENTLEN	512

#define DEFAULT_DELAY	3000 // Ĭ�ϵĻõ�Ƭ����ʾʱ��
// **************************************************
// ������������
// **************************************************
static LPHEAD ParseHead(LPTSTR *lppPtr);
static LPBODY ParseBody(LPTSTR *lppPtr);
static LPLAYOUT ParseLayout(LPTSTR *lppPtr);
static LPROOTLAYOUT ParseRootLayout(LPTSTR *lppPtr);
static LPREGION ParseRegion(LPTSTR *lppPtr);
static LPPAR ParsePar(LPTSTR *lppPtr);
static LPIMG ParseImg(LPTSTR *lppPtr);
static LPAUDIO ParseAudio(LPTSTR *lppPtr);
static LPTEXT ParseText(LPTSTR *lppPtr);
static LPREF ParseRef(LPTSTR *lppPtr);

static UINT GetPixels(LPTSTR lpAttrContent);
static UINT GetDurTime(LPTSTR lpAttrContent);
static void AdjustTheParTime(LPPAR lpPar);

LPTSTR SMILBufferAssign(const LPTSTR pSrc);

// **************************************************


// **************************************************
// ������LPSMIL ParseSMIL(LPTSTR lpContent)
// ������
// 	IN lpContent -- Ҫ�������ı�����
// 
// ����ֵ�����ط������SMILָ��
// ��������������SMIL���ļ����ݡ�
// ����: 
// **************************************************
LPSMIL ParseSMIL(LPTSTR lpContent)
{
	LPSMIL lpSmil = NULL; // SMIL �ṹָ�룬���浱ǰ��SMIL����
	LPTSTR lpPtr; // ���ڱ��浱ǰ�Ѿ�������ָ��λ��
	TAGID tagID; // ��ǩ���
	LPHEAD lpHead; // ����ͷ��Ϣ
	LPBODY lpBody; // ����BODY��Ϣ
		
		RETAILMSG(1,(TEXT("+ParseText")));

		lpSmil = (LPSMIL)malloc(sizeof(SMIL)); // ����SMIL�ṹ�Ļ���

		if (lpSmil == NULL)
		{ // ���仺��ʧ��
			return NULL; // ����ʧ��
		}
		
		lpPtr = lpContent ;// ��ʼ������ʼλ��

		// ��ʼ��������
		while(1)
		{
			tagID=SMIL_LocateTag(&lpPtr);  // �õ���ǩ���

			if(tagID == SMILTAG_HEAD)
			{
				// ��ǰ��<HEAD>��ǩ
				SMIL_ToNextTag(&lpPtr); // ����һ����ǩ��λ�ã���<HEAD> </HEAD>֮��ı�ǩ
				lpHead = ParseHead(&lpPtr); // ����ͷ��Ϣ
				lpSmil->lpHead = lpHead; // ����ǰͷ��Ϣ��ӵ�SMIL��
			}
			if(tagID == SMILTAG_BODY)
			{
				// ��ǰ��<BODY>��ǩ
				SMIL_ToNextTag(&lpPtr); // ����һ����ǩ��λ�ã���<BODY> </BODY>֮��ı�ǩ
				lpBody = ParseBody(&lpPtr); // ����ͷ��Ϣ
				lpSmil->lpBody = lpBody; // ����ǰbody��Ϣ��ӵ�SMIL��
			}
			if(tagID == SMILTAG_SMIL + SMILTAG_END)
			{
				// ��ǰ��</SMIL>��ǩ
				break; // �Ѿ�����
			}
			if(tagID == SMILTAG_NULL)
			{
				// �Ѿ�û��������
				break;
			}
			SMIL_ToNextTag(&lpPtr);
		}
	
		RETAILMSG(1,(TEXT("-ParseText")));
		return lpSmil;
}

// **************************************************
// ������static LPHEAD ParseHead(LPTSTR *lppPtr)
// ������
// 	IN/OUT lppPtr -- ָ��ǰҪ�������ı����ݣ����������µ�ָ��
// 
// ����ֵ�����ط������HEADָ��
// �������� ����<HEAD>�����ݡ�
// ����: 
// **************************************************
static LPHEAD ParseHead(LPTSTR *lppPtr)
{
	LPHEAD lpHead; // ����ͷ��Ϣ
	LPLAYOUT lpLayout; // ����LAYOUT��Ϣ
	TAGID tagID; // ��ǰ��ǩ�ı��
	
		RETAILMSG(1,(TEXT("+ParseHead\r\n")));
		lpHead = (LPHEAD)malloc(sizeof(HEAD)); //  ����HEAD�ṹָ��
		if (lpHead == NULL)
		{
			// �����ڴ�ʧ��
			RETAILMSG(1,(TEXT("!!!ERROR MALLOC FAILURE IN [ParseHead]\r\n")));
			return NULL;
		}

		// ��ʼ����HEAD����
		// HEAD �����ݰ��� META�� LAYOUT�� ROOT-LAYOUT
		while(1)
		{
			tagID=SMIL_LocateTag(lppPtr);  // �õ���ǩ���

			if(tagID == SMILTAG_NULL)
			{
				// �Ѿ�û��������
				RETAILMSG(1,(TEXT("!!!ERROR IN <HEAD> TAG ,CAN'T OCCUR NULL TAG\r\n")));
				break;
			}
			if(tagID == SMILTAG_HEAD + SMILTAG_END)
			{
				// ��ǰ��</HEAD>��ǩ
				break; // HEAD �������Ѿ�����
			}
			if(tagID == SMILTAG_META)
			{
				// ��ǰ��<META>��ǩ
			}
			if(tagID == SMILTAG_LAYOUT)
			{
				// ��ǰ��<LAYOUT>��ǩ
				SMIL_ToNextTag(lppPtr); // ����һ����ǩ��λ�ã���<LAYOUT> </LAYOUT>֮��ı�ǩ
				lpLayout = ParseLayout(lppPtr); // ����LAYOUT������
				lpHead ->lpLayout = lpLayout;  // ����LAYOUT�Ľṹ��HEAD
			}
			if(tagID == SMILTAG_ROOTLAYOUT)
			{
				// ��ǰ��<ROOT-LAYOUT>��ǩ
			}
			SMIL_ToNextTag(lppPtr);
		}

		RETAILMSG(1,(TEXT("-ParseHead\r\n")));
		return lpHead;
}


// **************************************************
// ������static LPBODY ParseBody(LPTSTR *lppPtr)
// ������
// 	IN/OUT lppPtr -- ָ��ǰҪ�������ı����ݣ����������µ�ָ��
// 
// ����ֵ�����ط������BODYָ��
// �������� ����<BODY>�����ݡ�
// ����: 
// **************************************************
static LPBODY ParseBody(LPTSTR *lppPtr)
{
	LPBODY lpBody; // ����ͷ��Ϣ
	LPPAR lpPar; // ���PAR����Ϣ
	TAGID tagID; // ��ǰ��ǩ�ı��
	
		RETAILMSG(1,(TEXT("+ParseBody\r\n")));
		lpBody = (LPBODY)malloc(sizeof(BODY)); //  ����BODY�ṹָ��
		if (lpBody == NULL)
		{
			// �����ڴ�ʧ��
			RETAILMSG(1,(TEXT("!!!ERROR MALLOC FAILURE IN [ParseBody]\r\n")));
			return NULL;
		}
		// ��ʼ��lpBody�ṹ
		lpBody->lpPar = NULL;

		// ��ʼ����BODY����
		// BODY �����ݰ��� PAR
		while(1)
		{
			tagID=SMIL_LocateTag(lppPtr);  // �õ���ǩ���

			if(tagID == SMILTAG_NULL)
			{
				// �Ѿ�û��������
				RETAILMSG(1,(TEXT("!!!ERROR IN <BODY> TAG ,CAN'T OCCUR NULL TAG\r\n")));
				break;
			}
			if(tagID == SMILTAG_BODY + SMILTAG_END)
			{
				// ��ǰ��</BODY>��ǩ
				break; // BODY �������Ѿ�����
			}
			if(tagID == SMILTAG_PAR)
			{
				// ��ǰ��<PAR>��ǩ
				lpPar = ParsePar(lppPtr); // ����һ���õ�Ƭ
				if (lpPar != NULL) 
				{
					// ��lpPar ���뵽lpBody��
					if (lpBody->lpPar)
					{
						LPPAR lpCurrent; // ���PAR����Ϣ
						lpCurrent = lpBody->lpPar; // �õ���һ���õ�Ƭ
						while(lpCurrent) // ��ǰ�õ�Ƭ��Ч
						{
							if (lpCurrent->next == NULL)
							{
								lpCurrent->next = lpPar; // �Ѿ�û�лõ�Ƭ������ǰ�����õ��Ļõ�Ƭ���뵽����
								break;
							}
							else
							{
								lpCurrent = lpCurrent->next; // ָ����һ���õ�Ƭ
							}
						}
					}
					else
					{
						lpBody->lpPar = lpPar;
					}
				}
			}
			SMIL_ToNextTag(lppPtr);
		}

		RETAILMSG(1,(TEXT("-ParseBody\r\n")));
		return lpBody;
}


// **************************************************
// ������static LPLAYOUT ParseLayout(LPTSTR *lppPtr)
// ������
// 	IN/OUT lppPtr -- ָ��ǰҪ�������ı����ݣ����������µ�ָ��
// 
// ����ֵ�����ط������LAYOUTָ��
// �������� ����<LAYOUT>�����ݡ�
// ����: 
// **************************************************
static LPLAYOUT ParseLayout(LPTSTR *lppPtr)
{
	LPLAYOUT lpLayout; // ����LAYOUT��Ϣ
	LPREGION lpRegion; // ����REGION��Ϣ
	LPROOTLAYOUT lpRootLayout; // ����ROOT-LAYOUT��Ϣ
	TAGID tagID; // ��ǰ��ǩ�ı��
	
		RETAILMSG(1,(TEXT("+ParseLayout\r\n")));
		lpLayout = (LPLAYOUT)malloc(sizeof(LAYOUT)); //  ����LAYOUT�ṹָ��
		if (lpLayout == NULL)
		{
			// �����ڴ�ʧ��
			RETAILMSG(1,(TEXT("!!!ERROR MALLOC FAILURE IN [ParseLayout]\r\n")));
			return NULL;
		}
		// ��ʼ��lpLayout�ṹ
		lpLayout->lpRegion = NULL;

		// ��ʼ����LAYOUT����
		// LAYOUT �����ݰ��� REGION �� ROOT-LAYOUT
		while(1)
		{
			tagID=SMIL_LocateTag(lppPtr);  // �õ���ǩ���

			if(tagID == SMILTAG_NULL)
			{
				// �Ѿ�û��������
				RETAILMSG(1,(TEXT("!!!ERROR IN <LAYOUT> TAG ,CAN'T OCCUR NULL TAG\r\n")));
				break;
			}
			if(tagID == SMILTAG_LAYOUT + SMILTAG_END)
			{
				// ��ǰ��</LAYOUT>��ǩ
				break; // LAYOUT �������Ѿ�����
			}
			else if(tagID == SMILTAG_ROOTLAYOUT)
			{
				// ��ǰ��<ROOT-LAYOUT>��ǩ
				lpRootLayout = ParseRootLayout(lppPtr); // ����ROOT-LAYOUT������

				// ��lpRegion ���뵽lpLayout�ĵ�һ��
				lpLayout->lpRootLayout = lpRootLayout;
				SMIL_ToNextTag(lppPtr); // ����һ����ǩ��λ��
			}
			else if(tagID == SMILTAG_REGION)
			{
				// ��ǰ��<REGION>��ǩ
				lpRegion = ParseRegion(lppPtr); // ����REGION������

				// ��lpRegion ���뵽lpLayout�ĵ�һ��
				lpRegion->next = lpLayout->lpRegion;
				lpLayout->lpRegion = lpRegion;
				SMIL_ToNextTag(lppPtr); // ����һ����ǩ��λ��
			}
			else
			{ // ��ǰ��ǩ����Ч��ǩ
				SMIL_ToNextTag(lppPtr); // ����һ����ǩ��λ��
			}
		}

		RETAILMSG(1,(TEXT("-ParseLayout\r\n")));
		return lpLayout;
}

// **************************************************
// ������static LPROOTLAYOUT ParseRootLayout(LPTSTR *lppPtr)
// ������
// 	IN/OUT lppPtr -- ָ��ǰҪ�������ı����ݣ����������µ�ָ��
// 
// ����ֵ�����ط������ROOT-LAYOUTָ��
// �������� ����ROOT-LAYOUT�����ݡ�
// ����: 
// **************************************************
static LPROOTLAYOUT ParseRootLayout(LPTSTR *lppPtr)
{
	LPROOTLAYOUT lpRootLayout;
	ATTRID idAttr;
	TCHAR lpAttrContent[MAX_ATTRCONTENTLEN];

		RETAILMSG(1,(TEXT("+ParseROOT-LAYOUT\r\n")));
		lpRootLayout = (LPROOTLAYOUT)malloc(sizeof(ROOTLAYOUT)); // ����REGION�ṹ���ڴ�
		if (lpRootLayout == NULL)
		{
			// �����ڴ�ʧ��
			return NULL;
		}
		// ��ʼ��LPROOTLAYOUT �ṹ
		lpRootLayout->width = SMIL_DEFAILTWIDTH; // ��ǰSMIL�Ŀ�ȣ��Ե�Ϊ��λ
		lpRootLayout->height = SMIL_DEFAILTHEIGHT; // ��ǰSMIL�ĸ߶ȣ��Ե�Ϊ��λ��

		// ��ʼ���� ROOT-LAYOUT �� ATTRIBUTE
		// ROOT-LAYOUT �� ATTRIBUTE ���� width,height
		while(1)
		{
			idAttr = SMIL_LocateAttribute(lppPtr); // �õ���ǩ��һ������
			if (idAttr == 0)
			{ // ��ǩ����
				break;
			}
			switch(idAttr)
			{
				case SMILATTR_WIDTH:
					RETAILMSG(1,(TEXT("current ATTR is \"WIDTH\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpRootLayout->width = GetPixels(lpAttrContent);
					break;
				case SMILATTR_HEIGHT:
					RETAILMSG(1,(TEXT("current ATTR is \"HEIGHT\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpRootLayout->height = GetPixels(lpAttrContent);
					break;
				default:
					RETAILMSG(1,(TEXT("current ID<%x> is Invalid ID \r\n"),idAttr));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					break;
			}
		}

		RETAILMSG(1,(TEXT("-ParseROOT-LAYOUT\r\n")));
		return lpRootLayout;
}

// **************************************************
// ������static LPREGION ParseRegion(LPTSTR *lppPtr)
// ������
// 	IN/OUT lppPtr -- ָ��ǰҪ�������ı����ݣ����������µ�ָ��
// 
// ����ֵ�����ط������REGIONָ��
// �������� ����REGION�����ݡ�
// ����: 
// **************************************************
static LPREGION ParseRegion(LPTSTR *lppPtr)
{
	LPREGION lpRegion;
	ATTRID idAttr;
	TCHAR lpAttrContent[MAX_ATTRCONTENTLEN];

		RETAILMSG(1,(TEXT("+ParseRegion\r\n")));
		lpRegion = (LPREGION)malloc(sizeof(REGION)); // ����REGION�ṹ���ڴ�
		if (lpRegion == NULL)
		{
			// �����ڴ�ʧ��
			return NULL;
		}
		// ��ʼ��REGION �ṹ
		lpRegion->id = NULL;
		lpRegion->top = 0; // ��ǰregion������ľ��룬�Ե�Ϊ��λ
		lpRegion->left = 0; // ��ǰregion������ߵľ��룬�Ե�Ϊ��λ
		lpRegion->width = 0; // ��ǰregion�Ŀ�ȣ��Ե�Ϊ��λ
		lpRegion->height = 0; // ��ǰregion �ĸ߶ȣ��Ե�Ϊ��λ��
		lpRegion->fit = 0;
		lpRegion->fill = 0;
		lpRegion->hidden = 0;
		lpRegion->meet = 0;
		lpRegion->slice = 0;
		lpRegion->next = NULL; 

		// ��ʼ���� LAYOUT �� ATTRIBUTE
		// LAYOUT �� ATTRIBUTE ���� id,top,left,width,height,fit,fill,hidden, meet, slice
		while(1)
		{
			idAttr = SMIL_LocateAttribute(lppPtr); // �õ���ǩ��һ������
			if (idAttr == 0)
			{ // ��ǩ����
				break;
			}
			switch(idAttr)
			{
				case SMILATTR_ID:
					RETAILMSG(1,(TEXT("current ATTR is \"ID\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpRegion->id = SMILBufferAssign(lpAttrContent);
					break;
				case SMILATTR_TOP:
					RETAILMSG(1,(TEXT("current ATTR is \"TOP\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpRegion->top = GetPixels(lpAttrContent);
					break;
				case SMILATTR_LEFT:
					RETAILMSG(1,(TEXT("current ATTR is \"LEFT\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpRegion->left = GetPixels(lpAttrContent);
					break;
				case SMILATTR_WIDTH:
					RETAILMSG(1,(TEXT("current ATTR is \"WIDTH\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpRegion->width = GetPixels(lpAttrContent);
					break;
				case SMILATTR_HEIGHT:
					RETAILMSG(1,(TEXT("current ATTR is \"HEIGHT\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpRegion->height = GetPixels(lpAttrContent);
					break;
				default:
					RETAILMSG(1,(TEXT("current ID<%x> is Invalid ID \r\n"),idAttr));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					break;
			}
		}

		RETAILMSG(1,(TEXT("-ParseRegion\r\n")));
		return lpRegion;

}



// **************************************************
// ������static LPPAR ParsePar(LPTSTR *lppPtr)
// ������
// 	IN/OUT lppPtr -- ָ��ǰҪ�������ı����ݣ����������µ�ָ��
// 
// ����ֵ�����ط������PARָ��
// �������� ����<PAR>�����ݡ�
// ����: 
// **************************************************
static LPPAR ParsePar(LPTSTR *lppPtr)
{
	LPPAR lpPar; // ����PAR��Ϣ
	TAGID tagID; // ��ǰ��ǩ�ı��

	ATTRID idAttr;
	TCHAR lpAttrContent[MAX_ATTRCONTENTLEN];
	
		RETAILMSG(1,(TEXT("+ParsePar\r\n")));
		lpPar = (LPPAR)malloc(sizeof(PAR)); //  ����PAR�ṹָ��
		if (lpPar == NULL)
		{
			// �����ڴ�ʧ��
			RETAILMSG(1,(TEXT("!!!ERROR MALLOC FAILURE IN [ParsePar]\r\n")));
			return NULL;
		}
		// ��ʼ��lpLayout�ṹ
		lpPar->next = NULL;
		lpPar->lpImg = NULL; // ͼ��ṹ
		lpPar->lpAudio = NULL; // �����ṹ
		lpPar->lpText = NULL; // �ı��ṹ
		lpPar->lpRef = NULL; // REF�ṹ

		lpPar->startTime = 0 ; // ���뵱ǰ�õ�Ƭ�Ϳ�ʼ����
		lpPar->durTime = DEFAULT_DELAY ; // Ĭ��ÿһ���õ�Ƭ�ӳ�3s
		lpPar->endTime = 0 ; // Ĭ�ϻõƽ�������������
//		lpPar->durTime = -1 ; // ����ʱ�����޳�
//		lpPar->endTime = -1 ; // ����ʱ�����޳�

		// ��ʼ���� PAR �� ATTRIBUTE
		// PAR �� ATTRIBUTE ���� begin, end , dur

		while(1)
		{
			idAttr = SMIL_LocateAttribute(lppPtr); // �õ���ǩ��һ������
			if (idAttr == 0)
			{ // ��ǩ����
				break;
			}
			switch(idAttr)
			{
				case SMILATTR_BEGIN:
					RETAILMSG(1,(TEXT("current ATTR is \"BEGIN\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpPar->startTime = GetDurTime(lpAttrContent);
					break;
				case SMILATTR_END:
					RETAILMSG(1,(TEXT("current ATTR is \"BEGIN\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpPar->endTime = GetDurTime(lpAttrContent);
					break;
				case SMILATTR_DUR:
					RETAILMSG(1,(TEXT("current ATTR is \"BEGIN\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpPar->durTime = GetDurTime(lpAttrContent);
					break;
				default:
					RETAILMSG(1,(TEXT("current ID<%x> is Invalid ID \r\n"),idAttr));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					break;
			}
		}


		SMIL_ToNextTag(lppPtr); // ����һ����ǩ��λ��

		// ��ʼ����PAR����
		// PAR �����ݰ��� img,audio,text,ref
		while(1)
		{
			tagID=SMIL_LocateTag(lppPtr);  // �õ���ǩ���

			if(tagID == SMILTAG_NULL)
			{
				// �Ѿ�û��������
				RETAILMSG(1,(TEXT("!!!ERROR IN <LAYOUT> TAG ,CAN'T OCCUR NULL TAG\r\n")));
				break;
			}
			if(tagID == SMILTAG_PAR + SMILTAG_END)
			{
				// ��ǰ��</PAR>��ǩ
				break; // PAR �������Ѿ�����
			}
			else if(tagID == SMILTAG_IMG)
			{
				// ��ǰ��<IMG>��ǩ
				LPIMG lpImg;
				lpImg = ParseImg(lppPtr); // ����IMG������

				// ��lpImg ���뵽lpPar
				lpPar->lpImg = lpImg;
				SMIL_ToNextTag(lppPtr); // ����һ����ǩ��λ��
			}
			else if(tagID == SMILTAG_AUDIO)
			{
				// ��ǰ��<AUDIO>��ǩ
				LPAUDIO lpAudio;
				lpAudio = ParseAudio(lppPtr); // ����AUDIO������

				// ��lpAudio ���뵽lpPar
				lpPar->lpAudio = lpAudio;
				SMIL_ToNextTag(lppPtr); // ����һ����ǩ��λ��
			}
			else if(tagID == SMILTAG_TEXT)
			{
				// ��ǰ��<TEXT>��ǩ
				LPTEXT lpText;
				lpText = ParseText(lppPtr); // ����TEXT������

				// ��lpText ���뵽lpPar
				lpPar->lpText = lpText;
				SMIL_ToNextTag(lppPtr); // ����һ����ǩ��λ��
			}
			else if(tagID == SMILTAG_REF)
			{
				// ��ǰ��<REF>��ǩ
				LPREF lpRef;
				lpRef = ParseRef(lppPtr); // ����REF������

				// ��lpRef ���뵽lpPar
				lpPar->lpRef = lpRef;
				SMIL_ToNextTag(lppPtr); // ����һ����ǩ��λ��
			}
			else
			{ // ��ǰ��ǩ����Ч��ǩ
				SMIL_ToNextTag(lppPtr); // ����һ����ǩ��λ��
			}
		}

		AdjustTheParTime(lpPar); // �����õ�Ƭʱ��
		RETAILMSG(1,(TEXT("-ParsePar\r\n")));
		return lpPar;
}


// **************************************************
// ������static LPIMG ParseImg(LPTSTR *lppPtr)
// ������
// 	IN/OUT lppPtr -- ָ��ǰҪ�������ı����ݣ����������µ�ָ��
// 
// ����ֵ�����ط������IMGָ��
// �������� ����IMG�����ݡ�
// ����: 
// **************************************************
static LPIMG ParseImg(LPTSTR *lppPtr)
{
	LPIMG lpImg;
	ATTRID idAttr;
	TCHAR lpAttrContent[MAX_ATTRCONTENTLEN];

		RETAILMSG(1,(TEXT("+ParseImg\r\n")));
		lpImg = (LPIMG)malloc(sizeof(IMG)); // ����IMG�ṹ���ڴ�
		if (lpImg == NULL)
		{
			// �����ڴ�ʧ��
			return NULL;
		}
		// ��ʼ��IMG �ṹ
		lpImg->lpRegion = NULL;
		lpImg->lpAlt = NULL;
		lpImg->lpSrc = NULL;
		lpImg->next = NULL;

		// ��ʼ���� IMG �� ATTRIBUTE
		// IMG �� ATTRIBUTE ���� src, region, alt
		while(1)
		{
			idAttr = SMIL_LocateAttribute(lppPtr); // �õ���ǩ��һ������
			if (idAttr == 0)
			{ // ��ǩ����
				break;
			}
			switch(idAttr)
			{
				case SMILATTR_SRC:
					RETAILMSG(1,(TEXT("current ATTR is \"SRC\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpImg->lpSrc = SMILBufferAssign(lpAttrContent);
					break;
				case SMILATTR_REGION:
					RETAILMSG(1,(TEXT("current ATTR is \"REGION\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpImg->lpRegion = SMILBufferAssign(lpAttrContent);
					break;
				default:
					RETAILMSG(1,(TEXT("current ID<%x> is Invalid ID \r\n"),idAttr));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					break;
			}
		}

		RETAILMSG(1,(TEXT("-ParseImg\r\n")));
		return lpImg;

}


// **************************************************
// ������static LPAUDIO ParseAudio(LPTSTR *lppPtr)
// ������
// 	IN/OUT lppPtr -- ָ��ǰҪ�������ı����ݣ����������µ�ָ��
// 
// ����ֵ�����ط������AUDIOָ��
// �������� ����AUDIO�����ݡ�
// ����: 
// **************************************************
static LPAUDIO ParseAudio(LPTSTR *lppPtr)
{
	LPAUDIO lpAudio;
	ATTRID idAttr;
	TCHAR lpAttrContent[MAX_ATTRCONTENTLEN];

		RETAILMSG(1,(TEXT("+ParseAudio\r\n")));
		lpAudio = (LPAUDIO)malloc(sizeof(AUDIO)); // ����AUDIO�ṹ���ڴ�
		if (lpAudio == NULL)
		{
			// �����ڴ�ʧ��
			return NULL;
		}
		// ��ʼ��AUDIO �ṹ
		lpAudio->lpSrc = NULL;
		lpAudio->lpAlt = NULL;
		lpAudio->next = NULL;

		// ��ʼ���� AUDIO �� ATTRIBUTE
		// AUDIO �� ATTRIBUTE ���� src, alt
		while(1)
		{
			idAttr = SMIL_LocateAttribute(lppPtr); // �õ���ǩ��һ������
			if (idAttr == 0)
			{ // ��ǩ����
				break;
			}
			switch(idAttr)
			{
				case SMILATTR_SRC:
					RETAILMSG(1,(TEXT("current ATTR is \"SRC\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpAudio->lpSrc = SMILBufferAssign(lpAttrContent);
					break;
				case SMILATTR_ALT:
					RETAILMSG(1,(TEXT("current ATTR is \"REGION\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
//					lpText->lpRegion = SMILBufferAssign(lpAttrContent);
					break;
				default:
					RETAILMSG(1,(TEXT("current ID<%x> is Invalid ID \r\n"),idAttr));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					break;
			}
		}

		RETAILMSG(1,(TEXT("-ParseAudio\r\n")));
		return lpAudio;

}


// **************************************************
// ������static LPTEXT ParseText(LPTSTR *lppPtr)
// ������
// 	IN/OUT lppPtr -- ָ��ǰҪ�������ı����ݣ����������µ�ָ��
// 
// ����ֵ�����ط������TEXTָ��
// �������� ����TEXT�����ݡ�
// ����: 
// **************************************************
static LPTEXT ParseText(LPTSTR *lppPtr)
{
	LPTEXT lpText;
	ATTRID idAttr;
	TCHAR lpAttrContent[MAX_ATTRCONTENTLEN];

		RETAILMSG(1,(TEXT("+ParseText\r\n")));
		lpText = (LPTEXT)malloc(sizeof(TEXT)); // ����TEXT�ṹ���ڴ�
		if (lpText == NULL)
		{
			// �����ڴ�ʧ��
			return NULL;
		}
		// ��ʼ��TEXT �ṹ
		lpText->lpAlt = NULL;
		lpText->lpRegion = NULL;
		lpText->lpSrc = NULL;
		lpText->next = NULL;

		// ��ʼ���� TEXT �� ATTRIBUTE
		// TEXT �� ATTRIBUTE ���� src, region, alt
		while(1)
		{
			idAttr = SMIL_LocateAttribute(lppPtr); // �õ���ǩ��һ������
			if (idAttr == 0)
			{ // ��ǩ����
				break;
			}
			switch(idAttr)
			{
				case SMILATTR_SRC:
					RETAILMSG(1,(TEXT("current ATTR is \"SRC\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpText->lpSrc = SMILBufferAssign(lpAttrContent);
					break;
				case SMILATTR_REGION:
					RETAILMSG(1,(TEXT("current ATTR is \"REGION\"\r\n")));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					lpText->lpRegion = SMILBufferAssign(lpAttrContent);
					break;
				default:
					RETAILMSG(1,(TEXT("current ID<%x> is Invalid ID \r\n"),idAttr));
					SMIL_LocateAttributeContent(lppPtr,lpAttrContent,MAX_ATTRCONTENTLEN); 
					RETAILMSG(1,(TEXT("Attr Content = <%s>\r\n"),lpAttrContent));
					break;
			}
		}

		RETAILMSG(1,(TEXT("-ParseText\r\n")));
		return lpText;

}


// **************************************************
// ������static LPREF ParseRef(LPTSTR *lppPtr)
// ������
// 	IN/OUT lppPtr -- ָ��ǰҪ�������ı����ݣ����������µ�ָ��
// 
// ����ֵ�����ط������REFָ��
// �������� ����REF�����ݡ�
// ����: 
// **************************************************
static LPREF ParseRef(LPTSTR *lppPtr)
{
	LPREF lpRef;

		RETAILMSG(1,(TEXT("+ParseRef\r\n")));
		lpRef = (LPREF)malloc(sizeof(REF)); // ����REF�ṹ���ڴ�
		if (lpRef == NULL)
		{
			// �����ڴ�ʧ��
			return NULL;
		}
		// ��ʼ��REF �ṹ

		// ��ʼ���� REF �� ATTRIBUTE
		// REF �� ATTRIBUTE ���� src, alt
		while(1)
		{
			break;
		}

		RETAILMSG(1,(TEXT("-ParseRef\r\n")));
		return lpRef;

}


// **************************************************
// ������static UINT GetPixels(LPTSTR lpAttrContent)
// ������
// 	IN lpAttrContent -- ������ǰ�������ı�����
// 
// ����ֵ�����ص�ǰ�ĵ���
// �����������õ���ǰ�ĵ�����
// ����: 
// **************************************************
static UINT GetPixels(LPTSTR lpAttrContent)
{
	return atoi(lpAttrContent); // ���ı�ת��Ϊ����
}

// **************************************************
// ������static UINT GetPixels(LPTSTR lpAttrContent)
// ������
// 	IN lpAttrContent -- ������ǰ�������ı�����
// 
// ����ֵ�����ص�ǰҪ������ʱ��
// �����������õ���ǰҪ������ʱ�䡣
// ����: 
// **************************************************
static UINT GetDurTime(LPTSTR lpAttrContent)
{
	LPTSTR lpUnit;

		lpUnit = lpAttrContent; 
		while(1)
		{
			if (*lpUnit > '9' || *lpUnit < '0')
				break;
			lpUnit++;
		}
		if (strnicmp(lpUnit,"s",1) == 0)
		{
			// ��λ����
			return atoi(lpAttrContent) * 1000; // ���ı�ת��Ϊ����
		}
		else if (strnicmp(lpUnit,"ms",2) == 0)
		{
			// ��λ�Ǻ���
			return atoi(lpAttrContent); // ���ı�ת��Ϊ����
		}
		else
		{
			// ����ʶ�ĵ�λ
			return DEFAULT_DELAY; // ����Ĭ��ֵ
		}
}


// **************************************************
// ������LPTSTR SMILBufferAssign(const LPTSTR pSrc)
// ������
// 	IN pSrc -- Ҫ���Ƶ��ı���
// 
// ����ֵ���ɹ������´������ı��������򷵻�NULL
// ��������������һ����ԭ���ı�һ������µ��ı�������ԭ�����ı����Ƶ��´���
// ����: 
// **************************************************
LPTSTR SMILBufferAssign(const LPTSTR pSrc)
{
	LPTSTR ptr;
	DWORD dwLen;

		if (pSrc==NULL)
			return NULL;
		dwLen=strlen(pSrc)+1;
		ptr=(TCHAR *)malloc(dwLen*sizeof(TCHAR));
		if (ptr==NULL)
		{
			MessageBox(NULL,TEXT("The memory is not enough"),TEXT("memory alloc failure"),MB_OK);
			return NULL;
		}
		strcpy(ptr,pSrc);
		return ptr;
}


// **************************************************
// ������static void AdjustTheParTime(LPPAR lpPar)
// ������
// 	IN lpPar -- ��ǰ�Ļõ�Ƭ����
// 
// ����ֵ����
// ����������������ǰ�õ�Ƭ��ʱ�䡣
// ����: 
// **************************************************
static void AdjustTheParTime(LPPAR lpPar)
{
	// ע�⣺ 1��endTime ������� startTime
	//		  2��endTime ���� >= startTime + durTime

	if (lpPar->durTime == -1 && lpPar->endTime == -1)
	{ // ��ǰ�õƽ�һֱ������ȥ
		return ; 
	}
	if (lpPar->durTime == -1)
	{
		// ����ʱ�����,��Ҫ����
		if (lpPar->endTime < lpPar->startTime)
		{
			// ����ʱ�����,��������ʱ��
			lpPar->endTime = lpPar->startTime;
		}
		// ��������ʱ��
		lpPar->durTime = lpPar->endTime - lpPar->startTime;

		// �����ɹ�
		return ;
	}

	if (lpPar->endTime < lpPar->durTime + lpPar->startTime)
	{ // ����ʱ�����,��������ʱ��
		lpPar->endTime = lpPar->durTime + lpPar->startTime;
	}

	return ;
}
