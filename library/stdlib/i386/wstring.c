#include <edef.h>
#include <estring.h>

#define TOUPPER( c ) ( ( (c) >= 'a' && (c) <= 'z' ) ? ((c) - 'a' + 'A') : (c) )
#define TOLOWER( c ) ( ( (c) >= 'A' && (c) <= 'Z' ) ? ((c) - 'A' + 'a') : (c) )

int wcs_icmp( const wchar_t *string1, const wchar_t *string2 )
{
	register const wchar_t * p1 = string1;//(const wchar_t *)string1;
	register const wchar_t * p2 = string2;//(const wchar_t *)string2;

	while( 1 )
	{
		register wchar_t s1 = *p1++;
		register wchar_t s2 = *p2++;
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

int wcs_nicmp( const wchar_t *string1, const wchar_t *string2, size_t count )
{
	register const wchar_t * p1 = string1;
	register const wchar_t * p2 = string2;

	while( count )
	{
		register wchar_t s1 = *p1++;
		register wchar_t s2 = *p2++;
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

int wcs_ncmp( const wchar_t *string1, const wchar_t *string2, size_t count )
{
	register const wchar_t * p1 = (const wchar_t *)string1;
	register const wchar_t * p2 = (const wchar_t *)string2;

	while( count )
	{
		register wchar_t s1 = *p1++;
		register wchar_t s2 = *p2++;

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

int wcs_cmp( const wchar_t *string1, const wchar_t *string2 )
{
	register const wchar_t * p1 = (const wchar_t *)string1;
	register const wchar_t * p2 = (const wchar_t *)string2;

	while( 1 )
	{
		register wchar_t s1 = *p1++;
		register wchar_t s2 = *p2++;

		if(  s1 > s2 )
			return 1;
		else if( s1 < s2 )
			return -1;
		else if( s1 == 0 && s2 == 0 )
			break;
	}
	return 0;
}


wchar_t *wcs_cpy( wchar_t *strDest, const wchar_t *strSource )
{
	register wchar_t * pd = strDest;
	register const wchar_t * ps = strSource;

	while( *ps )
	{
		*pd++ = *ps++;
	}
	*pd = 0;
	return strDest;
}

wchar_t *wcs_ncpy( wchar_t *strDest, const wchar_t *strSource, size_t count )
{
	register wchar_t * pd = strDest;
	register const wchar_t * ps = strSource;
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

wchar_t *wcs_cat( wchar_t *strDest, const wchar_t *strSource )
{
	register wchar_t * pd = strDest;
	register const wchar_t * ps = strSource;

	while( *pd )pd++;

	while( *ps )
	{
		*pd++ = *ps++;
	}
	*pd = 0;
	return strDest;
}

wchar_t * wcs_ncat( wchar_t * strDest, const wchar_t * strSource, size_t count )
{
	wchar_t *start = strDest;
	
	while (*strDest++)
		;
	strDest--;
	
	while (count--)
		if (!(*strDest++ = *strSource++))
			return(start);
		
    *strDest = '\0';
    return(start);
}

size_t wcs_len( const wchar_t *string )
{
	register const wchar_t * pd = string;
	register size_t count = 0;
	
	while( *pd++ )count++;

	return count;
}

wchar_t *wcs_chr( const wchar_t *s1, int i )
{
	const wchar_t *s = (const wchar_t *)s1;
	wchar_t c = (wchar_t)i;
	
	while ( *s && *s != c )
    {
		s++;
    }
	
	if (*s != c)
    {
		s = 0;
    }
	
	return (wchar_t *) s;
}

wchar_t * wcs_rchr( const wchar_t *s, int i )
{
	const wchar_t *last = 0;
	
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
		  
	return (wchar_t *) last;
}

wchar_t * wcs_strstr( const wchar_t *str1, const wchar_t *str2 )
{
	const wchar_t *s1, *s2;
	const wchar_t * cp = (const wchar_t *)str1;
	
	if ( !*str2 )
		return( (wchar_t *)str1 );
	
	while( *cp )
	{
		s1 = cp;
		s2 = (wchar_t *)str2;
		
		while( *s1 && *s2 && !( *s1 - *s2 ) )
		{
			s1++;
			s2++;
		}
		
		if( !*s2 )
			return (wchar_t*)cp;		
		cp++;
	}
	
	return (0);
}

wchar_t * wcs_stristr( const wchar_t *str1, const wchar_t *str2 )
{
	const wchar_t *s1, *s2;
	const wchar_t * cp = (const wchar_t *)str1;
	
	if ( !*str2 )
		return( (wchar_t *)str1 );
	
	while( *cp )
	{
		s1 = cp;
		s2 = (wchar_t *)str2;
		
		while( *s1 && 
			   *s2 && 
			   ( !( *s1 - *s2 ) || 
			     TOUPPER(*s1) == TOUPPER(*s2) ) )
		{
			s1++;
			s2++;
		}
		
		if( !*s2 )
			return (wchar_t*)cp;
		cp++;
	}
	
	return (0);
}
