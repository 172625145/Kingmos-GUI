#include <edef.h>

#include <unicode.h>

typedef struct {
    BYTE        tblBegin;
    BYTE        tblEnd;
    WORD        tblOffset;
    WORD        algOffset;
}INDEXTBL;

#include "936code_t.h"

#define IN_RANGE(c, lower, upper)  (((c) >= (lower)) && ((c) <= (upper)))
#define IS_LATIN(c)        ((c) <= 0x7F)
#define IsByteInGb2312(c)        (IN_RANGE((c), 0xA1, 0xFE))
#define Is1stByte(c)        (IN_RANGE((c), 0x81, 0xFE))
#define Is2ndByteIn2Bytes(c)        (IN_RANGE((c), 0x40, 0xFE) && (c) != 0x7F)
#define Is2ndByteIn4Bytes(c)        (IN_RANGE((c), 0x30, 0x39))
#define Is2ndByte(c)        (Is2ndByteIn2Bytes(c) || Is2ndByteIn4Bytes(c))
#define Is3rdByte(c)        (IN_RANGE((c), 0x81, 0xFE))
#define Is4thByte(c)        (IN_RANGE((c), 0x30, 0x39))

#define IsUDA1(a, b)        (IN_RANGE((a), 0xAA, 0xAF) && IN_RANGE((b), 0xA1, 0xFE))
#define IsUDA2(a, b)        (IN_RANGE((a), 0xF8, 0xFE) && IN_RANGE((b), 0xA1, 0xFE))
#define IsUDA3(a, b)        (IN_RANGE((a), 0xA1, 0xA7) && IN_RANGE((b), 0x40, 0xA0) && ((b) != 0x7F))
#define GET_LATIN1CHAR( c ) (c)

#define   GetValidChar(u)        ((u) ? ((USHORT)(u)) : (BYTE)('?'))


UINT Gb18030ToUnicode(const UCHAR *gbstr, int * lpLen) 
{
    /* Returns Unicode. */
    UINT    uni;
    UCHAR   first = *gbstr;

    if (IS_LATIN(first)) {
        *lpLen = 1;
        uni = (UINT)first;
    }
    else if (Is1stByte(first) && *lpLen >= 2) {
        UCHAR        second = gbstr[1];

        if (Is2ndByteIn2Bytes(second)) {
            *lpLen = 2;

            if (IsUDA1(first, second))
                uni = 0xE000 + (first - 0xAA) * 94 + (second - 0xA1);
            else if (IsUDA2(first, second))
                uni = 0xE234 + (first - 0xF8) * 94 + (second - 0xA1);
            else if (IsUDA3(first, second))
                uni = 0xE4C6 + (first - 0xA1) * 96 + (second - 0x40)
                             - ((second >= 0x80) ? 1 : 0);
            else {
                // Use the mapping table
                UINT i;

                i = (first - 0x81) * 190 + (second - 0x40)
                                         - ((second >= 0x80) ? 1 : 0);

                if (IN_RANGE(first, 0xA1, 0xA7))
                    i -= (first - 0xA0) * 96;
                if (first > 0xA7)
                    i -= 672;
                if (IN_RANGE(first, 0xAA, 0xAF))
                    i -= (first - 0xAA) * 94;
                if (first > 0xAF)
                    i -= 564;
                if (first >= 0xF8)
                    i -= (first - 0xF8) * 94;

                uni = (UINT)gb18030_2byte_to_ucs[i];
            }
        }
        else if (Is2ndByteIn4Bytes(second) && *lpLen >= 4) {
            UCHAR   third  = gbstr[2],
                    fourth = gbstr[3];

            if (Is3rdByte(third) && Is4thByte(fourth)) {
                // Valid 4-byte GB18030, whether defined or not
                UINT            gb4lin;
                INDEXTBL  g2u;

                gb4lin = (first - 0x81) * 12600 + (second - 0x30) * 1260
                          + (third - 0x81) * 10 + (fourth - 0x30);

                *lpLen = 4;
                if (gb4lin <= 0x99FB) {
                    /* GB+81308130 - GB+8431A439 */
                    g2u = gb18030_to_ucs_index[gb4lin >> 8];

                    if ((BYTE)(gb4lin & 0xFF) >= g2u.tblBegin &&
                        (BYTE)(gb4lin & 0xFF) <= g2u.tblEnd) {

                        uni = (UINT)gb18030_4byte_to_ucs[gb4lin - g2u.tblOffset];
                    }
                    else {
                        uni = g2u.algOffset + (gb4lin & 0xFF);
                    }
                } else if (IN_RANGE(gb4lin, 0x2E248, 0x12E247)) {
                    /* GB+90308130 - GB+E3329A35 */
                    uni = gb4lin - 0xE248;
                } else {
                    /* undefined or reserved area */
                    *lpLen = 1;
                    uni = '?';//replacement character;
                }
            }
            else {
                *lpLen = 1;
                uni = '?';//replacement character;
            }
        }
        else {
            *lpLen = 1;
            uni = '?';//replacement character;
        }
    }
    else {
        *lpLen = 1;
        uni = '?';//replacement character;
    }
    return uni;
}

static DWORD GetUnicode( DWORD dwHandle,
						 LPCSTR chars,
						 int len,  // if 0, get size need
						 LPWSTR lpWideCharStr,
						 int cchWideChar )
{
//UINT QGb18030Codec_ConvertToUnicode( const char* chars, int len, WORD * lpwBuf, int wBufLen )
//{
    UCHAR buf[4];
    int nbuf = 0;
    BYTE replacement = '?';//QChar::ReplacementCharacter;
	/*
    if (state) {
        if (state->flags & ConvertInvalidToNull)
            replacement = QChar::Null;
        nbuf = state->remainingChars;
        buf[0] = (state->state_data[0] >> 24) & 0xff;
        buf[1] = (state->state_data[0] >> 16) & 0xff;
        buf[2] = (state->state_data[0] >>  8) & 0xff;
        buf[3] = (state->state_data[0] >>  0) & 0xff;
    }
	*/
    int invalid = 0;
	int i, wCharSize;


    //QString result;
    //qDebug("QGb18030Decoder::toUnicode(const char* chars, int len = %d)", len);
	wCharSize = cchWideChar;
    for ( i = 0; i < len && wCharSize; i++) {
        BYTE ch = chars[i];
        switch (nbuf) {
        case 0:
            if (ch < 0x80) {
                // ASCII
                //result += GET_LATIN1CHAR(ch);
				if( lpWideCharStr )
					*lpWideCharStr++ = GET_LATIN1CHAR(ch);
				wCharSize--;
            } else if (Is1stByte(ch)) {
                // GB18030?
                buf[0] = ch;
                nbuf = 1;
            } else {
                // Invalid
                //result += replacement;
				if( lpWideCharStr )
					*lpWideCharStr++ = replacement;
				wCharSize--;
                ++invalid;
            }
            break;
        case 1:
            // GB18030 2 bytes
            if (Is2ndByteIn2Bytes(ch)) {
                int clen = 2;
				UINT u;
				buf[1] = ch;
                
                u = Gb18030ToUnicode(buf, &clen);
                if (clen == 2) {
                    //result += GetValidChar(u);
					if( lpWideCharStr )
						*lpWideCharStr++ = GetValidChar(u);
					wCharSize--;
                } else {
                    //result += replacement;
					if( lpWideCharStr )
						*lpWideCharStr++ = replacement;
					wCharSize--;
                    ++invalid;
                }
                nbuf = 0;
            } else if (Is2ndByteIn4Bytes(ch)) {
                buf[1] = ch;
                nbuf = 2;
            } else {
                // Error
                //result += replacement;
				if( lpWideCharStr )
					*lpWideCharStr++ = replacement;
				wCharSize--;
                ++invalid;
                nbuf = 0;
            }
            break;
        case 2:
            // GB18030 3 bytes
            if (Is3rdByte(ch)) {
                buf[2] = ch;
                nbuf = 3;
            } else {
                //result += replacement;
				if( lpWideCharStr )
					*lpWideCharStr++ = replacement;
				wCharSize--;

                ++invalid;
                nbuf = 0;
            }
            break;
        case 3:
            // GB18030 4 bytes
            if (Is4thByte(ch)) {
				int clen;
				UINT u;
                
				buf[3] = ch;
                clen = 4;
                u = Gb18030ToUnicode(buf, &clen);
                if (clen == 4) {
                    //result += GetValidChar(u);
					if( lpWideCharStr )
						*lpWideCharStr++ = GetValidChar(u);
					wCharSize--;
                } else {
                    //result += replacement;
					if( lpWideCharStr )
						*lpWideCharStr++ = replacement;
					wCharSize--;
                    ++invalid;
                }
            } else {
                //result += replacement;
				if( lpWideCharStr )
					*lpWideCharStr++ = replacement;
				wCharSize--;
                ++invalid;
            }
            nbuf = 0;
            break;
        }
    }
	/*
    if (state) {
        state->remainingChars = nbuf;
        state->state_data[0] = (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
        state->invalidChars += invalid;
    }
	*/
    return cchWideChar - wCharSize;//result;
}
/*
typedef struct {
    BYTE        tblBegin;
    BYTE        tblEnd;
    WORD        tblOffset;
    WORD        algOffset;
} indexTbl_t;
*/
static UINT gb4lin_to_gb(UINT gb4lin) 
{
    BYTE   a, b, c, d;
    a = 0x81 + gb4lin / 12600;
    b = 0x30 + (gb4lin / 1260) % 10;
    c = 0x81 + (gb4lin / 10) % 126;
    d = 0x30 + gb4lin % 10;
    return ((a << 24) | (b << 16) | (c << 8) | d);
}

static int UnicodeToGb18030( UINT uni, UCHAR *gbchar) 
{
    /* Returns the bytesize of the GB18030 character. */
    UINT        gb, gb4lin;
    INDEXTBL        u2g;

    if (IS_LATIN(uni)) {
        *gbchar = (UCHAR)uni;
        return 1;
    }
    else if (uni <= 0xD7FF || IN_RANGE(uni, 0xE766, 0xFFFF)) {
        u2g = ucs_to_gb18030_index[uni >> 8];

        if ((BYTE)(uni & 0xFF) >= u2g.tblBegin && (BYTE)(uni & 0xFF) <= u2g.tblEnd) {
            // Use mapping table (2-byte or 4-byte GB18030)
            UINT tblEntry;

            tblEntry = ucs_to_gb18030[uni - u2g.tblOffset];

            if (tblEntry > 0x8000) {
                // 2-byte GB18030
                gb = tblEntry;
            }
            else {
                // 4-byte GB18030 stored in a special compact format
                UCHAR        a, b;
                a = 0x81;
                b = 0x30 + (tblEntry >> 11);
                if (tblEntry >= 0x7000) {
                    a += 3;
                    b -= 14;
                } else if (tblEntry >= 0x6000) {
                    a += 2;
                    b -= 6;
                } else if (tblEntry >= 0x3000) {
                    a += 1;
                    b -= 6;
                } else if (b >= 0x31) {
                    b += 5;
                }
                gbchar[0] = a;
                gbchar[1] = b;
                gbchar[2] = 0x81 + ((tblEntry >> 4) & 0x7F);
                gbchar[3] = 0x30 + (tblEntry & 0xF);
                return 4;
            }
        }
        else {
            // 4-byte GB18030 calculated algorithmically
            gb4lin = u2g.algOffset + (uni & 0xFF);
            // Yikes, my index table could not cover all the bases...
            if (IN_RANGE(uni, 0x49B8, 0x49FF))
                gb4lin -= 11;
            gb = gb4lin_to_gb(gb4lin);
        }
    }
    else if (IN_RANGE(uni, 0xE000, 0xE765)) {
        // User-defined areas in GB18030 (2-byte)
        if (uni <= 0xE233)
            gb = 0xAAA1 + (((uni - 0xE000) / 94) << 8) + (uni - 0xE000) % 94;
        else if (uni <= 0xE4C5)
            gb = 0xF8A1 + (((uni - 0xE234) / 94) << 8) + (uni - 0xE234) % 94;
        else {
            gb = 0xA140 + (((uni - 0xE4C6) / 96) << 8) + (uni - 0xE4C6) % 96;
            // Skip the gap at 0x7F
            if ((gb & 0xFF) >= 0x7F)
                gb++;
        }
    }
    else if (IN_RANGE(uni, 0x10000, 0x10FFFF)) {
        // Qt 3.x does not support beyond BMP yet, but what the heck...
        // (U+10000 = GB+90308130) to (U+10FFFF = GB+E3329A35)
        gb = gb4lin_to_gb(0x1E248 + uni);
    }
    else {
        // Surrogate area and other undefined/reserved areas (discard)
        *gbchar = 0;
        return 0;
    }

    if (gb <= 0xFFFF) {
        gbchar[0] = (UCHAR)((gb >> 8) & 0xFF);
        gbchar[1] = (UCHAR)(gb & 0xFF);
        return 2;
    } else {
        gbchar[0] = (UCHAR)((gb >> 24) & 0xFF);
        gbchar[1] = (UCHAR)((gb >> 16) & 0xFF);
        gbchar[2] = (UCHAR)((gb >> 8) & 0xFF);
        gbchar[3] = (UCHAR)(gb & 0xFF);
        return 4;
    }
}


//QByteArray QGb18030Codec::convertFromUnicode(const QChar *uc, int len, ConverterState *state) const
static int GetChar( DWORD dwHandle,
				    LPCWSTR lpWideCharStr, 
					int cchWideChar, 
					LPSTR lpMultiByteStr, 
					int cbMultiByte, 
					LPCSTR lpDefaultChar, 
					BOOL * lpUsedDefaultChar )
{
    char replacement = '?';
    int high = -1;
	int nCharBytes = cbMultiByte;
	/*
    if (state) {
        if (state->flags & ConvertInvalidToNull)
            replacement = 0;
        if (state->remainingChars)
            high = state->state_data[0];
    }
	*/
    int invalid = 0;
	int i;

    //int rlen = 4 *len + 1;
    //QByteArray rstr;
    //rstr.resize(rlen);
    //UCHAR* cursor = (UCHAR*)rstr.data();
	UCHAR * cursor = lpMultiByteStr;
	int lenWideChar = cchWideChar;

    
    for (i = 0; i < lenWideChar && nCharBytes; i++) {
        WORD ch = *lpWideCharStr++;//uc[i].unicode();
        int len;
        UCHAR buf[4];
        if (high >= 0) {
            if (ch >= 0xdc00 && ch < 0xe000) {
                // valid surrogate pair
				UINT u;
                ++i;
                u = (high-0xd800)*0x400+(ch-0xdc00)+0x10000;
                len = UnicodeToGb18030(u, buf);
				if( cursor )
				{
					if (len >= 2) {
						int j;
						for (j=0; j<len && nCharBytes; j++, nCharBytes--)
							*cursor++ = buf[j];
					} else {
						*cursor++ = replacement;
						nCharBytes--;
						++invalid;
					}
				}
				else
				{
					nCharBytes-=len;
				}
                high = -1;
                continue;
            } else {
				if( cursor )
				{
					*cursor++ = replacement;					
					++invalid;
					nCharBytes--;
				}
				else
				{
					nCharBytes--;
				}
				
                high = -1;
            }
        }

        if (ch < 0x80) {
            // ASCII
			if( cursor )
			{
				*cursor++ = (BYTE)ch;
				nCharBytes--;
			}
			else
			{
				nCharBytes--;
			}
        } else if ((ch >= 0xd800 && ch < 0xdc00)) {
            // surrogates area. check for correct encoding
            // we need at least one more character, first the high surrogate, then the low one
            high = ch;
        } else if ((len = UnicodeToGb18030(ch, buf)) >= 2) {
			int j;
			if( cursor )
			{
				for (j=0; j<len && nCharBytes; j++,nCharBytes--)
					*cursor++ = buf[j];
			}
			else
			{
				nCharBytes-=len;
			}
        } else {
            // Error
			if( cursor )
			{
				*cursor++ = replacement;
				nCharBytes--;
			}
			else
			{
				nCharBytes--;
			}
            ++invalid;
        }
    }
	/*
    rstr.resize(cursor - (UCHAR*)rstr.constData());

    if (state) {
        state->invalidChars += invalid;
        state->state_data[0] = high;
        if (high)
            state->remainingChars = 1;
    }
	*/
    return cbMultiByte - nCharBytes;
}


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

const CODE_PAGE_DRV CP_936Drv = 
{
	Create,
	Delete,
	GetUnicode,
	GetChar,
	CountBufferSize
};


static DWORD Create( DWORD dwReserver )
{
	return (DWORD)1;
}

static BOOL Delete( DWORD dwHandle )
{
	return TRUE;
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
		iCount = GetChar( dwHandle, lpStr, cbCountLen, NULL, MAXLONG, NULL, NULL );
	}
	else
	{   // multibytes
		iCount = GetUnicode( dwHandle, lpStr, cbCountLen, NULL, MAXLONG );
	}
	return iCount;
}

