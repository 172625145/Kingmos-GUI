#include <eframe.h>
//typedef unsigned SIZE_T;

#define TOUPPER( c ) ( ( (c) >= 'a' && (c) <= 'z' ) ? ((c) - 'a' + 'A') : (c) )
#define TOLOWER( c ) ( ( (c) >= 'A' && (c) <= 'Z' ) ? ((c) - 'A' + 'a') : (c) )

int str_icmp( const char *string1, const char *string2 )
{
	register const unsigned char * p1 = (const unsigned char *)string1;
	register const unsigned char * p2 = (const unsigned char *)string2;

	while( 1 )
	{
		register unsigned char s1 = *p1++;
		register unsigned char s2 = *p2++;
		s1 = TOUPPER(s1);
		s2 = TOUPPER(s2);
		if(  s1 > s2 )
			return 1;
		else if( s1 < s2 )
			return -1;
		else if( s1 == 0 && s2 == 0 )
			break;
	}
	return 0;
}

int str_nicmp( const char *string1, const char *string2, SIZE_T count )
{
	register const unsigned char * p1 = (const unsigned char *)string1;
	register const unsigned char * p2 = (const unsigned char *)string2;

	while( count )
	{
		register unsigned char s1 = *p1++;
		register unsigned char s2 = *p2++;
		count--;
		s1 = TOUPPER(s1);
		s2 = TOUPPER(s2);
		if(  s1 > s2 )
			return 1;
		else if( s1 < s2 )
			return -1;
		else if( s1 == 0 && s2 == 0 )
			break;
	}
	return 0;
}

int str_ncmp( const char *string1, const char *string2, SIZE_T count )
{
	register const unsigned char * p1 = (const unsigned char *)string1;
	register const unsigned char * p2 = (const unsigned char *)string2;

	while( count )
	{
		register unsigned char s1 = *p1++;
		register unsigned char s2 = *p2++;

		count--;
		if(  s1 > s2 )
			return 1;
		else if( s1 < s2 )
			return -1;
		else if( s1 == 0 && s2 == 0 )
			break;
	}
	return 0;
}

int str_cmp( const char *string1, const char *string2 )
{
	register const unsigned char * p1 = (const unsigned char *)string1;
	register const unsigned char * p2 = (const unsigned char *)string2;

	while( 1 )
	{
		register unsigned char s1 = *p1++;
		register unsigned char s2 = *p2++;

		if(  s1 > s2 )
			return 1;
		else if( s1 < s2 )
			return -1;
		else if( s1 == 0 && s2 == 0 )
			break;
	}
	return 0;
}


char *str_cpy( char *strDest, const char *strSource )
{
	register char * pd = strDest;
	register const char * ps = strSource;

	while( *ps )
	{
		*pd++ = *ps++;
	}
	*pd = 0;
	return strDest;
}

char *str_ncpy( char *strDest, const char *strSource, SIZE_T count )
{
	register char * pd = strDest;
	register const char * ps = strSource;
	while( count )
	{
		if( *ps )
		{
			*pd++ = *ps++;
			count--;
		}
		else
			break;
	}
	if( count )
	    *pd = 0;
	return strDest;
}

char *str_cat( char *strDest, const char *strSource )
{
	register char * pd = strDest;
	register const char * ps = strSource;

	while( *pd )pd++;

	while( *ps )
	{
		*pd++ = *ps++;
	}
	*pd = 0;
	return strDest;
}

char * str_ncat( char * strDest, const char * strSource, SIZE_T count )
{
	char *start = strDest;
	
	while (*strDest++)
		;
	strDest--;
	
	while (count--)
		if (!(*strDest++ = *strSource++))
			return(start);
		
    *strDest = '\0';
    return(start);
}

SIZE_T str_len( const char *string )
{
	register const char * pd = string;
	register SIZE_T count = 0;
	
	while( *pd++ )count++;

	return count;
}


void *mem_cpy( void *dest, const void *src, SIZE_T count )
{
	register char * pd = (char*)dest;
	register const char * ps = (const char*)src;

	while( count )
	{
		*pd++ = *ps++;
		count--;
	}
	return dest;
}

void *mem_move( void *dest, const void *src, SIZE_T count )
{
	register char * pd = (char*)dest;
	register const char * ps = (const char*)src;
	SIZE_T m = count;

	if( ps > pd )
	{
		while( count )
		{
			*pd++ = *ps++;
			count--;
		}
	}
	else if( ps < pd )
	{
		ps += (count-1);
		pd += (count-1);
		while( count )
		{
			*pd-- = *ps--;
			count--;
		}
	}
	return dest;
}

void *mem_set( void *dest, int c, SIZE_T count )
{
	register char * pd = (char*)dest;
	while( count )
	{
		*pd++ = c; count--;
	}
	return dest;
}

int mem_cmp( const void *buf1, const void *buf2, SIZE_T count )
{
	register const unsigned char * p1 = (const unsigned char*)buf1;
	register const unsigned char * p2 = (const unsigned char*)buf2;
	
	while( count )
	{
		if( *p1 > *p2 )
			return 1;
		else if( *p1 < *p2 )
			return -1;
		p1++; p2++;
		count--;
	}
	return 0;
}

void *mem_chr( const void *buf, int c, SIZE_T count )
{
	register const char * lpbuf = buf;
	register int v = c;
	register SIZE_T n = count;

	while( n && *lpbuf != v  )
	{
		lpbuf++; n--;
	}
	if( *lpbuf == v )
		return (void *)lpbuf;
	else
		return 0;
}
/*
char std_toupper( char c )
{
	return TOUPPER( c );
}

char std_tolower( char c )
{
	return TOLOWER( c );
}
*/

char *str_chr( const char *s1, int i )
{
	const unsigned char *s = (const unsigned char *)s1;
	unsigned char c = (unsigned int)i;
	
	while ( *s && *s != c )
    {
		s++;
    }
	
	if (*s != c)
    {
		s = 0;
    }
	
	return (char *) s;
}

char * str_rchr( const char *s, int i )
{
	const char *last = 0;
	
	if( i )
    {
		while (s=str_chr(s, i))
		{
			last = s;
			s++;
		}
    }
	else
    {
		last = str_chr(s, i);
    }
		  
	return (char *) last;
}

char * str_strstr( const char * str1, const char * str2 )
{
	const char *s1, *s2;
	const char * cp = (const char *)str1;
	
	if ( !*str2 )
		return( (char *)str1 );
	
	while( *cp )
	{
		s1 = cp;
		s2 = (char *)str2;
		
		while( *s1 && *s2 && !( *s1 - *s2 ) )
		{
			s1++;
			s2++;
		}
		
		if( !*s2 )
			return (char*)cp;		
		cp++;
	}
	
	return (0);
}

char * str_stristr( const char * str1, const char * str2 )
{
	const char *s1, *s2;
	const char * cp = (const char *)str1;
	
	if ( !*str2 )
		return( (char *)str1 );
	
	while( *cp )
	{
		s1 = cp;
		s2 = (char *)str2;
		
		while( *s1 && 
			   *s2 && 
			   ( !( *s1 - *s2 ) || 
			     TOUPPER(*s1) == TOUPPER(*s2) ) )
		{
			s1++;
			s2++;
		}
		
		if( !*s2 )
			return (char*)cp;
		cp++;
	}
	
	return (0);
}


char *str_strdup( const char *str )
{
	char	*pnewstr;
	int		len;

	len = strlen( str ) + 1;
	pnewstr = malloc( len );
	if( pnewstr )
	    memcpy( pnewstr, str, len );

	return pnewstr;
}
