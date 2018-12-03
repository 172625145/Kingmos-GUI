
/* Copyright (c) Mark J. Kilgard, 1994. */

/* This program is freely distributable without licensing fees
   and is provided without guarantee or warrantee expressed or
   implied. This program is -not- in the public domain. */

#include <estdlib.h>
#include <estdarg.h>
#include <estring.h>
#include <estdio.h>

#include "glutint.h"
#include "gl\mlg_glstruct.h"
//#include "gl\mlg_glfile.h"
//extern	int gl_fprintf( FILE *filename, const char *sz, ...);
#define		gl_fprintf(p1,p2,p3)	( NULL )
#define		vfprintf(p1,p2,p3)		( NULL )
#define		putc(p1,p2)				( NULL )

/* strdup is actually not a standard ANSI C or POSIX routine
   so implement a private one for GLUT.  OpenVMS does not have a
   strdup; Linux's standard libc doesn't declare strdup by default
   (unless BSD or SVID interfaces are requested). */
char *
__glutStrdup(const char *string)
{
  char *copy;

  copy = (char*) malloc(strlen(string) + 1);
  if (copy == NULL)
    return NULL;
  strcpy(copy, string);
  return copy;
}

void
__glutWarning(char *format,...)
{
  va_list args;

  va_start(args, format);
  gl_fprintf(stderr, "GLUT: Warning in %s: ",
    __glutProgramName ? __glutProgramName : "(unamed)");
  vfprintf(stderr, format, args);
  va_end(args);
  putc('\n', stderr);
}

/* CENTRY */
void GLUTAPIENTRY 
glutReportErrors(void)
{
  GLenum error;

  while ((error = glGetError()) != GL_NO_ERROR)
    __glutWarning("GL error: %s", gluErrorString(error));
}
/* ENDCENTRY */

void
__glutFatalError(char *format,...)
{
  va_list args;

  va_start(args, format);
  gl_fprintf(stderr, "GLUT: Fatal Error in %s: ",
    __glutProgramName ? __glutProgramName : "(unamed)");
  vfprintf(stderr, format, args);
  va_end(args);
  putc('\n', stderr);
#ifdef _WIN32
  if (__glutExitFunc) {
    __glutExitFunc(1);
  }
#endif
  exit(1);
}

void
__glutFatalUsage(char *format,...)
{
  va_list args;

  va_start(args, format);
  gl_fprintf(stderr, "GLUT: Fatal API Usage in %s: ",
    __glutProgramName ? __glutProgramName : "(unamed)");
  vfprintf(stderr, format, args);
  va_end(args);
  putc('\n', stderr);
  abort();
}
