typedef unsigned size_t;


void *memcpy( void *dest, const void *src, size_t count )
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

void *memset( void *dest, int c, size_t count )
{
	register char * pd = (char*)dest;
	while( count )
	{
		*pd++ = c; count--;
	}
	return dest;
}

