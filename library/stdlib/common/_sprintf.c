#include <edef.h>
#include <estring.h>
#include <estdarg.h>

#define	GET_DIGIT( c )	( (c) - '0' )
#define IS_DIGIT( c )   ( ( (unsigned)GET_DIGIT(c) ) <= 9 )
#define	TO_CHAR( n )    ( (n) + '0' )

// flags  
#define	LONGINT		0x01		// long int
#define	SHORTINT	0x02		// short int
#define	ALT		    0x04		// alternate form
#define	LEFTJUST	0x10		// left adjustment
#define	ZEROFILL	0x20		// zero fill
#define	HEXPREFIX	0x40		// add 0x or 0X prefix 


#define OCT 0
#define DEC 1
#define HEX 2

#define BUF 32

#define	SARG() \
	( flags & LONGINT ? va_arg(ap, long) : \
	    flags&SHORTINT ? (long)(short)va_arg(ap, int) : \
	    (long)va_arg(ap, int) )
#define	UARG() \
	( flags&LONGINT ? va_arg(ap, unsigned long) : \
	    flags&SHORTINT ? (unsigned long)(unsigned short)va_arg(ap, int) : \
	    (unsigned long)va_arg(ap, unsigned int) )



#ifdef QUICK_OUTPUT

#define out_byte( pbuf, c )    ( *(pbuf) = (char)(c), 1 )

#else

static int out_byte( char * pbuf, char c )
{
    *pbuf = c;
	return 1;
}

#endif   // QUICK_OUTPUT

static int out_string( char * pbuf, const char * s )
{
	int i = 0;
	if( s )	
	    while( *s ) { *pbuf++ = *s++; i++; };
	return i;
}

static int out_mem( char * pbuf, const char * s, int num )
{
	int i = 0;
	if( s )	
	{
	    i = num;
		while( num && *s ) { *pbuf++ = *s++; num--; };
	}
	return i;
}


static int out_pad( char * pbuf, int c, unsigned int num )
{
	int i = num;
	
	while( num ) { *pbuf++ = c; num--; };
	return i;
}


//int sprintf( fp, fmt0, ap )
     //_IO_FILE *fp;
     //char const *fmt0;
     //_IO_va_list ap;
//unsigned int std_sprintf( unsigned char *pbuf, const unsigned char *format, ... )

unsigned int _out_stream( 
						 char *outbuf,
						 const char *format, 
						 va_list ap )
{
	register const char *fmt; /* format string */
	register int ch;	/* character from fmt */
	register int n;		/* handy integer (short term usage) */
	register char *cp;	/* handy char pointer (short term usage) */
	const char *fmark;	/* for remembering a place in fmt */
	register int flags;	/* flags as above */
	int ret;		/* return value accumulator */
	int width;		/* width from format (%8d), or 0 */
	int prec;		/* precision from format (%.3d), or -1 */
	char sign;		/* sign prefix (' ', '+', '-', or \0) */
	int base;

	unsigned long _ulong;	/* integer arguments %[diouxX] */
	
	int dprec;		/* a copy of prec if [diouxX], 0 otherwise */
	int dpad;		/* extra 0 padding needed for integers */
	int fieldsz;		/* field size expanded by sign, dpad etc */
	/* The initialization of 'size' is to suppress a warning that
	   'size' might be used unitialized.  It seems gcc can't
	   quite grok this spaghetti code ... */
	int size = 0;		/* size of converted field or string */
	char buf[33];		/* space for %c, %[diouxX], %[eEfgG] */
	//char ox[2];		/* space for 0x hex-prefix */


	fmt = format;//fmt0;
	ret = 0;

	// Scan the format for conversions (`%' character).
	 
	while( 1 )
	{
		for( fmark = fmt; (ch = *fmt) != '\0' && ch != '%'; fmt++ )
			;
		if( ( n = fmt - fmark ) != 0 )
		{
			//PRINT( fmark, n );  // out_string
			outbuf += out_mem( outbuf, fmark, n );
			ret += n;
		}
		if( ch == '\0' )
			goto done;
		fmt++;		// skip over '%'

		flags = 0;
		dprec = 0;

		width = 0;
		prec = -1;
		sign = '\0';

rflag:		
		ch = *fmt++;
reswitch:	
		switch( ch )
		{
		case ' ':
			//**************************************************
			// ``If the space and + flags both appear, the space
			// flag will be ignored.''
			//	-- ANSI X3J11
			//*************************************************
			if ( !sign )
				sign = ' ';
			goto rflag;
		case '#':
			flags |= ALT;
			goto rflag;
		case '*':
			//***********************************************
			// ``A negative field width argument is taken as a
			// - flag followed by a positive field width.''
			//	-- ANSI X3J11
			// They don't exclude field widths read from args.
			//************************************************
			if( (width = va_arg(ap, int) ) >= 0 )
				goto rflag;
			width = -width;
			/* FALLTHROUGH */
		case '-':
			flags |= LEFTJUST;
			flags &= ~ZEROFILL; /* '-' disables '0' */
			goto rflag;
		case '+':
			sign = '+';
			goto rflag;
		case '.':
			if( (ch = *fmt++) == '*') 
			{
				n = va_arg(ap, int);
				prec = n < 0 ? -1 : n;
				goto rflag;
			}
			n = 0;
			while( IS_DIGIT(ch) )
			{
				n = 10 * n + GET_DIGIT(ch);
				ch = *fmt++;
			}
			prec = n < 0 ? -1 : n;
			goto reswitch;
		case '0':
			//*********************************************
			//  ``Note that 0 is taken as a flag, not as the
			// beginning of a field width.''
			//	-- ANSI X3J11
			//*********************************************
			if( !(flags & LEFTJUST) )
			    flags |= ZEROFILL; // '-' disables '0'
			goto rflag;
		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			n = 0;
			do 
			{
				n = 10 * n + GET_DIGIT(ch);
				ch = *fmt++;
			}while ( IS_DIGIT(ch) );
			width = n;
			goto reswitch;
		case 'h':
			flags |= SHORTINT;
			goto rflag;
		case 'l':
			flags |= LONGINT;
			goto rflag;
		case 'c':
			*( cp = buf ) = va_arg(ap, int);
			size = 1;
			sign = '\0';
			break;
		case 'D':
			flags |= LONGINT;
			//FALLTHROUGH
		case 'd':
		case 'i':
			// signed 
			_ulong = SARG();
			if( (long)_ulong < 0 )
			{
				_ulong = -(long)_ulong;
				sign = '-';
			}
			base = DEC;
			goto number;
		case 'n':
			if( flags & LONGINT )
				*va_arg( ap, long * ) = ret;
			else if( flags & SHORTINT )
				*va_arg( ap, short * ) = ret;
			else
				*va_arg( ap, int * ) = ret;
			continue;	// no output
		case 'O':
			flags |= LONGINT;
			//FALLTHROUGH
		case 'o':
			// unsigned 
			_ulong = UARG();

			base = OCT;
			goto nosign;
		case 'p':
			//************************************************
			// The argument shall be a pointer to void.  The
			// value of the pointer is converted to a sequence
			// of printable characters, in an implementation-
			// defined manner.
			//	-- ANSI X3J11
			//************************************************
			/* NOSTRICT */
			_ulong = (unsigned long)va_arg( ap, void * );
			base = HEX;
			flags |= HEXPREFIX;
			ch = 'x';
			goto nosign;
		case 's':
			if ( ( cp = va_arg( ap, char * ) ) == 0 )
				cp = "0";//"(null)";
			if( prec >= 0 )
			{
				char *p = (char*)memchr( cp, 0, prec );

				if( p != 0 )
				{
					size = p - cp;
					if( size > prec )
						size = prec;
				} else
					size = prec;
			}
			else
				size = strlen(cp);
			sign = '\0';
			break;
		case 'U':
			flags |= LONGINT;
			//FALLTHROUGH
		case 'u':
			// unsigned 
			_ulong = UARG();
			base = DEC;
			goto nosign;
		case 'X':
		case 'x':
			_ulong = UARG();
			base = HEX;
			// leading 0x/X only if non-zero
			if( (flags & ALT) && _ulong != 0 )
				flags |= HEXPREFIX;

			// unsigned conversions
nosign:			
			sign = '\0';
			//************************************************
			//  ``... diouXx conversions ... if a precision is
			// specified, the 0 flag will be ignored.''
			//	-- ANSI X3J11
			//************************************************
number:		
			if( (dprec = prec) >= 0 )
				flags &= ~ZEROFILL;
			//************************************************
			// ``The result of converting a zero value with an
			// explicit precision of zero is no characters.''
			//	-- ANSI X3J11
			//************************************************
			cp = buf + BUF;
			if (_ulong != 0 || prec != 0) 
			{
			    char *xdigs; // digits for [xX] conversion
				//
				// unsigned mod is hard, and unsigned mod
				// by a constant is easier than that by
				// a variable; hence this switch.
				//
				switch (base) 
				{
				case OCT:
					do {
						*--cp = TO_CHAR( (char)(_ulong & 7) );
						_ulong >>= 3;
					}while (_ulong);
					// handle octal leading 0
					if( (flags & ALT) && *cp != '0' )
						*--cp = '0';
					break;
				case DEC:
					// many numbers are 1 digit
					while( _ulong >= 10 ) 
					{
						*--cp = TO_CHAR( (char)(_ulong % 10) );
						_ulong /= 10;
					}
					*--cp = TO_CHAR( (char)_ulong);
					break;
				case HEX:
					if( ch == 'X' )
					    xdigs = "0123456789ABCDEF";
					else // ch == 'x' || ch == 'p'
					    xdigs = "0123456789abcdef";
					do 
					{
						*--cp = xdigs[_ulong & 15];
						_ulong >>= 4;
					} while( _ulong );
					break;
				default:
					cp = "";//"bug in vform: bad base";
					goto skipsize;
				}
			}
			size = buf + BUF - cp;
		skipsize:
			break;
		default:	// "%?" prints ?, unless ? is NUL
			if( ch == '\0' )
				goto done;
			// pretend it was %c with argument ch 
			cp = buf;
			*cp = ch;
			size = 1;
			sign = '\0';
			break;
		}

		/*
		 * All reasonable formats wind up here.  At this point,
		 * `cp' points to a string which (if not flags&LEFTJUST)
		 * should be padded out to `width' places.  If
		 * flags&ZEROFILL, it should first be prefixed by any
		 * sign or other prefix; otherwise, it should be blank
		 * padded before the prefix is emitted.  After any
		 * left-hand padding and prefixing, emit zeroes
		 * required by a decimal [diouxX] precision, then print
		 * the string proper, then emit zeroes required by any
		 * leftover floating precision; finally, if LEFTJUST,
		 * pad with blanks.
		 */

		/*
		 * compute actual size, so we know how much to pad.
		 */

		fieldsz = size;

		dpad = dprec - size;
		if (dpad < 0)
		    dpad = 0;

		if (sign)
			fieldsz++;
		else if (flags & HEXPREFIX)
			fieldsz += 2;
		fieldsz += dpad;

		// right-adjusting blank padding
		if( (flags & (LEFTJUST|ZEROFILL) ) == 0 && width > fieldsz )
			//PAD_SP(width - fieldsz);
			outbuf += out_pad( outbuf, ' ', width - fieldsz );

		// prefix
		if( sign )
		{
			//PRINT(&sign, 1);
			outbuf += out_byte( outbuf, sign );
		} 
		else if (flags & HEXPREFIX) 
		{
			//ox[0] = '0';
			//ox[1] = ch;
			//PRINT(ox, 2);
			outbuf += out_byte( outbuf, '0' );
			outbuf += out_byte( outbuf, (char)ch );
		}

		// right-adjusting zero padding 
		if ( (flags & (LEFTJUST|ZEROFILL)) == ZEROFILL && width > fieldsz )
			//PAD_0(width - fieldsz);
			outbuf += out_pad( outbuf, '0', width - fieldsz );


		// leading zeroes from decimal precision
		//PAD_0(dpad);
		// 2004-03-15 lilin -begin
		//out_pad( outbuf, '0', dpad ); 
		outbuf += out_pad( outbuf, '0', dpad ); 
		// -end


		// the string or number proper
		//PRINT(cp, size);
		outbuf += out_mem( outbuf, cp, size );

		// left-adjusting padding (always blank)
		if( (flags & LEFTJUST) && width > fieldsz )
			//PAD_SP(width - fieldsz);
			outbuf += out_pad( outbuf, ' ', width - fieldsz );

		// finally, adjust ret
		ret += width > fieldsz ? width : fieldsz;
	}
done:
	return ret;
//error:
//	return 0;
}


unsigned int std_sprintf( char *pbuf, const char *format, ... )
{
	int i;
	va_list         vl;
	
	va_start(vl, format);
	i = _out_stream( pbuf, format, vl );
	pbuf[i] = 0;
	return i;	
}

unsigned int std_vsprintf( char *pbuf, const char *format, va_list argptr )
{
	int i;
	i = _out_stream( pbuf, format, argptr );
	pbuf[i] = 0;
	return i;
}
