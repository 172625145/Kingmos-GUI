/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：japan unicode & multi-byte codec, 原始代码来自与Qt，
版本号：1.0.0
开发时期：2005-09-12
作者：李林
修改记录：

******************************************************/

/****************************************************************************
**
** Copyright (C) 1992-2005 Trolltech AS. All rights reserved.
**
** This file is part of the internationalization module of the Qt Toolkit.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
** information about Qt Commercial License Agreements.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

// Most of the code here was originally written by Serika Kurusugawa
// a.k.a. Junji Takagi, and is included in Qt with the author's permission,
// and the grateful thanks of the Trolltech team.

/*! \class QSjisCodec qsjiscodec.h
    \reentrant
    \internal
  \brief The QSjisCodec class provides conversion to and from Shift-JIS.

  More precisely, the QSjisCodec class subclasses QTextCodec to
  provide support for Shift-JIS, an encoding of JIS X 0201 Latin, JIS
  X 0201 Kana or JIS X 0208.

  The environment variable \c UNICODEMAP_JP can be used to fine-tune
  QJisCodec, QSjisCodec and QEucJpCodec. The \l QJisCodec
  documentation describes how to use this variable.

  Most of the code here was written by Serika Kurusugawa,
  a.k.a. Junji Takagi, and is included in Qt with the author's
  permission and the grateful thanks of the Trolltech team.
  Here is the copyright statement for the code as it was at the
  point of contribution. Trolltech's subsequent modifications
  are covered by the usual copyright for Qt.

  \legalese

  Copyright (C) 1999 Serika Kurusugawa. All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  \list 1
  \i Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  \i Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
  \endlist

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS".
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
  SUCH DAMAGE.
*/

#include <eframe.h>
#include "qsjiscodec.h"
#include <unicode.h>
//#include "qlist.h"

enum {
    Esc = 0x1b
};

#define        IsKana(c)        (((c) >= 0xa1) && ((c) <= 0xdf))
#define        IsSjisChar1(c)        ((((c) >= 0x81) && ((c) <= 0x9f)) ||        \
                         (((c) >= 0xe0) && ((c) <= 0xfc)))
#define        IsSjisChar2(c)        (((c) >= 0x40) && ((c) != 0x7f) && ((c) <= 0xfc))
#define        IsUserDefinedChar1(c)        (((c) >= 0xf0) && ((c) <= 0xfc))

#define        QValidChar(u)        ((u) ? ((USHORT)(u)) : (BYTE)('?'))

/*!
  Creates a Shift-JIS codec. Note that this is done automatically by
  the QApplication, you do not need construct your own.
*/
static QSjisCodec * QSjisCodec_Init()  //::QSjisCodec() // : conv(QJpUnicodeConv::newConverter(QJpUnicodeConv::Default))
{
	QSjisCodec * lpThis = malloc( sizeof(QSjisCodec) );
	if( lpThis )
	{
		lpThis->conv = (QJpUnicodeConv *)QJpUnicodeConv_NewConverter( QJpUnicodeConv_Default );
		if( !lpThis->conv )
		{
			free( lpThis );
			lpThis = NULL;
		}
	}
	return lpThis;
}


/*!
  Destroys the Shift-JIS codec.
*/
static VOID QSjisCodec_Deinit( QSjisCodec * lpThis )
{
//    delete (QJpUnicodeConv*)conv;
//    conv = 0;
	lpThis->conv->lpDrv->JpUnicodeConv_Deinit( lpThis->conv );
	lpThis->conv = 0;
	free( lpThis );
}


static UINT QSjisCodec_ConvertFromUnicode( QSjisCodec * lpThis, const WORD *uc, int len, UCHAR * lpszBuf, int bufSize )
{
    char replacement = '?';
	/*
    if (state) {
        if (state->flags & ConvertInvalidToNull)
            replacement = 0;
    }
	*/
    int invalid = 0;
	int i;

    //int rlen = 2*len + 1;
    //QByteArray rstr;
    //rstr.resize(rlen);
    UCHAR* cursor = lpszBuf;//(UCHAR*)rstr.data();
	int cursorLen = bufSize;
    for (i = 0; i < len && cursorLen; i++) {
        WORD ch = uc[i];
        UINT j;
        //if (ch.row() == 0x00 && ch.cell() < 0x80) {
		if( ch < 0x80 ){
            // ASCII
			if( cursor )
				*cursor++ = (BYTE)ch;//ch.cell();
			cursorLen--;
        } else if ((j = QJpUnicodeConv_unicodeToJisx0201( lpThis->conv, ch )) != 0) {
            // JIS X 0201 Latin or JIS X 0201 Kana
			if( cursor )
				*cursor++ = j;
			cursorLen--;
        } else if ((j = QJpUnicodeConv_unicodeToSjis( lpThis->conv, ch)) != 0) {
            // JIS X 0208
			if( cursor )
				*cursor++ = (j >> 8);
			cursorLen--;
			if( !cursorLen )
				break;
			if( cursor )
				*cursor++ = (j & 0xff);
			cursorLen--;
        } else if ((j = QJpUnicodeConv_unicodeToJisx0212( lpThis->conv, ch )) != 0) {
            // JIS X 0212 (can't be encoded in ShiftJIS !)
			if( cursor )
				*cursor++ = 0x81;        // white square
			cursorLen--;
			if( !cursorLen )
				break;
			if( cursor )
				*cursor++ = 0xa0;        // white square
			cursorLen--;
        } else {
            // Error
			if( cursor )
				*cursor++ = replacement;
			cursorLen--;
            ++invalid;
        }
    }
/*
    rstr.resize(cursor - (const UCHAR*)rstr.constData());

    if (state) {
        state->invalidChars += invalid;
    }
	*/
    return bufSize - cursorLen;
}


static WORD QSjisCodec_convertToUnicode( 
										QSjisCodec * lpThis, 
										const char* chars, 
										int len, 
										WORD * lpwBuf, 
										int lenWord 
										)
{
    UCHAR buf[1];
    int nbuf = 0;
    BYTE replacement = '?';//QChar::ReplacementCharacter;
	//WORD result;
	int i;
	int curLen = lenWord;
	/*
    if (state) {
        if (state->flags & ConvertInvalidToNull)
            replacement = QChar::Null;
        nbuf = state->remainingChars;
        buf[0] = state->state_data[0];
    }
    int invalid = 0;

    QString result;
	*/
    for ( i=0; i<len && curLen; i++) {
        UCHAR ch = chars[i];
        switch (nbuf) {
        case 0:
            if (ch < 0x80 || IsKana(ch)) {
                // JIS X 0201 Latin or JIS X 0201 Kana
                //UINT u = conv->jisx0201ToUnicode(ch);
                //result += QValidChar(u);
				if( lpwBuf )
				{
					UINT u = QJpUnicodeConv_jisx0201ToUnicode( lpThis->conv,ch );
					//result = QValidChar(u);
					//goto _return;					
					*lpwBuf++ = QValidChar(u);
				}
				curLen--;
            } else if (IsSjisChar1(ch)) {
                // JIS X 0208
                buf[0] = ch;
                nbuf = 1;
            } else {
                // Invalid
                //result += replacement;
                //++invalid;
				//result = replacement;
				//goto _return;
				if( lpwBuf )
					*lpwBuf++ = replacement;
				curLen--;
            }
            break;
        case 1:
            // JIS X 0208
			if( lpwBuf )
			{
				if (IsSjisChar2(ch)) {					
					if (IsUserDefinedChar1(buf[0])) {
						//result += QChar::ReplacementCharacter;
						//result = replacement;//QChar::ReplacementCharacter;
						//goto _return;
						*lpwBuf++ = replacement;						
					} else {
						//UINT u = conv->sjisToUnicode(buf[0], ch);
						//result += QValidChar(u);
						UINT u = lpThis->conv->lpDrv->sjisToUnicode(lpThis->conv,buf[0], ch);
						//result = QValidChar(u);
						//goto _return;						
						*lpwBuf++ = QValidChar(u);
					}
				} else {
					// Invalid
					//result += replacement;
					//++invalid;
					//result = replacement;
					//goto _return;
					*lpwBuf++ = replacement;					
				}
			}
			curLen--;
            nbuf = 0;			
            break;
        }
    }
/*
    if (state) {
        state->remainingChars = nbuf;
        state->state_data[0] = buf[0];
        state->invalidChars += invalid;
    }
*/
//_return:

    //return result;
	return lenWord - curLen;
}


static int QSjisCodec_mibEnum()
{
    return 17;
}

static LPCBYTE QSjisCodec_name()
{
    return "Shift_JIS";
}

/*!
    Returns the codec's mime name.
*/
static LPCBYTE QSjisCodec_aliases()
{
//    QList<QByteArray> list;
    //list << "SJIS" // Qt 3 compat
         //<< "MS_Kanji";
    //return list;
	return "SJISMS_Kanji";
}

VOID * JapanCodec_Init( void )
{
	return QSjisCodec_Init();
}

VOID JapanCodec_Deinit( void * lpThis )
{
	QSjisCodec_Deinit( lpThis );
}


WORD JapanCodec_GetCodeIndex( HANDLE h, LPCBYTE lpbCode, UINT len )
{
	WORD v = '?';
	QSjisCodec_convertToUnicode( (QSjisCodec*)h, lpbCode, len, &v, 1 );
	return v;
}

int JapanCodec_ConvertToUnicode( HANDLE h, LPCBYTE lpbCode, int len, LPWSTR lpWideCharStr, int cchWideChar )
{
	return QSjisCodec_convertToUnicode( (QSjisCodec*)h, lpbCode, len, lpWideCharStr, cchWideChar );	
}

int JapanCodec_ConvertFromUnicode( 
								  HANDLE h, 
								  LPCWSTR lpWideCharStr, 
								  int cchWideChar, 
								  LPSTR lpMultiByteStr, 
								  int cbMultiByte,
								  LPCSTR lpDefaultChar,
								  BOOL * lpUsedDefaultChar )
{
	return QSjisCodec_ConvertFromUnicode( (QSjisCodec*)h, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte );
}


// 
static DWORD Create( DWORD dwReserver );
static BOOL Delete( DWORD dwHandle );
static DWORD GetUnicode( DWORD dwHandle,
				         LPCSTR lpMultiByteStr,
				         int cbMultiByte,  
				         LPWSTR lpWideCharStr,
				         int cchWideChar );
static int GetChar( DWORD dwHandle,
			   LPCWSTR lpWideCharStr, 
			   int cchWideChar, 
			   LPSTR lpMultiByteStr, 
			   int cbMultiByte, 
			   LPCSTR lpDefaultChar, 
			   BOOL * lpUsedDefaultChar );
static int CountBufferSize( DWORD dwHandle,
				     LPCVOID lpStr,
				     int cbCountLen,
			         BOOL IsWideChar );


const CODE_PAGE_DRV JapanDrv = 
{
	Create,
	Delete,
	GetUnicode,
	GetChar,
	CountBufferSize
};


static DWORD Create( DWORD dwReserver )
{
	return (DWORD)JapanCodec_Init();
}

static BOOL Delete( DWORD dwHandle )
{
	JapanCodec_Deinit( (void *)dwHandle );	
	return TRUE;
}

// japan->unicode
static DWORD GetUnicode( DWORD dwHandle,
				         LPCSTR lpMultiByteStr,
				         int cbMultiByte,  
				         LPWSTR lpWideCharStr,
				         int cchWideChar )
{
	return JapanCodec_ConvertToUnicode( (void *)dwHandle, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar );
}

// unicode -> japan
static int GetChar( DWORD dwHandle,
			   LPCWSTR lpWideCharStr, 
			   int cchWideChar, 
			   LPSTR lpMultiByteStr, 
			   int cbMultiByte, 
			   LPCSTR lpDefaultChar, 
			   BOOL * lpUsedDefaultChar )
{
	return JapanCodec_ConvertFromUnicode( (void *)dwHandle, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar );
}

//统计需要的字符数
static int CountBufferSize( DWORD dwHandle,
				     LPCVOID lpStr,
				     int cbCountLen,
			         BOOL IsWideChar )
{
	int iCount = 0;
	if( IsWideChar )
	{   // wide-char
		iCount = JapanCodec_ConvertFromUnicode( (void *)dwHandle, lpStr, cbCountLen, NULL, MAXLONG, NULL, NULL );
	}
	else
	{   // multibytes
		iCount = JapanCodec_ConvertToUnicode( (void *)dwHandle, lpStr, cbCountLen, NULL, MAXLONG );
	}
	return iCount;
}

