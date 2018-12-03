#include <edef.h>
#include <cpu.h>

void AtomicAdd( ATOMIC i, ATOMIC *lpv )
{
    *lpv += i;
}

void AtomicSub( ATOMIC i, ATOMIC *lpv )
{
    *lpv -= i;
}

void AtomicInc( ATOMIC *lpv )
{
    (*lpv)++;
}

void AtomicDec( ATOMIC *lpv )
{
    (*lpv)--;
}
