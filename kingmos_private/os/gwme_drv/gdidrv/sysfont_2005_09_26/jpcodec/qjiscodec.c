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

// Most of the code here was originally written by Serika Kurusugawa,
// a.k.a. Junji Takagi, and is included in Qt with the author's permission
// and the grateful thanks of the Trolltech team.

/*! \class QJisCodec
    \reentrant
    \internal

  \brief The QJisCodec class provides conversion to and from JIS character sets.

  More precisely, the QJisCodec class subclasses QTextCodec to
  provide support for JIS X 0201 Latin, JIS X 0201 Kana, JIS X 0208
  and JIS X 0212.

  The environment variable UNICODEMAP_JP can be used to fine-tune
  QJisCodec, QSjisCodec and QEucJpCodec. The mapping names are as for
  the Japanese XML working group's \link
  http://www.y-adagio.com/public/standards/tr_xml_jpf/toc.htm XML
  Japanese Profile\endlink, because it names and explains all the
  widely used mappings. Here are brief descriptions, written by
  Serika Kurusugawa:

  \list

  \i "unicode-0.9" or "unicode-0201" for Unicode style. This assumes
  JISX0201 for 0x00-0x7f. (0.9 is a table version of jisx02xx mapping
  used for Uniocde spec version 1.1.)

  \i "unicode-ascii" This assumes US-ASCII for 0x00-0x7f; some
  chars (JISX0208 0x2140 and JISX0212 0x2237) are different from
  Unicode 1.1 to avoid conflict.

  \i "open-19970715-0201" ("open-0201" for convenience) or
  "jisx0221-1995" for JISX0221-JISX0201 style. JIS X 0221 is JIS
  version of Unicode, but a few chars (0x5c, 0x7e, 0x2140, 0x216f,
  0x2131) are different from Unicode 1.1. This is used when 0x5c is
  treated as YEN SIGN.

  \i "open-19970715-ascii" ("open-ascii" for convenience) for
  JISX0221-ASCII style. This is used when 0x5c is treated as REVERSE
  SOLIDUS.

  \i "open-19970715-ms" ("open-ms" for convenience) or "cp932" for
  Microsoft Windows style. Windows Code Page 932. Some chars (0x2140,
  0x2141, 0x2142, 0x215d, 0x2171, 0x2172) are different from Unicode
  1.1.

  \i "jdk1.1.7" for Sun's JDK style. Same as Unicode 1.1, except that
  JIS 0x2140 is mapped to UFF3C. Either ASCII or JISX0201 can be used
  for 0x00-0x7f.

  \endlist

  In addition, the extensions "nec-vdc", "ibm-vdc" and "udc" are
  supported.

  For example, if you want to use Unicode style conversion but with
  NEC's extension, set \c UNICODEMAP_JP to
  <nobr>\c {unicode-0.9, nec-vdc}.</nobr> (You will probably
  need to quote that in a shell command.)

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

#include <eframe.h>

#include "qjiscodec.h"
//#include "qlist.h"

enum {
    Esc = 0x1b,
    So = 0x0e,         // Shift Out
    Si = 0x0f,         // Shift In

    ReverseSolidus = 0x5c,
    YenSign = 0x5c,
    Tilde = 0x7e,
    Overline = 0x7e
};

#define        IsKana(c)        (((c) >= 0xa1) && ((c) <= 0xdf))
#define        IsJisChar(c)        (((c) >= 0x21) && ((c) <= 0x7e))

#define        QValidChar(u)        ( (u) ? ( (USHORT)(u) ) : (BYTE)('?'))

enum Iso2022State{ Ascii, MinState = Ascii,
                   JISX0201_Latin, JISX0201_Kana,
                   JISX0208_1978, JISX0208_1983,
                   JISX0212, MaxState = JISX0212,
                   UnknownState };

static const char Esc_CHARS[] = "()*+-./";

static const char Esc_Ascii[]                 = {Esc, '(', 'B', 0 };
static const char Esc_JISX0201_Latin[]        = {Esc, '(', 'J', 0 };
static const char Esc_JISX0201_Kana[]        = {Esc, '(', 'I', 0 };
static const char Esc_JISX0208_1978[]        = {Esc, '$', '@', 0 };
static const char Esc_JISX0208_1983[]        = {Esc, '$', 'B', 0 };
static const char Esc_JISX0212[]        = {Esc, '$', '(', 'D', 0 };
static const char * const Esc_SEQ[] = { Esc_Ascii,
                                        Esc_JISX0201_Latin,
                                        Esc_JISX0201_Kana,
                                        Esc_JISX0208_1978,
                                        Esc_JISX0208_1983,
                                        Esc_JISX0212 };

/*! \internal */
QJisCodec * QJisCodec_Init() //::QJisCodec() : conv(QJpUnicodeConv::newConverter(QJpUnicodeConv::Default))
{
	QJisCodec * lpThis = malloc( sizeof(QJisCodec) );
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


/*! \internal */
void QJisCodec_Deinit( QJisCodec * lpThis ) //::~QJisCodec()
{
	
    //delete (QJpUnicodeConv*)conv;
    //conv = 0;
	lpThis->conv->lpDrv->JpUnicodeConv_Deinit( lpThis->conv );
	lpThis->conv = 0;
	free( lpThis );
}

/*
QJisCodec::convertFromUnicode(QJisCodec * lpThis, const BYTE *uc, int len, ConverterState *cs, LPBYTE lpByteArray )// const
{
    char replacement = '?';
    if (cs) {
        if (cs->flags & ConvertInvalidToNull)
            replacement = 0;
    }
    int invalid = 0;

    QByteArray result;
    Iso2022State state = Ascii;
    Iso2022State prev = Ascii;
    for (int i = 0; i < len; i++) {
        BYTE ch = uc[i];
        UINT j;
        if (ch.row() == 0x00 && ch.cell() < 0x80) {
            // Ascii
            if (state != JISX0201_Latin ||
                ch.cell() == ReverseSolidus || ch.cell() == Tilde) {
                state = Ascii;
            }
            j = ch.cell();
        } else if ((j = conv->unicodeToJisx0201(ch.row(), ch.cell())) != 0) {
            if (j < 0x80) {
                // JIS X 0201 Latin
                if (state != Ascii ||
                    ch.cell() == YenSign || ch.cell() == Overline) {
                    state = JISX0201_Latin;
                }
            } else {
                // JIS X 0201 Kana
                state = JISX0201_Kana;
                j &= 0x7f;
            }
        } else if ((j = conv->unicodeToJisx0208(ch.row(), ch.cell())) != 0) {
            // JIS X 0208
            state = JISX0208_1983;
        } else if ((j = conv->unicodeToJisx0212(ch.row(), ch.cell())) != 0) {
            // JIS X 0212
            state = JISX0212;
        } else {
            // Invalid
            state = UnknownState;
            j = replacement;
            ++invalid;
        }
        if (state != prev) {
            if (state == UnknownState) {
                result += Esc_Ascii;
            } else {
                result += Esc_SEQ[state - MinState];
            }
            prev = state;
        }
        if (j < 0x0100) {
            result += j & 0xff;
        } else {
            result += (j >> 8) & 0xff;
            result += j & 0xff;
        }
    }
    if (prev != Ascii) {
        result += Esc_Ascii;
    }

    if (cs) {
        cs->invalidChars += invalid;
    }
    return result;
}
*/

WORD QJisCodec_convertToUnicode( QJisCodec * lpThis, const char* chars, int len )//, ConverterState *cs, LPBYTE lpBuf ) // const
{
    UCHAR buf[4];
    int nbuf = 0;
    //Iso2022State state = Ascii, prev = Ascii;
	int state = Ascii, prev = Ascii;
    BOOL esc = FALSE;
    BYTE replacement = '?';//BYTE::ReplacementCharacter;
	int i;
	WORD result;

	/*
    if (cs) {
        if (cs->flags & ConvertInvalidToNull)
            replacement = BYTE::Null;
        nbuf = cs->remainingChars;
        buf[0] = (cs->state_data[0] >> 24) & 0xff;
        buf[1] = (cs->state_data[0] >> 16) & 0xff;
        buf[2] = (cs->state_data[0] >>  8) & 0xff;
        buf[3] = (cs->state_data[0] >>  0) & 0xff;
        state = (Iso2022State)((cs->state_data[1] >>  0) & 0xff);
        prev = (Iso2022State)((cs->state_data[1] >>  8) & 0xff);
        esc = cs->state_data[2];
    }
    int invalid = 0;	

    QString result;
	*/

    for (i=0; i<len; i++) {
        UCHAR ch = chars[i];
        if (esc) {
            // Escape sequence
            state = UnknownState;
            switch (nbuf) {
            case 0:
                if (ch == '$' || strchr(Esc_CHARS, ch)) {
                    buf[nbuf++] = ch;
                } else {
                    nbuf = 0;
                    esc = FALSE;
                }
                break;
            case 1:
                if (buf[0] == '$') {
                    if (strchr(Esc_CHARS, ch)) {
                        buf[nbuf++] = ch;
                    } else {
                        switch (ch) {
                        case '@':
                            state = JISX0208_1978;        // Esc $ @
                            break;
                        case 'B':
                            state = JISX0208_1983;        // Esc $ B
                            break;
                        }
                        nbuf = 0;
                        esc = FALSE;
                    }
                } else {
                    if (buf[0] == '(') {
                        switch (ch) {
                        case 'B':
                            state = Ascii;        // Esc (B
                            break;
                        case 'I':
                            state = JISX0201_Kana;        // Esc (I
                            break;
                        case 'J':
                            state = JISX0201_Latin;        // Esc (J
                            break;
                        }
                    }
                    nbuf = 0;
                    esc = FALSE;
                }
                break;
            case 2:
                if (buf[1] == '(') {
                    switch (ch) {
                    case 'D':
                        state = JISX0212;        // Esc $ (D
                        break;
                    }
                }
                nbuf = 0;
                esc = FALSE;
                break;
            }
        } else {
            if (ch == Esc) {
                // Escape sequence
                nbuf = 0;
                esc = TRUE;
            } else if (ch == So) {
                // Shift out
                prev = state;
                state = JISX0201_Kana;
                nbuf = 0;
            } else if (ch == Si) {
                // Shift in
                if (prev == Ascii || prev == JISX0201_Latin) {
                    state = prev;
                } else {
                    state = Ascii;
                }
                nbuf = 0;
            } else {
                UINT u;
                switch (nbuf) {
                case 0:
                    switch (state) {
                    case Ascii:
                        if (ch < 0x80) {
                            //result += QLatin1Char(ch);
							result = ch;//QLatin1Char(ch);
                            goto _return;
							break;
                        }
                        /* fall through */
                    case JISX0201_Latin:
                        u = QJpUnicodeConv_jisx0201ToUnicode( lpThis->conv, ch );//lpThis->conv->lpDrv->jisx0201ToUnicode( lpThis->conv, ch);
                        //result += QValidChar(u);
						result = QValidChar(u);
						goto _return;
                        break;
                    case JISX0201_Kana:
                        //u = conv->jisx0201ToUnicode(ch | 0x80);
						u = QJpUnicodeConv_jisx0201ToUnicode( lpThis->conv, ch | 0x80);
                        result = QValidChar(u);
						goto _return;
                        break;
                    case JISX0208_1978:
                    case JISX0208_1983:
                    case JISX0212:
                        buf[nbuf++] = ch;
                        break;
                    default:
                        //result += BYTE::ReplacementCharacter;
						result = replacement;//ReplacementCharacter;
						goto _return;
                        break;
                    }
                    break;
                case 1:
                    switch (state) {
                    case JISX0208_1978:
                    case JISX0208_1983:
                        //u = conv->jisx0208ToUnicode(buf[0] & 0x7f, ch & 0x7f);
						u = lpThis->conv->lpDrv->jisx0208ToUnicode( lpThis->conv, buf[0] & 0x7f, ch & 0x7f);
                        //result += QValidChar(u);
						result = QValidChar(u);
						goto _return;
                        break;
                    case JISX0212:
                        //u = conv->jisx0212ToUnicode(buf[0] & 0x7f, ch & 0x7f);
						u = lpThis->conv->lpDrv->jisx0212ToUnicode(lpThis->conv, buf[0] & 0x7f, ch & 0x7f);
                        //result += QValidChar(u);
						result = QValidChar(u);
						goto _return;
                        break;
                    default:
                        //result += replacement;
						result = replacement;
                        //++invalid;
						goto _return;
                        break;
                    }
                    nbuf = 0;
                    break;
                }
            }
        }
    }
/*
    if (cs) {
        cs->remainingChars = nbuf;
        cs->invalidChars += invalid;
        cs->state_data[0] = (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
        cs->state_data[1] = (prev << 8) + state;
        cs->state_data[2] = esc;
    }
*/
_return:

    return result;
}



/*! \internal */
int QJisCodec_mibEnum()
{
    return 39;
}

/*! \internal */
LPCBYTE QJisCodec_name()
{
    return "ISO-2022-JP";
}

/*!
    Returns the codec's mime name.
*/
LPCBYTE QJisCodec_aliases()
{
    //QList<QByteArray> list;
    //list << "JIS7"; // Qt 3 compat
    return "JIS7";//list;
}

/*
VOID * JapanCodec_Init( void )
{
	return QJisCodec_Init();
}

WORD JapanCodec_GetCodeIndex( HANDLE h, LPCBYTE lpbCode, UINT len )
{
	return QJisCodec_convertToUnicode( (QJisCodec*)h, lpbCode, len );
}

*/