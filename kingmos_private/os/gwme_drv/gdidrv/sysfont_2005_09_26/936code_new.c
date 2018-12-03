#include <edef.h>


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

