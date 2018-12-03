#include <edef.h>
#include <estdlib.h>

#define MULTIPLIER      0x015a4e35L
#define INCREMENT       1

static  ULONG ulSeed = 1;
int errno = 0;

void std_srand(unsigned seed)
{
    ulSeed = seed;
}

int std_rand(void)
{
    ulSeed = MULTIPLIER * ulSeed + INCREMENT;
    return((int)(ulSeed >> 16) & 0x7fff);
}

unsigned long std_random( void )
{
    ULONG hi, lo; 
	ulSeed = MULTIPLIER * ulSeed + INCREMENT;
	hi = ulSeed & 0xffff0000l;
    ulSeed = MULTIPLIER * ulSeed + INCREMENT;
	lo = ulSeed >> 16;
    return (hi | lo);
}


static void  xtoa (
        unsigned long val,
        char *buf,
        unsigned radix,
        int is_neg
        )
{
	char *p;                /* pointer to traverse string */
	char *firstdig;         /* pointer to first digit */
	char temp;              /* temp char */
	unsigned digval;        /* value of digit */
	
	p = buf;
	
	if (is_neg) {
		/* negative, so output '-' and negate */
		*p++ = '-';
		val = (unsigned long)(-(long)val);
	}
	
	firstdig = p;           /* save pointer to first digit */
	
	do {
		digval = (unsigned) (val % radix);
		val /= radix;       /* get next digit */
		
		/* convert to ascii and store */
		if (digval > 9)
			*p++ = (char) (digval - 10 + 'a');  /* a letter */
		else
			*p++ = (char) (digval + '0');       /* a digit */
	} while (val > 0);
	
	/* We now have the digit of the number in the buffer, but in reverse
	order.  Thus we reverse them now. */
	
	*p-- = '\0';            /* terminate string; p points to last digit */
	
	do {
		temp = *p;
		*p = *firstdig;
		*firstdig = temp;   /* swap *p and *firstdig */
		--p;
		++firstdig;         /* advance to next two digits */
	} while (firstdig < p); /* repeat until halfway */
}

char * std_itoa( int val, char *buf, int radix )
{
	if (radix == 10 && val < 0)
		xtoa((unsigned long)val, buf, radix, 1);
	else
		xtoa((unsigned long)(unsigned int)val, buf, radix, 0);
	return buf;
}

char * std_ltoa ( long val, char *buf, int radix )
{
	xtoa((unsigned long)val, buf, radix, (radix == 10 && val < 0));
	return buf;
}

char * std_ultoa ( unsigned long val, char *buf, int radix )
{
	xtoa(val, buf, radix, 0);
	return buf;
}

#define IS_NUM( c ) ( (c) >= '0' && (c) <= '9' )
long std_atol( const char *nptr )
{
	int c;              /* current char */
	long total;         /* current total */
	int sign;           /* if '-', then negative, otherwise positive */
	
	/* skip whitespace */
	//while( *nptr == ' ' )++ptr;

	while( isspace( (int)(unsigned char)*nptr ) )++nptr;
	
	c = (int)(unsigned char)*nptr++;
	sign = c;           /* save sign indication */
	if (c == '-' || c == '+')
		c = (int)(unsigned char)*nptr++;    /* skip sign */
	
	total = 0;
	
	while (IS_NUM(c)) {
		total = 10 * total + (c - '0');     /* accumulate digit */
		c = (int)(unsigned char)*nptr++;    /* get next char */
	}
	
	if (sign == '-')
		return -total;
	else
		return total;   /* return result, negated if necessary */
}

int std_atoi( const char *nptr )
{
    return (int)std_atol(nptr);
}


#ifndef ULONG_MAX
#define	ULONG_MAX	((unsigned long)(~0L))
#endif

#ifndef LONG_MAX
#define	LONG_MAX	((long)(ULONG_MAX >> 1))
#endif

#ifndef LONG_MIN
#define	LONG_MIN	((long)(~LONG_MAX))
#endif


#define NF_UNSIGNED     1
#define NF_OVERFLOW     2
#define NF_HASDIGIT     4
#define NF_ISNEG        8

static unsigned long strtoxl( const char *pn, char **pend, int ibase, int flags )
{
	const char *pcur;
	unsigned long nadd;
	unsigned val;
	unsigned long maxval;
	char c;
	
	if( ibase < 0 ||
		ibase == 1 ||
		ibase > 36 )
	{
		if( pend )
			*pend = (char*)pn;
		return 0;
	}

	nadd = 0;    
	pcur = pn;
	
	//c = *pcur++;
	// skip space
	//while( c == ' ' )
//		c = *pcur++;	

	while( isspace( (int)(unsigned char)*pcur ) )++pcur;

	c = (int)(unsigned char)*pcur++;

	if( c == '-' )
	{
		flags |= NF_ISNEG;
		c = *pcur++;
	}
	else if( c == '+' )
		c = *pcur++;
	
	if( ibase == 0 )
	{  // get base from string
		if( c == '0' )
		{
			if( *pcur == 'x' || *pcur == 'X' )
				ibase = 16;
			else
				ibase = 8;
		}
		else
			ibase = 10;
	}
	
	if( ibase == 16 )
	{  // remove "0x" or "0X"
		if( c == '0' &&
			( *pcur == 'x' || *pcur == 'X' ) 
		  ) 
		{
			pcur++;
			c = *pcur++;
		}
	}
	
	maxval = ULONG_MAX / ibase;	
	
	while( 1 )
	{  
		if( c >= '0' && c <= '9' )
			val = c - '0';
		else if( c >= 'a' && c <= 'z' )
			val = c - 'a' + 10;
		else if( c >= 'A' && c <= 'Z' )
			val = c - 'A' + 10;
		else
			break;
		if( val >= (unsigned)ibase )
			break;          //error
		
	    flags |= NF_HASDIGIT;

		if( nadd < maxval || 
			( nadd == maxval &&
			  (unsigned long)val <= ULONG_MAX % ibase ) )
		{
		    nadd = nadd * ibase + val;
		}
		else
		{
			// overflow
			flags |= NF_OVERFLOW;
		}		
		c = *pcur++;
	}
	
	pcur--;
	
	if( ( flags & NF_HASDIGIT ) == 0 )
	{
		if( pend )
			pcur = pn;
		nadd = 0;
	}
	else if( ( flags & NF_OVERFLOW ) ||
		     ( !( flags & NF_UNSIGNED ) &&
		        ( ( ( flags & NF_ISNEG ) && 
				    ( nadd > -LONG_MIN ) ) ||
		          ( !( flags & NF_ISNEG ) && ( nadd > LONG_MAX ) ) ) ) )
	{
		// overflow or signed overflow
		errno = ERANGE;
		if( flags & NF_UNSIGNED )
			nadd = ULONG_MAX;
		else if( flags & NF_ISNEG )
			nadd = (unsigned long)(-LONG_MIN);
		else
			nadd = LONG_MAX;
	}
	
	if( pend )
		*pend = (char*)pcur;	
	if( flags & NF_ISNEG )
		nadd = (unsigned long)(-(long)nadd);	
	return nadd;
}

unsigned long std_strtoul( const char *pn, char **pend, int ibase )
{
    return strtoxl( pn, pend, ibase, NF_UNSIGNED );
}

long std_strtol( const char *pn, char **pend, int ibase )
{
	return (long)strtoxl(pn, pend, ibase, 0);
}

char *std_getenv( const char *varname )
{
	return 0;
}

static INT64 __atoi64( const char *nptr )
{
	int c;
	INT64 total;
	int sign;
	
	while( isspace((int)(unsigned char)*nptr) )
		++nptr;
	
	c = (int)(unsigned char)*nptr++;
	sign = c;           /* save sign indication */
	if (c == '-' || c == '+')
		c = (int)(unsigned char)*nptr++;    /* skip sign */
	
	total = 0;
	
	while (isdigit(c)) {
		total = 10 * total + (c - '0');     /* accumulate digit */
		c = (int)(unsigned char)*nptr++;    /* get next char */
	}
	
	if (sign == '-')
		return -total;
	else
		return total;   /* return result, negated if necessary */
}


INT64 std_atoi64( const char *ptr )
{
	return __atoi64( ptr );
}

INT64 std_atoll( const char *ptr )
{
	return __atoi64( ptr );
}
