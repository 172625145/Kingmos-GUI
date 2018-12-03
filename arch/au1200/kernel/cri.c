#include <edef.h>
#include <ecore.h>
#include <pthread.h>
#include <errno.h>

VOID WINAPI CriticalSection_Initialize( LPCRITICAL_SECTION lpCriticalSection )
{
	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );

	pthread_mutexattr_t mutexattr;
	pthread_mutexattr_init( &mutexattr );
	pthread_mutexattr_settype( &mutexattr, PTHREAD_MUTEX_RECURSIVE_NP );
	//lpCriticalSection->mutex = 0;//PTHREAD_MUTEX_INITIALIZER;
	//lpCriticalSection->mutex;
	memset( lpCriticalSection, 0, sizeof(CRITICAL_SECTION) );
	pthread_mutex_init( &lpCriticalSection->mutex, &mutexattr );
	pthread_mutexattr_destroy( &mutexattr );
}

VOID WINAPI CriticalSection_Delete( LPCRITICAL_SECTION lpCriticalSection )
{
	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );

	pthread_mutex_destroy( &lpCriticalSection->mutex );
}

VOID WINAPI CriticalSection_Leave( LPCRITICAL_SECTION lpCriticalSection )
{
	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );

	pthread_mutex_unlock( &lpCriticalSection->mutex );
}

VOID WINAPI CriticalSection_Enter( LPCRITICAL_SECTION lpCriticalSection )
{
	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );

	pthread_mutex_lock( &lpCriticalSection->mutex );
}

BOOL WINAPI CriticalSection_TryEnter( LPCRITICAL_SECTION lpCriticalSection )
{
	//printf( "call entry %s, %d.\r\n", __FILE__, __LINE__ );

	int retv = pthread_mutex_trylock( &lpCriticalSection->mutex );
	if( retv == 0 || retv == EDEADLK )
		return TRUE;
	else
		return FALSE;
}

