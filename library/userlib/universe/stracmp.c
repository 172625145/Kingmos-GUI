#include <eWindows.h>

#define TOUPPER( c ) ( ( (c) >= 'a' && (c) <= 'z' ) ? ((c) - 'a' + 'A') : (c) )

int StrAsteriskCmp(const char *string1, const char *string2)
{
	register const unsigned char * p1 = (const unsigned char *)string1;
	register const unsigned char * p2 = (const unsigned char *)string2;

	while( 1 )
	{
		register unsigned char s1 = *p1++;
		register unsigned char s2 = *p2++;
		if (s1 == '?')
		{
			continue;
		}
		else if (s1 == '*')
		{
			s1 = *p1++;
			if (s1 == 0)
				break;
			while(1)
			{
				s1 = TOUPPER(s1);
				s2 = TOUPPER(s2);
				if (s1 == s2)
					break;
				if (s2 == 0)
					return 1;
				s2 = *p2++;
			}
			continue;
		}
		s1 = TOUPPER(s1);
		s2 = TOUPPER(s2);
		{
			if(  s1 > s2 )
				return 1;
			else if( s1 < s2 )
				return -1;
			else if( s1 == 0 && s2 == 0 )
				break;
		}
	}
	return 0;
}
