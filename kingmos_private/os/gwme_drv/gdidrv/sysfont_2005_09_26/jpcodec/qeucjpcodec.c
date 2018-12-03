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

/*! \class QEucJpCodec qeucjpcodec.h
    \reentrant
    \internal

  \brief The QEucJpCodec class provides conversion to and from EUC-JP character sets.

  More precisely, the QEucJpCodec class subclasses QTextCodec to
  provide support for EUC-JP, the main legacy encoding for Unix
  machines in Japan.

  The environment variable \c UNICODEMAP_JP can be used to fine-tune
  QJisCodec, QSjisCodec and QEucJpCodec. The \l QJisCodec
  documentation describes how to use this variable.

  Most of the code here was written by Serika Kurusugawa,
  a.k.a. Junji Takagi, and is included in Qt with the author's
  permission and the grateful thanks of the Trolltech team. Here is
  the copyright statement for that code:

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

/*
 * Copyright (C) 1999 Serika Kurusugawa, All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <eframe.h>

#include "qeucjpcodec.h"

static const UCHAR Esc = 0x1b;
static const UCHAR Ss2 = 0x8e;        // Single Shift 2
static const UCHAR Ss3 = 0x8f;        // Single Shift 3

#define        IsKana(c)        (((c) >= 0xa1) && ((c) <= 0xdf))
#define        IsEucChar(c)        (((c) >= 0xa1) && ((c) <= 0xfe))

#define        QValidChar(u)        ((u) ? ((USHORT)(u)) : (BYTE)('?'))

/*!
  Constructs a QEucJpCodec.
*/
QEucJpCodec* QEucJpCodec_Init() //::QEucJpCodec() // : conv(QJpUnicodeConv::newConverter(QJpUnicodeConv::Default))
{
	QEucJpCodec* lpQEucJpCodec = (QEucJpCodec*)malloc( sizeof(QEucJpCodec) );

	if( lpQEucJpCodec )
	{
		lpQEucJpCodec->conv = (QJpUnicodeConv*)QJpUnicodeConv_NewConverter(QJpUnicodeConv_Default);
		if( !lpQEucJpCodec->conv )
		{
			free( lpQEucJpCodec );
			lpQEucJpCodec = NULL;
		}
			
	}
	return lpQEucJpCodec;
	
}

/*!
  Destroys the codec.
*/
void QEucJpCodec_Deinit(QEucJpCodec* lpQEucJpCodec)
{
    //delete (QJpUnicodeConv*)conv;
    //conv = 0;
	lpQEucJpCodec->conv = 0;
	free( lpQEucJpCodec );
}
/*
BOOL QEucJpCodec_ConvertFromUnicode( QEucJpCodec* lpQEucJpCodec, const BYTE *uc, int len ) //, ConverterState *state, BYTE lpszBuf )// const
{
    char replacement = '?';

	
    if (state) {
        if (state->flags & ConvertInvalidToNull)
            replacement = 0;
    }
    int invalid = 0;
	

    int rlen = 3*len + 1;
    QByteArray rstr;
    rstr.resize(rlen);
    UCHAR* cursor = (UCHAR*)rstr.data();
    for (int i = 0; i < len; i++) {
        BYTE ch = uc[i];
        UINT j;
        if (ch.unicode() < 0x80) {
            // ASCII
            *cursor++ = ch.cell();
        } else if ((j = conv->unicodeToJisx0201(ch.row(), ch.cell())) != 0) {
            if (j < 0x80) {
                // JIS X 0201 Latin ?
                *cursor++ = j;
            } else {
                // JIS X 0201 Kana
                *cursor++ = Ss2;
                *cursor++ = j;
            }
        } else if ((j = conv->unicodeToJisx0208(ch.row(), ch.cell())) != 0) {
            // JIS X 0208
            *cursor++ = (j >> 8)   | 0x80;
            *cursor++ = (j & 0xff) | 0x80;
        } else if ((j = conv->unicodeToJisx0212(ch.row(), ch.cell())) != 0) {
            // JIS X 0212
            *cursor++ = Ss3;
            *cursor++ = (j >> 8)   | 0x80;
            *cursor++ = (j & 0xff) | 0x80;
        } else {
            // Error
            *cursor++ = replacement;
            ++invalid;
        }
    }
    rstr.resize(cursor - (UCHAR*)rstr.constData());

    if (state) {
        state->invalidChars += invalid;
    }
    return rstr;
}
*/


//QString QEucJpCodec::convertToUnicode(const char* chars, int len, ConverterState *state) const
WORD QEucJpCodec_ConvertToUnicode(QEucJpCodec* lpQEucJpCodec, const char* chars, int len )//, ConverterState *state) const
{
    UCHAR buf[2];
    int nbuf = 0;
    BYTE replacement = '?';//BYTE::ReplacementCharacter;
	int i;
	WORD result;
	/*
    if (state) {
        if (state->flags & ConvertInvalidToNull)
            replacement = BYTE::Null;
        nbuf = state->remainingChars;
        buf[0] = state->state_data[0];
        buf[1] = state->state_data[1];
    }
	*/
    //int invalid = 0;

    //QString result;
    for ( i=0; i<len; i++) {
        UCHAR ch = chars[i];
        switch (nbuf) {
        case 0:
            if (ch < 0x80) {
                // ASCII
                //result += QLatin1Char(ch);
				result = ch;//QLatin1Char(ch);
            } else if (ch == Ss2 || ch == Ss3) {
                // JIS X 0201 Kana or JIS X 0212
                buf[0] = ch;
                nbuf = 1;
				continue;
            } else if (IsEucChar(ch)) {
                // JIS X 0208
                buf[0] = ch;
                nbuf = 1;
				continue;
            } else {
                // Invalid
                //result += replacement;
				result = replacement;
                //++invalid;
            }
            break;
        case 1:
            if (buf[0] == Ss2) {
                // JIS X 0201 Kana
                if (IsKana(ch)) {
                    UINT u = QJpUnicodeConv_jisx0201ToUnicode(lpQEucJpCodec->conv, ch);//conv->jisx0201ToUnicode(ch);
                    //result += QValidChar(u);
					result = u;
                } else {
                    //result += replacement;
					result = replacement;
                    //++invalid;
                }
                nbuf = 0;
            } else if (buf[0] == Ss3) {
                // JIS X 0212-1990
                if (IsEucChar(ch)) {
                    buf[1] = ch;
                    nbuf = 2;
					continue;
                } else {
                    // Error
                    //result += replacement;
					result = replacement;
                    //++invalid;
                    nbuf = 0;
                }
            } else {
                // JIS X 0208-1990
                if (IsEucChar(ch)) {
                    UINT u = lpQEucJpCodec->conv->lpDrv->jisx0208ToUnicode( lpQEucJpCodec->conv, buf[0] & 0x7f, ch & 0x7f );//conv->jisx0208ToUnicode(buf[0] & 0x7f, ch & 0x7f);
                    //result += QValidChar(u);
					result = u;
                } else {
                    // Error
                    //result += replacement;
					result = replacement;
                    //++invalid;
                }
                nbuf = 0;
            }
            break;
        case 2:
            // JIS X 0212
            if (IsEucChar(ch)) {
                UINT u = lpQEucJpCodec->conv->lpDrv->jisx0212ToUnicode( lpQEucJpCodec->conv, buf[1] & 0x7f, ch & 0x7f);
                //result += QValidChar(u);
				result = u;
            } else {
                //result += replacement;
				result = replacement;
                //++invalid;
            }
            nbuf = 0;
        }
		break;
    }
	/*
    if (state) {
        state->remainingChars = nbuf;
        state->state_data[0] = buf[0];
        state->state_data[1] = buf[1];
        state->invalidChars += invalid;
    }
	*/
    return result;
}

int QEucJpCodec_mibEnum( QEucJpCodec* lpQEucJpCodec )
{
    return 18;
}

LPCBYTE QEucJpCodec_Name( QEucJpCodec* lpQEucJpCodec  )
{
    return "EUC-JP";
}
/*
VOID * JapanCodec_Init( void )
{
	return QEucJpCodec_Init();
}

WORD JapanCodec_GetCodeIndex( HANDLE h, LPCBYTE lpbCode, UINT len )
{
	return QEucJpCodec_ConvertToUnicode( (QEucJpCodec*)h, lpbCode, len );
}

*/