#ifndef __SYMBOLS_H
#define __SYMBOLS_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus


#define SYM_CLOSE               33
#define SYM_DOWN                34
#define SYM_DOWNARROW           35
#define SYM_FIND                36
#define SYM_HELP                37
#define SYM_LEFT                38
#define SYM_LEFTARROW           39
#define SYM_OK                  40
#define SYM_RIGHT               41
#define SYM_RIGHTARROW          42
#define SYM_UP                  43
#define SYM_UPARROW             44
#define SYM_CHECK_SET           45
#define SYM_CHECK_NOSET         46
#define SYM_RADIO_SET           47
#define SYM_RADIO_NOSET         48
#define SYM_DEL                 49
#define SYM_DISKETTE            50
#define SYM_KEYBOARD            51
#define SYM_NEWDOC              52
#define SYM_SCROLL_HTHUMB       53
#define SYM_SCROLL_VTHUMB       54

// the symbol font 
extern const HFONT __hSymbol16x16;

// when you use the symbol font , you must do:
// BYTE bSymText;
// HFONT hfont = SelectObject( hdc, __hSymbol16x16 );

// bSymText = SYM_OK;
// use text option to draw text
// TextOut( hdc, 0, 0, &bSymText, 1 );

// SelectObject( hdc, hfont );

#ifdef __cplusplus
}
#endif  // __cplusplus


#endif    //__SYMBOLS_H
