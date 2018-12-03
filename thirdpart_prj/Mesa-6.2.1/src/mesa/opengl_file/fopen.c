///////////////////////////////////////////////////////////
// 实现OPENGL的专用文件操作函数
///////////////////////////////////////////////////////////

#include <edef.h>
#include <ecore.h>
#include <efile.h>
#include <estdio.h>
#include <estring.h>

#include "gl/mlg_glfile.h"

#include "gl/mlg_glthird.h"

HANDLE gl_fopen( const char *filename, const char *mode )
{
	FILE* hFile;
	hFile = (FILE*)CreateFile(filename, GENERIC_READ|GENERIC_WRITE,0, NULL, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL );
	if(hFile == INVALID_HANDLE_VALUE)
		return NULL;
	else
		return hFile;
}


int gl_fclose( FILE *filename )
{
	if(CloseHandle(filename) == TRUE)
		return 0;
	else
		return -1;

}

size_t gl_fread( void *buffer, size_t size, size_t count, FILE *filename )
{
	DWORD sizenum;
	ReadFile(filename,buffer,count * size,&sizenum,NULL);
	return sizenum;
}

size_t gl_fwrite( const void *buffer, size_t size, size_t count, FILE *filename )
{
	DWORD sizenum;
	WriteFile(filename,buffer,count*size,&sizenum,NULL);
	return sizenum;
}

int gl_fputc( int c, FILE *filename )
{
	char ch = c;
	DWORD sizenum;
	WriteFile(filename,&ch,1,&sizenum,NULL);
	if(sizenum == 1)
		return 0;
	else
		return -1;
}

int gl_fputs( const char *string, FILE *filename )
{
	DWORD count,sizenum;
	count = strlen(string);
	WriteFile(filename,string,count,&sizenum,NULL);
	if(count == sizenum)
		return 0;
	else
		return -1;

}

int gl_fflush( FILE *filename )
{
	return 0;
}

int gl_ferror( FILE *filename )
{
	return 0;
}


int gl_fseek( FILE *filename, long offset, int origin )
{
	DWORD ret,orig;

	if(origin == SEEK_CUR)
		orig = FILE_CURRENT;
	else if(origin == SEEK_END)
		orig = FILE_END;
	else if(origin == SEEK_SET)
		orig = FILE_BEGIN;

	ret = SetFilePointer(filename,offset,NULL,orig);
	if(ret == 0xffffffff)
		return -1;
	else
		return 0;
}

void gl_ftell( FILE *filename )
{
	;
}

int gl_fgetc( FILE *filename )
{
	char buf;
	DWORD sizenum;
	ReadFile(filename,&buf,1,&sizenum,NULL);
	if(sizenum == 1)
		return buf;
	else
		return -1;
}
char * gl_fgets( char *string, int n, FILE *filename )
{
	DWORD sizenum;
	BOOL ret;
	char buf;
	char * pt = string;
	int i = 0;
	do
	{
		ret = ReadFile(filename,&buf,1,&sizenum,NULL);
		if (ret == FALSE || sizenum == 0)
			break;
		*pt++ = buf;
		i++;
	}while((buf != '\n') && (i != n));

	if(i == 0)
		return 0;
	else
	{
		*pt = '\0';
		return string;
	}
}


void gl_feof( FILE *filename )
{
	;
}

// 注意：在程序中用到“%i”和“%f”，因为它们的宽度相同，故可以用sizeof(int)
// 如果以后修改，则必须修改过来
int	gl_fscanf( FILE * fp, int* lpInt )
{
	gl_fread( lpInt, sizeof(int), 1, fp );	// OPENGL 专用
	return 0;
}
