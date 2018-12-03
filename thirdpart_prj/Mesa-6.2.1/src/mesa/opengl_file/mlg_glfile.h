// 定义文件转为OPENGL服务操作的函数

#ifndef __MLG_GLFILE
#define __MLG_GLFILE

#include <edef.h>
#include <ecore.h>
#include <efile.h>
#include <estdio.h>
#include <estring.h>

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

FILE*	gl_fopen( const char *filename, const char *mode );
int		gl_fclose( FILE *filename );
size_t	gl_fread( void *buffer, size_t size, size_t count, FILE *filename );
size_t	gl_fwrite( const void *buffer, size_t size, size_t count, FILE *filename );
int		gl_fputc( int c, FILE *filename );
int		gl_fputs( const char *string, FILE *filename );
int		gl_fflush( FILE *filename );
int		gl_ferror( FILE *filename );
int		gl_fseek( FILE *filename, long offset, int origin );
void	gl_ftell( FILE *filename );
int		gl_fgetc( FILE *filename );
char *	gl_fgets( char *string, int n, FILE *filename );
void	gl_feof( FILE *filename );
int		gl_fscanf( FILE * fp, void* lpInt );


/* Seek method constants */

#define SEEK_CUR    1
#define SEEK_END    2
#define SEEK_SET    0

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif