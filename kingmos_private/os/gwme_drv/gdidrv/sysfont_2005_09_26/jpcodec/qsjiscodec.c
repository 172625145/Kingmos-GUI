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
QSjisCodec * QSjisCodec_Init()  //::QSjisCodec() // : conv(QJpUnicodeConv::newConverter(QJpUnicodeConv::Default))
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
VOID QSjisCodec_Deinit( QSjisCodec * lpThis )
{
//    delete (QJpUnicodeConv*)conv;
//    conv = 0;
	lpThis->conv->lpDrv->JpUnicodeConv_Deinit( lpThis->conv );
	lpThis->conv = 0;
	free( lpThis );
}

/*
QByteArray QSjisCodec::convertFromUnicode(const QChar *uc, int len, ConverterState *state) const
{
    char replacement = '?';
    if (state) {
        if (state->flags & ConvertInvalidToNull)
            replacement = 0;
    }
    int invalid = 0;

    int rlen = 2*len + 1;
    QByteArray rstr;
    rstr.resize(rlen);
    UCHAR* cursor = (UCHAR*)rstr.data();
    for (int i = 0; i < len; i++) {
        QChar ch = uc[i];
        UINT j;
        if (ch.row() == 0x00 && ch.cell() < 0x80) {
            // ASCII
            *cursor++ = ch.cell();
        } else if ((j = conv->unicodeToJisx0201(ch.row(), ch.cell())) != 0) {
            // JIS X 0201 Latin or JIS X 0201 Kana
            *cursor++ = j;
        } else if ((j = conv->unicodeToSjis(ch.row(), ch.cell())) != 0) {
            // JIS X 0208
            *cursor++ = (j >> 8);
            *cursor++ = (j & 0xff);
        } else if ((j = conv->unicodeToJisx0212(ch.row(), ch.cell())) != 0) {
            // JIS X 0212 (can't be encoded in ShiftJIS !)
            *cursor++ = 0x81;        // white square
            *cursor++ = 0xa0;        // white square
        } else {
            // Error
            *cursor++ = replacement;
            ++invalid;
        }
    }
    rstr.resize(cursor - (const UCHAR*)rstr.constData());

    if (state) {
        state->invalidChars += invalid;
    }
    return rstr;
}
*/

WORD QSjisCodec_convertToUnicode( QSjisCodec * lpThis, const char* chars, int len ) //, ConverterState *state) const
{
    UCHAR buf[1];
    int nbuf = 0;
    BYTE replacement = '?';//QChar::ReplacementCharacter;
	WORD result;
	int i;
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
    for ( i=0; i<len; i++) {
        UCHAR ch = chars[i];
        switch (nbuf) {
        case 0:
            if (ch < 0x80 || IsKana(ch)) {
                // JIS X 0201 Latin or JIS X 0201 Kana
                //UINT u = conv->jisx0201ToUnicode(ch);
                //result += QValidChar(u);
				UINT u = QJpUnicodeConv_jisx0201ToUnicode( lpThis->conv,ch );
				result = QValidChar(u);
				goto _return;
            } else if (IsSjisChar1(ch)) {
                // JIS X 0208
                buf[0] = ch;
                nbuf = 1;
            } else {
                // Invalid
                //result += replacement;
                //++invalid;
				result = replacement;
				goto _return;
            }
            break;
        case 1:
            // JIS X 0208
            if (IsSjisChar2(ch)) {
                if (IsUserDefinedChar1(buf[0])) {
                    //result += QChar::ReplacementCharacter;
					result = replacement;//QChar::ReplacementCharacter;
					goto _return;
                } else {
                    //UINT u = conv->sjisToUnicode(buf[0], ch);
                    //result += QValidChar(u);
                    UINT u = lpThis->conv->lpDrv->sjisToUnicode(lpThis->conv,buf[0], ch);
                    result = QValidChar(u);
					goto _return;
                }
            } else {
                // Invalid
                //result += replacement;
                //++invalid;
				result = replacement;
				goto _return;
            }
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
_return:

    return result;
}


int QSjisCodec_mibEnum()
{
    return 17;
}

LPCBYTE QSjisCodec_name()
{
    return "Shift_JIS";
}

/*!
    Returns the codec's mime name.
*/
LPCBYTE QSjisCodec_aliases()
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

WORD JapanCodec_GetCodeIndex( HANDLE h, LPCBYTE lpbCode, UINT len )
{
	return QSjisCodec_convertToUnicode( (QSjisCodec*)h, lpbCode, len );
}
