#include <edef.h>
#include <estring.h>

#define TOUPPER( c ) ( ( (c) >= 'a' && (c) <= 'z' ) ? ((c) - 'a' + 'A') : (c) )
#define TOLOWER( c ) ( ( (c) >= 'A' && (c) <= 'Z' ) ? ((c) - 'A' + 'a') : (c) )

int wcs_icmp( const WCHAR_T *string1, const WCHAR_T *string2 )
{
	register const WCHAR_T * p1 = string1;//(const WCHAR_T *)string1;
	register const WCHAR_T * p2 = string2;//(const WCHAR_T *)string2;

	while( 1 )
	{
		register WCHAR_T s1 = *p1++;
		register WCHAR_T s2 = *p2++;
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

int wcs_nicmp( const WCHAR_T *string1, const WCHAR_T *string2, SIZE_T count )
{
	register const WCHAR_T * p1 = string1;
	register const WCHAR_T * p2 = string2;

	while( count )
	{
		register WCHAR_T s1 = *p1++;
		register WCHAR_T s2 = *p2++;
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

int wcs_ncmp( const WCHAR_T *string1, const WCHAR_T *string2, SIZE_T count )
{
	register const WCHAR_T * p1 = (const WCHAR_T *)string1;
	register const WCHAR_T * p2 = (const WCHAR_T *)string2;

	while( count )
	{
		register WCHAR_T s1 = *p1++;
		register WCHAR_T s2 = *p2++;

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

int wcs_cmp( const WCHAR_T *string1, const WCHAR_T *string2 )
{
	register const WCHAR_T * p1 = (const WCHAR_T *)string1;
	register const WCHAR_T * p2 = (const WCHAR_T *)string2;

	while( 1 )
	{
		register WCHAR_T s1 = *p1++;
		register WCHAR_T s2 = *p2++;

		if(  s1 > s2 )
			return 1;
		else if( s1 < s2 )
			return -1;
		else if( s1 == 0 && s2 == 0 )
			break;
	}
	return 0;
}


WCHAR_T *wcs_cpy( WCHAR_T *strDest, const WCHAR_T *strSource )
{
	register WCHAR_T * pd = strDest;
	register const WCHAR_T * ps = strSource;

	while( *ps )
	{
		*pd++ = *ps++;
	}
	*pd = 0;
	return strDest;
}

WCHAR_T *wcs_ncpy( WCHAR_T *strDest, const WCHAR_T *strSource, SIZE_T count )
{
	register WCHAR_T * pd = strDest;
	register const WCHAR_T * ps = strSource;
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

WCHAR_T *wcs_cat( WCHAR_T *strDest, const WCHAR_T *strSource )
{
	register WCHAR_T * pd = strDest;
	register const WCHAR_T * ps = strSource;

	while( *pd )pd++;

	while( *ps )
	{
		*pd++ = *ps++;
	}
	*pd = 0;
	return strDest;
}

WCHAR_T * wcs_ncat( WCHAR_T * strDest, const WCHAR_T * strSource, SIZE_T count )
{
	WCHAR_T *start = strDest;
	
	while (*strDest++)
		;
	strDest--;
	
	while (count--)
		if (!(*strDest++ = *strSource++))
			return(start);
		
    *strDest = '\0';
    return(start);
}

SIZE_T wcs_len( const WCHAR_T *string )
{
	register const WCHAR_T * pd = string;
	register SIZE_T count = 0;
	
	while( *pd++ )count++;

	return count;
}

WCHAR_T *wcs_chr( const WCHAR_T *s1, int i )
{
	const WCHAR_T *s = (const WCHAR_T *)s1;
	WCHAR_T c = (WCHAR_T)i;
	
	while ( *s && *s != c )
    {
		s++;
    }
	
	if (*s != c)
    {
		s = 0;
    }
	
	return (WCHAR_T *) s;
}

WCHAR_T * wcs_rchr( const WCHAR_T *s, int i )
{
	const WCHAR_T *last = 0;
	
	if( i )
    {
		while (s=wcs_chr(s, i))
		{
			last = s;
			s++;
		}
    }
	else
    {
		last = wcs_chr(s, i);
    }
		  
	return (WCHAR_T *) last;
}

WCHAR_T * wcs_strstr( const WCHAR_T *str1, const WCHAR_T *str2 )
{
	const WCHAR_T *s1, *s2;
	const WCHAR_T * cp = (const WCHAR_T *)str1;
	
	if ( !*str2 )
		return( (WCHAR_T *)str1 );
	
	while( *cp )
	{
		s1 = cp;
		s2 = (WCHAR_T *)str2;
		
		while( *s1 && *s2 && !( *s1 - *s2 ) )
		{
			s1++;
			s2++;
		}
		
		if( !*s2 )
			return (WCHAR_T*)cp;		
		cp++;
	}
	
	return (0);
}

WCHAR_T * wcs_stristr( const WCHAR_T *str1, const WCHAR_T *str2 )
{
	const WCHAR_T *s1, *s2;
	const WCHAR_T * cp = (const WCHAR_T *)str1;
	
	if ( !*str2 )
		return( (WCHAR_T *)str1 );
	
	while( *cp )
	{
		s1 = cp;
		s2 = (WCHAR_T *)str2;
		
		while( *s1 && 
			   *s2 && 
			   ( !( *s1 - *s2 ) || 
			     TOUPPER(*s1) == TOUPPER(*s2) ) )
		{
			s1++;
			s2++;
		}
		
		if( !*s2 )
			return (WCHAR_T*)cp;
		cp++;
	}
	
	return (0);
}
