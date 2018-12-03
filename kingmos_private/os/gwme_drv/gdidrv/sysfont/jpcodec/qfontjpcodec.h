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

#ifndef QFONTJPCODEC_H
#define QFONTJPCODEC_H

#include "qtextcodec.h"
#include "qlist.h"
class QJpUnicodeConv;

#ifdef Q_WS_X11
class QFontJis0201Codec : public QTextCodec
{
public:
    QFontJis0201Codec();

    static QByteArray _name();
    static QList<QByteArray> _aliases() { return QList<QByteArray>(); }
    static int _mibEnum();

    QByteArray name() const { return _name(); }
    QList<QByteArray> aliases() const { return _aliases(); }
    int mibEnum() const { return _mibEnum(); }

    QByteArray convertFromUnicode(const QChar *uc, int len,  ConverterState *) const;
    QString convertToUnicode(const char*, int,  ConverterState *) const;
};

class QFontJis0208Codec : public QTextCodec
{
public:
    QFontJis0208Codec();
    ~QFontJis0208Codec();

    static QByteArray _name();
    static QList<QByteArray> _aliases() { return QList<QByteArray>(); }
    static int _mibEnum();

    QByteArray name() const { return _name(); }
    QList<QByteArray> aliases() const { return _aliases(); }
    int mibEnum() const { return _mibEnum(); }

    QString convertToUnicode(const char* /*chars*/, int /*len*/, ConverterState *) const;
    QByteArray convertFromUnicode(const QChar *uc, int len, ConverterState *) const;
private:
    QJpUnicodeConv *convJP;
};
#endif

#endif // QFONTJPCODEC_H
