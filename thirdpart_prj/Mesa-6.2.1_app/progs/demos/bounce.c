
/*
 * Bouncing ball demo.
 *
 * This program is in the public domain
 *
 * Brian Paul
 *
 * Conversion to GLUT by Mark J. Kilgard
 */

#include "ewindows.h"
#include "third_zt.h"
#include "math.h"
#include <estdlib.h>
#include <estring.h>
#include <GL/mlg_glut.h>

#define COS(X)   cos( (X) * 3.14159/180.0 )
#define SIN(X)   sin( (X) * 3.14159/180.0 )

#define RED 1
#define WHITE 2
#define CYAN 3

GLboolean IndexMode = GL_FALSE;
GLuint Ball;
GLenum Mode;
GLfloat Zrot = 0.0, Zstep = 6.0;
GLfloat Xpos = 0.0, Ypos = 1.0;
GLfloat Xvel = 0.2, Yvel = 0.0;
GLfloat Xmin = -4.0, Xmax = 4.0;
GLfloat Ymin = -3.8, Ymax = 4.0;
GLfloat G = -0.1;

static GLuint 
make_ball(void)
{
  GLuint list;
  GLfloat a, b;
  GLfloat da = 18.0, db = 18.0;
  GLfloat radius = 1.0;
  GLuint color;
  GLfloat x, y, z;

  list = glGenLists(1);

  glNewList(list, GL_COMPILE);

  color = 0;
  for (a = -90.0; a + da <= 90.0; a += da) {

    glBegin(GL_QUAD_STRIP);
    for (b = 0.0; b <= 360.0; b += db) {

      if (color) {
	glIndexi(RED);
        glColor3f(1, 0, 0);
      } else {
	glIndexi(WHITE);
        glColor3f(1, 1, 1);
      }

      x = radius * COS(b) * COS(a);
      y = radius * SIN(b) * COS(a);
      z = radius * SIN(a);
      glVertex3f(x, y, z);

      x = radius * COS(b) * COS(a + da);
      y = radius * SIN(b) * COS(a + da);
      z = radius * SIN(a + da);
      glVertex3f(x, y, z);

      color = 1 - color;
    }
    glEnd();

  }

  glEndList();

  return list;
}

static void 
reshape(int width, int height)
{
  float aspect = (float) width / (float) height;
  glViewport(0, 0, (GLint) width, (GLint) height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-6.0 * aspect, 6.0 * aspect, -6.0, 6.0, -6.0, 6.0);
  glMatrixMode(GL_MODELVIEW);
}

/* ARGSUSED1 */
static void
key(unsigned char k, int x, int y)
{
  switch (k) {
  case 27:  /* Escape */
    exit(0);
  }
}

static void 
draw(void)
{
  GLint i;

  glClear(GL_COLOR_BUFFER_BIT);

  glIndexi(CYAN);
  glColor3f(0, 1, 1);
  glBegin(GL_LINES);
  for (i = -5; i <= 5; i++) {
    glVertex2i(i, -5);
    glVertex2i(i, 5);
  }
  for (i = -5; i <= 5; i++) {
    glVertex2i(-5, i);
    glVertex2i(5, i);
  }
  for (i = -5; i <= 5; i++) {
    glVertex2i(i, -5);
    glVertex2f(i * 1.15, -5.9);
  }
  glVertex2f(-5.3, -5.35);
  glVertex2f(5.3, -5.35);
  glVertex2f(-5.75, -5.9);
  glVertex2f(5.75, -5.9);
  glEnd();

  glPushMatrix();
  glTranslatef(Xpos, Ypos, 0.0);
  glScalef(2.0, 2.0, 2.0);
  glRotatef(8.0, 0.0, 0.0, 1.0);
  glRotatef(90.0, 1.0, 0.0, 0.0);
  glRotatef(Zrot, 0.0, 0.0, 1.0);

  glCallList(Ball);

  glPopMatrix();

  glFlush();
  glutSwapBuffers();
}

static void 
idle(void)
{
  static float vel0 = -100.0;

  Zrot += Zstep;

  Xpos += Xvel;
  if (Xpos >= Xmax) {
    Xpos = Xmax;
    Xvel = -Xvel;
    Zstep = -Zstep;
  }
  if (Xpos <= Xmin) {
    Xpos = Xmin;
    Xvel = -Xvel;
    Zstep = -Zstep;
  }
  Ypos += Yvel;
  Yvel += G;
  if (Ypos < Ymin) {
    Ypos = Ymin;
    if (vel0 == -100.0)
      vel0 = fabs(Yvel);
    Yvel = vel0;
  }
  glutPostRedisplay();
}

static void visible(int vis)
{
  if (vis == GLUT_VISIBLE)
    glutIdleFunc(idle);
  else
    glutIdleFunc(NULL);
}

// 让Mesa程序运行起来
int Run_Mesa_Bounce( )
{
	int argc = 1;
	char *argv[] = { "TEST", 0 };

	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(240, 300);


	IndexMode = argc > 1 && strcmp(argv[1], "-ci") == 0;
	IndexMode = 1;
	if (IndexMode)
	 glutInitDisplayMode(GLUT_INDEX | GLUT_DOUBLE);
	else
	 glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

	glutCreateWindow("GL-Test");
	Ball = make_ball();
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glDisable(GL_DITHER);
	glShadeModel(GL_FLAT);

	glutDisplayFunc(draw);
	glutReshapeFunc(reshape);
	glutVisibilityFunc(visible);
	glutKeyboardFunc(key);

	if (IndexMode) {
	glutSetColor(RED, 1.0, 0.0, 0.0);
	glutSetColor(WHITE, 1.0, 1.0, 1.0);
	glutSetColor(CYAN, 0.0, 1.0, 1.0);
	}

	glutMainLoop();
	return 0;             /* ANSI C requires main to return int. */
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
#define	ID_TIMER1	100
static LRESULT CALLBACK Mesa_Bounce_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_TIMER:
			{
				if (ID_TIMER1==wParam)
				{
					KillTimer( hWnd, ID_TIMER1 );
					Run_Mesa_Bounce( );
				}
			}
			return 0;
		case WM_CREATE:
			{
				SetTimer( hWnd, ID_TIMER1, 1000, NULL );
			}
			return 0;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
   return 0;
}

static TCHAR classMESA_BOUNCE[20] = "Mesa_Bounce";
static ATOM Register_Mesa_Bounce_Class( HINSTANCE hInstance )
{
	WNDCLASS    wc;
    
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)Mesa_Bounce_Proc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= NULL;
	wc.hCursor			= NULL;
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= classMESA_BOUNCE;
 
	return RegisterClass(&wc);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef INLINE_PROGRAM
LRESULT CALLBACK WinMain( HINSTANCE hInstance,HINSTANCE hPrevInstance,
            LPSTR     lpCmdLine,int       nCmdShow )
#else
LRESULT CALLBACK WinMain_Bounce( HINSTANCE hInstance,HINSTANCE hPrevInstance,
            LPSTR     lpCmdLine,int       nCmdShow )
#endif
{
	MSG msg;
	HWND hWnd = NULL;

	Register_Mesa_Bounce_Class( hInstance );	// 注册窗口
	hWnd = CreateWindowEx(0,classMESA_BOUNCE, "Bunce", WS_VISIBLE|WS_POPUP,
			WORK_AREA_STARTX, WORK_AREA_STARTY,
			WORK_AREA_WIDTH,
			WORK_AREA_HEIGHT,
			0,0,hInstance,0 );
	ShowWindow(hWnd, TRUE); // 显示窗口

	while( GetMessage(&msg, NULL, 0, 0) ) 
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	return msg.wParam;
}

