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

#ifndef QJPUNICODE_H
#define QJPUNICODE_H

#if 0 
#include "qglobal.h"

class QJpUnicodeConv {
public:
    virtual ~QJpUnicodeConv() {}
    enum Rules {
        // "ASCII" is ANSI X.3.4-1986, a.k.a. US-ASCII here.
        Default                        = 0x0000,

        Unicode                        = 0x0001,
        Unicode_JISX0201                = 0x0001,
        Unicode_ASCII                 = 0x0002,
        JISX0221_JISX0201         = 0x0003,
        JISX0221_ASCII                = 0x0004,
        Sun_JDK117                     = 0x0005,
        Microsoft_CP932                = 0x0006,

        NEC_VDC                = 0x0100,                // NEC Vender Defined Char
        UDC                        = 0x0200,                // User Defined Char
        IBM_VDC                = 0x0400                // IBM Vender Defined Char
    };
    static QJpUnicodeConv *newConverter(int rule);

    virtual UINT asciiToUnicode(UINT h, UINT l) const;
    /*virtual*/ UINT jisx0201ToUnicode(UINT h, UINT l) const;
    virtual UINT jisx0201LatinToUnicode(UINT h, UINT l) const;
    /*virtual*/ UINT jisx0201KanaToUnicode(UINT h, UINT l) const;
    virtual UINT jisx0208ToUnicode(UINT h, UINT l) const;
    virtual UINT jisx0212ToUnicode(UINT h, UINT l) const;

    UINT asciiToUnicode(UINT ascii) const {
        return asciiToUnicode((ascii & 0xff00) >> 8, (ascii & 0x00ff));
    }
    UINT jisx0201ToUnicode(UINT jis) const {
        return jisx0201ToUnicode((jis & 0xff00) >> 8, (jis & 0x00ff));
    }
    UINT jisx0201LatinToUnicode(UINT jis) const {
        return jisx0201LatinToUnicode((jis & 0xff00) >> 8, (jis & 0x00ff));
    }
    UINT jisx0201KanaToUnicode(UINT jis) const {
        return jisx0201KanaToUnicode((jis & 0xff00) >> 8, (jis & 0x00ff));
    }
    UINT jisx0208ToUnicode(UINT jis) const {
        return jisx0208ToUnicode((jis & 0xff00) >> 8, (jis & 0x00ff));
    }
    UINT jisx0212ToUnicode(UINT jis) const {
        return jisx0212ToUnicode((jis & 0xff00) >> 8, (jis & 0x00ff));
    }

    virtual UINT unicodeToAscii(UINT h, UINT l) const;
    /*virtual*/ UINT unicodeToJisx0201(UINT h, UINT l) const;
    virtual UINT unicodeToJisx0201Latin(UINT h, UINT l) const;
    /*virtual*/ UINT unicodeToJisx0201Kana(UINT h, UINT l) const;
    virtual UINT unicodeToJisx0208(UINT h, UINT l) const;
    virtual UINT unicodeToJisx0212(UINT h, UINT l) const;

    UINT unicodeToAscii(UINT unicode) const {
        return unicodeToAscii((unicode & 0xff00) >> 8, (unicode & 0x00ff));
    }
    UINT unicodeToJisx0201(UINT unicode) const {
        return unicodeToJisx0201((unicode & 0xff00) >> 8, (unicode & 0x00ff));
    }
    UINT unicodeToJisx0201Latin(UINT unicode) const {
        return unicodeToJisx0201Latin((unicode & 0xff00) >> 8, (unicode & 0x00ff));
    }
    UINT unicodeToJisx0201Kana(UINT unicode) const {
        return unicodeToJisx0201Kana((unicode & 0xff00) >> 8, (unicode & 0x00ff));
    }
    UINT unicodeToJisx0208(UINT unicode) const {
        return unicodeToJisx0208((unicode & 0xff00) >> 8, (unicode & 0x00ff));
    }
    UINT unicodeToJisx0212(UINT unicode) const {
        return unicodeToJisx0212((unicode & 0xff00) >> 8, (unicode & 0x00ff));
    }

    UINT sjisToUnicode(UINT h, UINT l) const;
    UINT unicodeToSjis(UINT h, UINT l) const;

    UINT sjisToUnicode(UINT sjis) const {
        return sjisToUnicode((sjis & 0xff00) >> 8, (sjis & 0x00ff));
    }
    UINT unicodeToSjis(UINT unicode) const {
        return unicodeToSjis((unicode & 0xff00) >> 8, (unicode & 0x00ff));
    }

protected:
    explicit QJpUnicodeConv(int r) : rule(r) {}

private:
    int rule;
};

#endif




enum Rules {
	// "ASCII" is ANSI X.3.4-1986, a.k.a. US-ASCII here.
	    QJpUnicodeConv_Default                        = 0x0000,
		
        QJpUnicodeConv_Unicode                        = 0x0001,
        QJpUnicodeConv_Unicode_JISX0201                = 0x0001,
        QJpUnicodeConv_Unicode_ASCII                 = 0x0002,
        QJpUnicodeConv_JISX0221_JISX0201         = 0x0003,
        QJpUnicodeConv_JISX0221_ASCII                = 0x0004,
        QJpUnicodeConv_Sun_JDK117                     = 0x0005,
        QJpUnicodeConv_Microsoft_CP932                = 0x0006,
		
        QJpUnicodeConv_NEC_VDC                = 0x0100,                // NEC Vender Defined Char
        QJpUnicodeConv_UDC                        = 0x0200,                // User Defined Char
        QJpUnicodeConv_IBM_VDC                = 0x0400                // IBM Vender Defined Char
};


struct _QJpUnicodeConv_Drv;
struct _QJpUnicodeConv;

typedef VOID (*LPQJpUnicodeConv_Init)( int rule );
typedef VOID (*LPQJpUnicodeConv_Deinit)( struct _QJpUnicodeConv * );
typedef UINT (*LPMakeCode)( struct _QJpUnicodeConv *, UINT h, UINT l );

typedef struct _QJpUnicodeConv_Drv{
    LPQJpUnicodeConv_Init JpUnicodeConv_Init;
	LPQJpUnicodeConv_Deinit JpUnicodeConv_Deinit;
    //UINT asciiToUnicode( QJpUnicodeConv *, UINT h, UINT l );	
    //UINT jisx0201ToUnicode( QJpUnicodeConv *, UINT h, UINT l);
    //UINT jisx0201LatinToUnicode( QJpUnicodeConv *, UINT h, UINT l);
    //UINT jisx0201KanaToUnicode( QJpUnicodeConv *, UINT h, UINT l);
    //UINT jisx0208ToUnicode( QJpUnicodeConv *, UINT h, UINT l);
    //UINT jisx0212ToUnicode( QJpUnicodeConv *, UINT h, UINT l);
	LPMakeCode asciiToUnicode;
	LPMakeCode jisx0201ToUnicode;
	LPMakeCode jisx0201LatinToUnicode;
	LPMakeCode jisx0201KanaToUnicode;
	LPMakeCode jisx0208ToUnicode;
	LPMakeCode jisx0212ToUnicode;


    //UINT asciiToUnicode(UINT ascii) const {
        //return asciiToUnicode((ascii & 0xff00) >> 8, (ascii & 0x00ff));
    //}
    //UINT jisx0201ToUnicode(UINT jis) const {
      //  return jisx0201ToUnicode((jis & 0xff00) >> 8, (jis & 0x00ff));
    //}
    //UINT jisx0201LatinToUnicode(UINT jis) const {
      //  return jisx0201LatinToUnicode((jis & 0xff00) >> 8, (jis & 0x00ff));
    //}
    //UINT jisx0201KanaToUnicode(UINT jis) const {
      //  return jisx0201KanaToUnicode((jis & 0xff00) >> 8, (jis & 0x00ff));
    //}
    //UINT jisx0208ToUnicode(UINT jis) const {
      //  return jisx0208ToUnicode((jis & 0xff00) >> 8, (jis & 0x00ff));
    //}
    //UINT jisx0212ToUnicode(UINT jis) const {
      //  return jisx0212ToUnicode((jis & 0xff00) >> 8, (jis & 0x00ff));
    //}


    //UINT unicodeToAscii( QJpUnicodeConv *, UINT h, UINT l);// const;
    //UINT unicodeToJisx0201( QJpUnicodeConv *, UINT h, UINT l);// const;
    //UINT unicodeToJisx0201Latin( QJpUnicodeConv *, UINT h, UINT l);// const;
    //UINT unicodeToJisx0201Kana( QJpUnicodeConv *, UINT h, UINT l);// const;
    //UINT unicodeToJisx0208( QJpUnicodeConv *, UINT h, UINT l);// const;
    //UINT unicodeToJisx0212( QJpUnicodeConv *, UINT h, UINT l);// const;
	LPMakeCode unicodeToAscii;
	LPMakeCode unicodeToJisx0201;
	LPMakeCode unicodeToJisx0201Latin;
	LPMakeCode unicodeToJisx0201Kana;
	LPMakeCode unicodeToJisx0208;
	LPMakeCode unicodeToJisx0212;
	

//    UINT unicodeToAscii(UINT unicode) const {
  //      return unicodeToAscii((unicode & 0xff00) >> 8, (unicode & 0x00ff));
    //}
    //UINT unicodeToJisx0201(UINT unicode) const {
      //  return unicodeToJisx0201((unicode & 0xff00) >> 8, (unicode & 0x00ff));
    //}
    //UINT unicodeToJisx0201Latin(UINT unicode) const {
      //  return unicodeToJisx0201Latin((unicode & 0xff00) >> 8, (unicode & 0x00ff));
    //}
//    UINT unicodeToJisx0201Kana(UINT unicode) const {
  //      return unicodeToJisx0201Kana((unicode & 0xff00) >> 8, (unicode & 0x00ff));
    //}
    //UINT unicodeToJisx0208(UINT unicode) const {
      //  return unicodeToJisx0208((unicode & 0xff00) >> 8, (unicode & 0x00ff));
    //}
    //UINT unicodeToJisx0212(UINT unicode) const {
      //  return unicodeToJisx0212((unicode & 0xff00) >> 8, (unicode & 0x00ff));
    //}

    //UINT sjisToUnicode( QJpUnicodeConv *, UINT h, UINT l);
    //UINT unicodeToSjis( QJpUnicodeConv *, UINT h, UINT l);

	LPMakeCode sjisToUnicode;
	LPMakeCode unicodeToSjis;


    //UINT sjisToUnicode(UINT sjis) const {
      //  return sjisToUnicode((sjis & 0xff00) >> 8, (sjis & 0x00ff));
    //}
    //UINT unicodeToSjis(UINT unicode) const {
      //  return unicodeToSjis((unicode & 0xff00) >> 8, (unicode & 0x00ff));
    //}

//protected:
  //  explicit QJpUnicodeConv(int r) : rule(r) {}

//private:
    //int rule;
}QJpUnicodeConv_Drv;

typedef struct _QJpUnicodeConv
{
	int rule;
	QJpUnicodeConv_Drv * lpDrv;
} QJpUnicodeConv;


UINT QJpUnicodeConv_asciiToUnicode_hl( QJpUnicodeConv *, UINT h, UINT l);// const;
UINT QJpUnicodeConv_jisx0201ToUnicode_hl( QJpUnicodeConv *, UINT h, UINT l);// const;
UINT QJpUnicodeConv_jisx0201LatinToUnicode_hl( QJpUnicodeConv *, UINT h, UINT l);// const;
UINT QJpUnicodeConv_jisx0201KanaToUnicode_hl( QJpUnicodeConv *, UINT h, UINT l);// const;
UINT QJpUnicodeConv_jisx0208ToUnicode_hl( QJpUnicodeConv *, UINT h, UINT l);// const;
UINT QJpUnicodeConv_jisx0212ToUnicode_hl( QJpUnicodeConv *, UINT h, UINT l);// const;

//UINT QJpUnicodeConv_asciiToUnicode(UINT ascii);// const {
//        return asciiToUnicode((ascii & 0xff00) >> 8, (ascii & 0x00ff));
//    }
#define QJpUnicodeConv_asciiToUnicode( lpthis, ascii ) ( QJpUnicodeConv_asciiToUnicode_hl( (lpthis), ( (ascii) & 0xff00) >> 8, ( (ascii) & 0x00ff)) )
//UINT QJpUnicodeConv_jisx0201ToUnicode(UINT jis); // const {
//        return jisx0201ToUnicode((jis & 0xff00) >> 8, (jis & 0x00ff));
//    }
#define QJpUnicodeConv_jisx0201ToUnicode(lpthis, jis) (QJpUnicodeConv_jisx0201ToUnicode_hl( (lpthis), ( (jis) & 0xff00) >> 8, ( (jis) & 0x00ff)))

//UINT QJpUnicodeConv_jisx0201LatinToUnicode(UINT jis);// const {
//        return jisx0201LatinToUnicode((jis & 0xff00) >> 8, (jis & 0x00ff));
//    }
#define QJpUnicodeConv_jisx0201LatinToUnicode(lpthis, jis) (QJpUnicodeConv_jisx0201LatinToUnicode_hl( (lpthis), ( (jis) & 0xff00) >> 8, ( (jis) & 0x00ff)))

//UINT QJpUnicodeConv_jisx0201KanaToUnicode(UINT jis);// const {
//        return jisx0201KanaToUnicode((jis & 0xff00) >> 8, (jis & 0x00ff));
//    }
#define QJpUnicodeConv_jisx0201KanaToUnicode(lpthis, jis) (QJpUnicodeConv_jisx0201KanaToUnicode_hl( (lpthis), ( (jis) & 0xff00) >> 8, ( (jis) & 0x00ff)))
//UINT QJpUnicodeConv_jisx0208ToUnicode(UINT jis);// const {
//        return jisx0208ToUnicode((jis & 0xff00) >> 8, (jis & 0x00ff));
//    }
#define QJpUnicodeConv_jisx0208ToUnicode(lpthis, jis) ( QJpUnicodeConv_jisx0208ToUnicode_hl( (lpthis), ( (jis) & 0xff00) >> 8, ( (jis) & 0x00ff)) )
//UINT QJpUnicodeConv_jisx0212ToUnicode(UINT jis);// const {
//        return jisx0212ToUnicode((jis & 0xff00) >> 8, (jis & 0x00ff));
//    }
#define QJpUnicodeConv_jisx0212ToUnicode(lpthis, jis) ( QJpUnicodeConv_jisx0212ToUnicode_hl( (lpthis), ( (jis) & 0xff00) >> 8, ( (jis) & 0x00ff)) )
UINT QJpUnicodeConv_unicodeToAscii_hl(QJpUnicodeConv *,UINT h, UINT l);// const;
UINT QJpUnicodeConv_unicodeToJisx0201_hl(QJpUnicodeConv *,UINT h, UINT l);// const;
UINT QJpUnicodeConv_unicodeToJisx0201Latin_hl(QJpUnicodeConv *,UINT h, UINT l);// const;
UINT QJpUnicodeConv_unicodeToJisx0201Kana_hl(QJpUnicodeConv *,UINT h, UINT l);// const;
UINT QJpUnicodeConv_unicodeToJisx0208_hl(QJpUnicodeConv *,UINT h, UINT l);// const;
UINT QJpUnicodeConv_unicodeToJisx0212_hl(QJpUnicodeConv *,UINT h, UINT l);// const;

//UINT QJpUnicodeConv_unicodeToAscii(UINT unicode);// const {
//        return unicodeToAscii((unicode & 0xff00) >> 8, (unicode & 0x00ff));
//    }
#define QJpUnicodeConv_unicodeToAscii( lpthis, unicode ) (QJpUnicodeConv_unicodeToAscii_hl(( (unicode) & 0xff00) >> 8, ( (unicode) & 0x00ff)) )
//UINT QJpUnicodeConv_unicodeToJisx0201(UINT unicode);// const {
//        return unicodeToJisx0201((unicode & 0xff00) >> 8, (unicode & 0x00ff));
//    }
#define QJpUnicodeConv_unicodeToJisx0201( lpthis, unicode ) (QJpUnicodeConv_unicodeToJisx0201_hl(((unicode) & 0xff00) >> 8, ( (unicode) & 0x00ff)) )

//UINT QJpUnicodeConv_unicodeToJisx0201Latin(UINT unicode) const {
        //return unicodeToJisx0201Latin((unicode & 0xff00) >> 8, (unicode & 0x00ff));
    //}
#define QJpUnicodeConv_unicodeToJisx0201Latin( lpthis, unicode ) ( QJpUnicodeConv_unicodeToJisx0201Latin_hl((unicode & 0xff00) >> 8, (unicode & 0x00ff)) )

//UINT QJpUnicodeConv_unicodeToJisx0201Kana(UINT unicode) const {
        //return unicodeToJisx0201Kana((unicode & 0xff00) >> 8, (unicode & 0x00ff));
    //}
#define QJpUnicodeConv_unicodeToJisx0201Kana( lpthis, unicode ) ( QJpUnicodeConv_unicodeToJisx0201Kana_hl((unicode & 0xff00) >> 8, (unicode & 0x00ff)) )

//UINT QJpUnicodeConv_unicodeToJisx0208(UINT unicode) const {
        //return unicodeToJisx0208((unicode & 0xff00) >> 8, (unicode & 0x00ff));
    //}
#define QJpUnicodeConv_unicodeToJisx0208( lpthis, unicode ) ( QJpUnicodeConv_unicodeToJisx0208_hl(( (unicode) & 0xff00) >> 8, ( (unicode) & 0x00ff)))
//UINT QJpUnicodeConv_unicodeToJisx0212(UINT unicode) const {
  //      return unicodeToJisx0212((unicode & 0xff00) >> 8, (unicode & 0x00ff));
//    }
#define QJpUnicodeConv_unicodeToJisx0212( lpthis, unicode ) ( QJpUnicodeConv_unicodeToJisx0212_hl(( (unicode) & 0xff00) >> 8, ( (unicode) & 0x00ff)) );

UINT QJpUnicodeConv_sjisToUnicode_hl(QJpUnicodeConv *,UINT h, UINT l);// const;
UINT QJpUnicodeConv_unicodeToSjis_hl(QJpUnicodeConv *,UINT h, UINT l);// const;

//UINT QJpUnicodeConv_sjisToUnicode(UINT sjis) const {
        //return sjisToUnicode((sjis & 0xff00) >> 8, (sjis & 0x00ff));
    //}
#define QJpUnicodeConv_sjisToUnicode( lpthis, sjis ) ( QJpUnicodeConv_sjisToUnicode_hl( ( (sjis) & 0xff00) >> 8, ( (sjis) & 0x00ff) ) )

//UINT QJpUnicodeConv_unicodeToSjis(UINT unicode) const {
  //      return unicodeToSjis((unicode & 0xff00) >> 8, (unicode & 0x00ff));
    //}
#define QJpUnicodeConv_unicodeToSjis( lpthis, unicode ) ( QJpUnicodeConv_unicodeToSjis_hl(( (unicode) & 0xff00) >> 8, ( (unicode) & 0x00ff)) )




#endif // QJPUNICODE_H
