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

#include "qfontjpcodec.h"

#include "qjpunicode.h"

#ifdef Q_WS_X11
// JIS X 0201

QFontJis0201Codec::QFontJis0201Codec()
{
}

QByteArray QFontJis0201Codec::_name()
{
    return "jisx0201*-0";
}

int QFontJis0201Codec::_mibEnum()
{
    return 15;
}

QByteArray QFontJis0201Codec::convertFromUnicode(const QChar *uc, int len,  ConverterState *) const
{
    QByteArray rstring;
    rstring.resize(len);
    uchar *rdata = (uchar *) rstring.data();
    const QChar *sdata = uc;
    int i = 0;
    for (; i < len; ++i, ++sdata, ++rdata) {
        if (sdata->unicode() < 0x80) {
            *rdata = (uchar) sdata->unicode();
        } else if (sdata->unicode() >= 0xff61 && sdata->unicode() <= 0xff9f) {
            *rdata = (uchar) (sdata->unicode() - 0xff61 + 0xa1);
        } else {
            *rdata = 0;
        }
    }
    return rstring;
}

QString QFontJis0201Codec::convertToUnicode(const char *, int,  ConverterState *) const
{
    return QString();
}

// JIS X 0208

QFontJis0208Codec::QFontJis0208Codec()
{
    convJP = QJpUnicodeConv::newConverter(QJpUnicodeConv::Default);
}


QFontJis0208Codec::~QFontJis0208Codec()
{
    delete convJP;
    convJP = 0;
}


QByteArray QFontJis0208Codec::_name()
{
    return "jisx0208*-0";
}


int QFontJis0208Codec::_mibEnum()
{
    return 63;
}


QString QFontJis0208Codec::convertToUnicode(const char* /*chars*/, int /*len*/, ConverterState *) const
{
    return QString();
}

QByteArray QFontJis0208Codec::convertFromUnicode(const QChar *uc, int len, ConverterState *) const
{
    QByteArray result;
    result.resize(len * 2);
    uchar *rdata = (uchar *) result.data();
    const QChar *ucp = uc;

    for (int i = 0; i < len; i++) {
        QChar ch(*ucp++);
        ch = convJP->unicodeToJisx0208(ch.unicode());

        if (!ch.isNull()) {
            *rdata++ = ch.row();
            *rdata++ = ch.cell();
        } else {
            *rdata++ = 0;
            *rdata++ = 0;
        }
    }
    return result;
}

#endif
