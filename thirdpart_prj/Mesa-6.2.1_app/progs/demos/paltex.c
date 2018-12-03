
/*
 * Paletted texture demo.  Written by Brian Paul.
 * This program is in the public domain.
 */

#include <estdio.h>
#include <estdlib.h>
#include <math.h>
#include <estring.h>
#ifdef _WIN32
#include <ewindows.h>
#endif
#define GL_GLEXT_PROTOTYPES
#include <GL/mlg_glut.h>


static float Rot = 0.0;
static GLboolean Anim = 1;


static void Idle( void )
{
   float t = glutGet(GLUT_ELAPSED_TIME) * 0.001;  /* in seconds */
   Rot = t * 360 / 4;  /* 1 rotation per 4 seconds */
   glutPostRedisplay();
}


static void Display( void )
{
   /* draw background gradient */
   glDisable(GL_TEXTURE_2D);
   glBegin(GL_POLYGON);
   glColor3f(1.0, 0.0, 0.2); glVertex2f(-1.5, -1.0);
   glColor3f(1.0, 0.0, 0.2); glVertex2f( 1.5, -1.0);
   glColor3f(0.0, 0.0, 1.0); glVertex2f( 1.5,  1.0);
   glColor3f(0.0, 0.0, 1.0); glVertex2f(-1.5,  1.0);
   glEnd();

   glPushMatrix();
   glRotatef(Rot, 0, 0, 1);

   glEnable(GL_TEXTURE_2D);
   glBegin(GL_POLYGON);
   glTexCoord2f(0, 1);  glVertex2f(-1, -0.5);
   glTexCoord2f(1, 1);  glVertex2f( 1, -0.5);
   glTexCoord2f(1, 0);  glVertex2f( 1,  0.5);
   glTexCoord2f(0, 0);  glVertex2f(-1,  0.5);
   glEnd();

   glPopMatrix();

   glutSwapBuffers();
}


static void Reshape( int width, int height )
{
   glViewport( 0, 0, width, height );
   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();
   glOrtho( -1.5, 1.5, -1.0, 1.0, -1.0, 1.0 );
   glMatrixMode( GL_MODELVIEW );
   glLoadIdentity();
}


static void Key( unsigned char key, int x, int y )
{
   (void) x;
   (void) y;
   switch (key) {
      case 27:
         exit(0);
         break;
      case 's':
         Rot += 0.5;
         break;
      case ' ':
         Anim = !Anim;
         if (Anim)
            glutIdleFunc( Idle );
         else
            glutIdleFunc( NULL );
         break;
   }
   glutPostRedisplay();
}


static void Init( void )
{
#define HEIGHT 8
#define WIDTH 32
   /* 257 = HEIGHT * WIDTH + 1 (for trailing '\0') */
/*   static char texture[257] = {"\
                                \
    MMM    EEEE   SSS    AAA    \
   M M M  E      S   S  A   A   \
   M M M  EEEE    SS    A   A   \
   M M M  E         SS  AAAAA   \
   M   M  E      S   S  A   A   \
   M   M   EEEE   SSS   A   A   \
                                "
      };*/
 static char texture[257] = {"\
                                \
    MMM    L       GGG    !!    \
   M M M   L      G   G   !!    \
   M M M   L      G       !!    \
   M M M   L      G GGG   !!    \
   M   M   L      G   G         \
   M   M   LLLLL   GGG    !!    \
                                "
      };
   GLubyte table[256][4];

   if (!glutExtensionSupported("GL_EXT_paletted_texture")) {
      //printf("Sorry, GL_EXT_paletted_texture not supported\n");
      //exit(0);
	   RETAILMSG(1, TEXT("Sorry, GL_EXT_paletted_texture not supported\r\n"));
	   PostQuitMessage( 0 );
   }

   /* load the color table for each texel-index */
   memset(table, 0, 256*4);
   table[' '][0] = 255;
   table[' '][1] = 255;
   table[' '][2] = 255;
   table[' '][3] = 64;
   table['M'][0] = 255;
   table['M'][1] = 0;
   table['M'][2] = 0;
   table['M'][3] = 255;
   table['L'][0] = 0;
   table['L'][1] = 255;
   table['L'][2] = 0;
   table['L'][3] = 255;
   table['G'][0] = 0;
   table['G'][1] = 0;
   table['G'][2] = 255;
   table['G'][3] = 255;
   table['!'][0] = 255;
   table['!'][1] = 255;
   table['!'][2] = 0;
   table['!'][3] = 255;

#ifdef GL_EXT_paletted_texture

#if defined(GL_EXT_shared_texture_palette) && defined(USE_SHARED_PALETTE)
   printf("Using shared palette\n");
   glColorTableEXT(GL_SHARED_TEXTURE_PALETTE_EXT,    /* target */
                   GL_RGBA,          /* internal format */
                   256,              /* table size */
                   GL_RGBA,          /* table format */
                   GL_UNSIGNED_BYTE, /* table type */
                   table);           /* the color table */
   glEnable(GL_SHARED_TEXTURE_PALETTE_EXT);
#else
   glColorTableEXT(GL_TEXTURE_2D,    /* target */
                   GL_RGBA,          /* internal format */
                   256,              /* table size */
                   GL_RGBA,          /* table format */
                   GL_UNSIGNED_BYTE, /* table type */
                   table);           /* the color table */
#endif

   glTexImage2D(GL_TEXTURE_2D,       /* target */
                0,                   /* level */
                GL_COLOR_INDEX8_EXT, /* internal format */
                WIDTH, HEIGHT,       /* width, height */
                0,                   /* border */
                GL_COLOR_INDEX,      /* texture format */
                GL_UNSIGNED_BYTE,    /* texture type */
                texture);            /* the texture */
#endif

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   glEnable(GL_TEXTURE_2D);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#undef HEIGHT
#undef WIDTH
}



/*
 * Color ramp test
 */
static void Init2( void )
{
#define HEIGHT 32
#define WIDTH 256
   static char texture[HEIGHT][WIDTH];
   GLubyte table[256][4];
   int i, j;

   if (!glutExtensionSupported("GL_EXT_paletted_texture")) {
	   RETAILMSG(1, TEXT("Sorry, GL_EXT_paletted_texture not supported\r\n"));
	   PostQuitMessage( 0 );
      //printf("Sorry, GL_EXT_paletted_texture not supported\n");
      //exit(0);
   }

   for (j = 0; j < HEIGHT; j++) {
      for (i = 0; i < WIDTH; i++) {
         texture[j][i] = i;
      }
   }

   for (i = 0; i < 255; i++) {
      table[i][0] = i;
      table[i][1] = 0;
      table[i][2] = 0;
      table[i][3] = 255;
   }

#ifdef GL_EXT_paletted_texture

#if defined(GL_EXT_shared_texture_palette) && defined(USE_SHARED_PALETTE)
   printf("Using shared palette\n");
   glColorTableEXT(GL_SHARED_TEXTURE_PALETTE_EXT,    /* target */
                   GL_RGBA,          /* internal format */
                   256,              /* table size */
                   GL_RGBA,          /* table format */
                   GL_UNSIGNED_BYTE, /* table type */
                   table);           /* the color table */
   glEnable(GL_SHARED_TEXTURE_PALETTE_EXT);
#else
   glColorTableEXT(GL_TEXTURE_2D,    /* target */
                   GL_RGBA,          /* internal format */
                   256,              /* table size */
                   GL_RGBA,          /* table format */
                   GL_UNSIGNED_BYTE, /* table type */
                   table);           /* the color table */
#endif

   glTexImage2D(GL_TEXTURE_2D,       /* target */
                0,                   /* level */
                GL_COLOR_INDEX8_EXT, /* internal format */
                WIDTH, HEIGHT,       /* width, height */
                0,                   /* border */
                GL_COLOR_INDEX,      /* texture format */
                GL_UNSIGNED_BYTE,    /* texture type */
                texture);            /* teh texture */
#endif

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   glEnable(GL_TEXTURE_2D);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


//int main( int argc, char *argv[] )
#ifndef INLINE_PROGRAM
LRESULT CALLBACK WinMain( HINSTANCE hInstance,HINSTANCE hPrevInstance,
            LPSTR     lpCmdLine,int       nCmdShow )
#else
LRESULT CALLBACK WinMain_Paltex( HINSTANCE hInstance,HINSTANCE hPrevInstance,
            LPSTR     lpCmdLine,int       nCmdShow )
#endif
{
	int argc = 1;
	char *argv[] = { "TEST", 0 };
   glutInit( &argc, argv );
   glutInitWindowPosition( 0, 20 );
   glutInitWindowSize( 240, 300 );

   glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );

   glutCreateWindow("GL-Test");

   Init();
   (void) Init2; /* silence warning */

   glutReshapeFunc( Reshape );
   glutKeyboardFunc( Key );
   glutDisplayFunc( Display );
   if (Anim)
      glutIdleFunc( Idle );

   glutMainLoop();
   return 0;
}
