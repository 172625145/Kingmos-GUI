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

#include <qtextcodecplugin.h>
#include <qtextcodec.h>
#include <qstringlist.h>

#include "qeucjpcodec.h"
#include "qjiscodec.h"
#include "qsjiscodec.h"
#ifdef Q_WS_X11
#include "qfontjpcodec.h"
#endif

class JPTextCodecs : public QTextCodecPlugin
{
public:
    JPTextCodecs() {}

    QList<QByteArray> names() const;
    QList<QByteArray> aliases() const;
    QList<int> mibEnums() const;

    QTextCodec *createForMib(int);
    QTextCodec *createForName(const QByteArray &);
};

QList<QByteArray> JPTextCodecs::names() const
{
    QList<QByteArray> list;
    list += QEucJpCodec::_name();
    list += QJisCodec::_name();
    list += QSjisCodec::_name();
#ifdef Q_WS_X11
    list += QFontJis0201Codec::_name();
    list += QFontJis0208Codec::_name();
#endif
    return list;
}

QList<QByteArray> JPTextCodecs::aliases() const
{
    QList<QByteArray> list;
    list += QEucJpCodec::_aliases();
    list += QJisCodec::_aliases();
    list += QSjisCodec::_aliases();
#ifdef Q_WS_X11
    list += QFontJis0201Codec::_aliases();
    list += QFontJis0208Codec::_aliases();
#endif
    return list;
}

QList<int> JPTextCodecs::mibEnums() const
{
    QList<int> list;
    list += QEucJpCodec::_mibEnum();
    list += QJisCodec::_mibEnum();
    list += QSjisCodec::_mibEnum();
#ifdef Q_WS_X11
    list += QFontJis0201Codec::_mibEnum();
    list += QFontJis0208Codec::_mibEnum();
#endif
    return list;
}

QTextCodec *JPTextCodecs::createForMib(int mib)
{
    if (mib == QEucJpCodec::_mibEnum())
        return new QEucJpCodec;
    if (mib == QJisCodec::_mibEnum())
        return new QJisCodec;
    if (mib == QSjisCodec::_mibEnum())
        return new QSjisCodec;
#ifdef Q_WS_X11
    if (mib == QFontJis0208Codec::_mibEnum())
        return new QFontJis0208Codec;
    if (mib == QFontJis0201Codec::_mibEnum())
        return new QFontJis0201Codec;
#endif
    return 0;
}


QTextCodec *JPTextCodecs::createForName(const QByteArray &name)
{
    if (name == QEucJpCodec::_name() || QEucJpCodec::_aliases().contains(name))
        return new QEucJpCodec;
    if (name == QJisCodec::_name() || QJisCodec::_aliases().contains(name))
        return new QJisCodec;
    if (name == QSjisCodec::_name() || QSjisCodec::_aliases().contains(name))
        return new QSjisCodec;
#ifdef Q_WS_X11
    if (name == QFontJis0208Codec::_name() || QFontJis0208Codec::_aliases().contains(name))
        return new QFontJis0208Codec;
    if (name == QFontJis0201Codec::_name() || QFontJis0201Codec::_aliases().contains(name))
        return new QFontJis0201Codec;
#endif
    return 0;
}


Q_EXPORT_PLUGIN(JPTextCodecs);
