
/*
 * 3-D gear wheels.  This program is in the public domain.
 *
 * Command line options:
 *    -info      print GL implementation information
 *    -exit      automatically exit after 30 seconds
 *
 *
 * Brian Paul
 */

/* Conversion to GLUT by Mark J. Kilgard */



#include "ewindows.h"
//#include "third_zt.h"
#include <math.h>
#include <estdlib.h>
#include <estdio.h>
#include <estring.h>
#include <GL/mlg_glut.h>

#ifndef M_PI
#define M_PI 3.14159265
#endif

static GLint T0 = 0;
static GLint Frames = 0;
static GLint autoexit = 0;

/**

  Draw a gear wheel.  You'll probably want to call this function when
  building a display list since we do a lot of trig here.
 
  Input:  inner_radius - radius of hole at center
          outer_radius - radius at center of teeth
          width - width of gear
          teeth - number of teeth
          tooth_depth - depth of tooth

 **/

static void
gear(GLfloat inner_radius, GLfloat outer_radius, GLfloat width,
  GLint teeth, GLfloat tooth_depth)
{
	DWORD dwCurTick =	GetTickCount();
  GLint i;
  GLfloat r0, r1, r2;
  GLfloat angle, da;
  GLfloat u, v, len;

  r0 = inner_radius;
  r1 = outer_radius - tooth_depth / 2.0;
  r2 = outer_radius + tooth_depth / 2.0;

  da = 2.0 * M_PI / teeth / 4.0;

  glShadeModel(GL_FLAT);

  glNormal3f(0.0, 0.0, 1.0);

  /* draw front face */
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;
    glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
    glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
    if (i < teeth) {
      glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
      glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
    }
  }
  glEnd();

  /* draw front sides of teeth */
  glBegin(GL_QUADS);
  da = 2.0 * M_PI / teeth / 4.0;
  for (i = 0; i < teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;

    glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
    glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5);
    glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5);
    glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
  }
  glEnd();

  glNormal3f(0.0, 0.0, -1.0);

  /* draw back face */
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;
    glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
    glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
    if (i < teeth) {
      glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
      glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
    }
  }
  glEnd();

  /* draw back sides of teeth */
  glBegin(GL_QUADS);
  da = 2.0 * M_PI / teeth / 4.0;
  for (i = 0; i < teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;

    glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
    glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), -width * 0.5);
    glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5);
    glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
  }
  glEnd();

  /* draw outward faces of teeth */
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i < teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;

    glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
    glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
    u = r2 * cos(angle + da) - r1 * cos(angle);
    v = r2 * sin(angle + da) - r1 * sin(angle);
    len = sqrt(u * u + v * v);
    u /= len;
    v /= len;
    glNormal3f(v, -u, 0.0);
    glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5);
    glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5);
    glNormal3f(cos(angle), sin(angle), 0.0);
    glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5);
    glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), -width * 0.5);
    u = r1 * cos(angle + 3 * da) - r2 * cos(angle + 2 * da);
    v = r1 * sin(angle + 3 * da) - r2 * sin(angle + 2 * da);
    glNormal3f(v, -u, 0.0);
    glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
    glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
    glNormal3f(cos(angle), sin(angle), 0.0);
  }

  glVertex3f(r1 * cos(0), r1 * sin(0), width * 0.5);
  glVertex3f(r1 * cos(0), r1 * sin(0), -width * 0.5);

  glEnd();

  glShadeModel(GL_SMOOTH);

  /* draw inside radius cylinder */
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;
    glNormal3f(-cos(angle), -sin(angle), 0.0);
    glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
    glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
  }
  glEnd();
  
   RETAILMSG( 1, ( "gear: tick=%d.\r\n", GetTickCount() - dwCurTick ) );

}

static GLfloat view_rotx = 20.0, view_roty = 30.0, view_rotz = 0.0;
static GLint gear1, gear2, gear3;
static GLfloat angle = 0.0;

static void
draw(void)
{
	DWORD dwCurTick =	GetTickCount();
	DWORD dwTick;
	
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //RETAILMSG( 1, ( "draw: tick0=%d.\r\n", (dwTick = GetTickCount()) - dwCurTick ) );
  //dwCurTick  = dwTick;

  glPushMatrix();
    glRotatef(view_rotx, 1.0, 0.0, 0.0);
    glRotatef(view_roty, 0.0, 1.0, 0.0);
    glRotatef(view_rotz, 0.0, 0.0, 1.0);

  //RETAILMSG( 1, ( "draw: tick1=%d.\r\n", (dwTick = GetTickCount()) - dwCurTick ) );
  //dwCurTick  = dwTick;


    glPushMatrix();
      glTranslatef(-3.0, -2.0, 0.0);
      glRotatef(angle, 0.0, 0.0, 1.0);
      glCallList(gear1);
    glPopMatrix();

  //RETAILMSG( 1, ( "draw: tick2=%d.\r\n", (dwTick = GetTickCount()) - dwCurTick ) );
  //dwCurTick  = dwTick;


    glPushMatrix();
      glTranslatef(3.1, -2.0, 0.0);
      glRotatef(-2.0 * angle - 9.0, 0.0, 0.0, 1.0);
      glCallList(gear2);
    glPopMatrix();

  //RETAILMSG( 1, ( "draw: tick3=%d.\r\n", (dwTick = GetTickCount()) - dwCurTick ) );
  //dwCurTick  = dwTick;


    glPushMatrix();
      glTranslatef(-3.1, 4.2, 0.0);
      glRotatef(-2.0 * angle - 25.0, 0.0, 0.0, 1.0);
      glCallList(gear3);
    glPopMatrix();

  //RETAILMSG( 1, ( "draw: tick4=%d.\r\n", (dwTick = GetTickCount()) - dwCurTick ) );
  //dwCurTick  = dwTick;

  glPopMatrix();

  
  glutSwapBuffers();
  Gwme_FlushGdi();

  //RETAILMSG( 1, ( "draw: tick5=%d.\r\n", (dwTick = GetTickCount()) - dwCurTick ) );
  //dwCurTick  = dwTick;


  Frames++;

  {
     GLint t = glutGet(GLUT_ELAPSED_TIME);
     if (t - T0 >= 5000) {
        GLfloat seconds = (t - T0) / 1000.0;
        GLfloat fps = Frames / seconds;
		RETAILMSG( 1, (TEXT("%d frames in %d seconds = %d FPS\r\n"),Frames, (int)seconds, (int)fps));
//        printf("%d frames in %6.3f seconds = %6.3f FPS\n", Frames, seconds, fps);
        T0 = t;
        Frames = 0;
        if ((t >= 999.0 * autoexit) && (autoexit))
           exit(0);
     }
  }
  
   RETAILMSG( 1, ( "draw: tick6=%d.\r\n", GetTickCount() - dwCurTick ) );
}


static void
idle(void)
{
  angle += 2.0;
  Sleep(50);
  glutPostRedisplay();
}

/* change view angle, exit upon ESC */
/* ARGSUSED1 */
static void
key(unsigned char k, int x, int y)
{
  switch (k) {
  case 'z':
    view_rotz += 5.0;
    break;
  case 'Z':
    view_rotz -= 5.0;
    break;
  case 27:  /* Escape */
	  {
		  PostQuitMessage( 0 );
	  }
    //exit(0);
    break;
  default:
    return;
  }
  glutPostRedisplay();
}

/* change view angle */
/* ARGSUSED1 */
static void
special(int k, int x, int y)
{
  switch (k) {
  case GLUT_KEY_UP:
    view_rotx += 5.0;
    break;
  case GLUT_KEY_DOWN:
    view_rotx -= 5.0;
    break;
  case GLUT_KEY_LEFT:
    view_roty += 5.0;
    break;
  case GLUT_KEY_RIGHT:
    view_roty -= 5.0;
    break;
  default:
    return;
  }
  glutPostRedisplay();
}

/* new window size or exposure */
static void
reshape(int width, int height)
{
	DWORD dwCurTick = GetTickCount();

  GLfloat h = (GLfloat) height / (GLfloat) width;

  glViewport(0, 0, (GLint) width, (GLint) height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -40.0);
  RETAILMSG( 1, ( "reshape: tick=%d.\r\n", GetTickCount() - dwCurTick ) );
}

static void
init(int argc, char *argv[])
{
  static GLfloat pos[4] = {5.0, 5.0, 10.0, 0.0};
  static GLfloat red[4] = {0.8, 0.1, 0.0, 1.0};
  static GLfloat green[4] = {0.0, 0.8, 0.2, 1.0};
  static GLfloat blue[4] = {0.2, 0.2, 1.0, 1.0};
  GLint i;

  glLightfv(GL_LIGHT0, GL_POSITION, pos);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);

  /* make the gears */
  gear1 = glGenLists(1);
  glNewList(gear1, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
  gear(1.0, 4.0, 1.0, 20, 0.7);
  glEndList();

  gear2 = glGenLists(1);
  glNewList(gear2, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
  gear(0.5, 2.0, 2.0, 10, 0.7);
  glEndList();

  gear3 = glGenLists(1);
  glNewList(gear3, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
  gear(1.3, 2.0, 0.5, 10, 0.7);
  glEndList();

  glEnable(GL_NORMALIZE);

  for ( i=1; i<argc; i++ ) {
    if (strcmp(argv[i], "-info")==0) 
	{
		//printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
		RETAILMSG( 1, (TEXT("GL_RENDERER = %s\r\n"), (char*)glGetString(GL_RENDERER) ) );
		//printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
		RETAILMSG( 1, (TEXT("GL_VERSION = %s\r\n"),(char*)glGetString(GL_VERSION)) );
		//printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
		RETAILMSG( 1, (TEXT("GL_VENDOR = %s\r\n"),(char*)glGetString(GL_VENDOR)) );
		//printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
		RETAILMSG( 1, (TEXT("GL_EXTENSIONS = %s\r\n"),(char*)glGetString(GL_EXTENSIONS)) );
    }
    else if ( strcmp(argv[i], "-exit")==0)
	{
      autoexit = 30;
      //printf("Auto Exit after %i seconds.\n", autoexit );
	  RETAILMSG( 1, (TEXT("Auto Exit after %d seconds.\r\n"), autoexit) );
    }
  }
}

static void 
visible(int vis)
{
  if (vis == GLUT_VISIBLE)
    glutIdleFunc(idle);
  else
    glutIdleFunc(NULL);
}

//int main(int argc, char *argv[])
int Run_Mesa_Gears( )
{
	int argc = 1;
	char *argv[] = { "TEST", 0 };

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(240, 300);
	//glutCreateWindow("Gears");
	glutCreateWindow("OpenGL-demo");
	init(argc, argv);

	glutDisplayFunc(draw);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(key);
	glutSpecialFunc(special);
	glutVisibilityFunc(visible);

	//glutMainLoop();
	return 0;             /* ANSI C requires main to return int. */
}

#define	ID_TIMER1	100
static LRESULT CALLBACK Mesa_Gears_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_TIMER:
			{
				if (ID_TIMER1==wParam)
				{
					KillTimer( hWnd, ID_TIMER1 );
					Run_Mesa_Gears( );
				}
			}
			return 0;
		case WM_DESTROY:
			{
				PostQuitMessage( 0 );
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

static TCHAR classMESA_GEARS[20] = "Mesa_Gears";
static ATOM Register_Mesa_Gears_Class( HINSTANCE hInstance )
{
	WNDCLASS    wc;
    
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)Mesa_Gears_Proc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= NULL;
	wc.hCursor			= NULL;
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= classMESA_GEARS;
 
	return RegisterClass(&wc);
}

#ifndef INLINE_PROGRAM
LRESULT CALLBACK WinMain( HINSTANCE hInstance,HINSTANCE hPrevInstance,
            LPSTR     lpCmdLine,int       nCmdShow )
#else
LRESULT CALLBACK WinMain_Gears( HINSTANCE hInstance,HINSTANCE hPrevInstance,
            LPSTR     lpCmdLine,int       nCmdShow )
#endif
{
#if 1
	HWND hWnd;
	HANDLE hRunEvent;

	SetLastError( 0 );
	hRunEvent = CreateEvent( NULL, FALSE, FALSE, "OpenGL_Demo" );
	if( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		hWnd = FindWindow( "GLUT", NULL ) ;  // ���ҵ�ǰ�����Ƿ��Ѿ�����
		SetForegroundWindow( hWnd ) ; // ���ô��ڵ���ǰ��
		SetEvent( hRunEvent );
		return 0;
	}
	
	Run_Mesa_Gears();	
	
	
	while(1)
	{
		glutMainLoop();
		WaitForSingleObject( hRunEvent, -1 );
	}

	return 0;
#else
	MSG msg;
	HWND hWnd = NULL;

	Register_Mesa_Gears_Class( hInstance );	// ע�ᴰ��
	hWnd = CreateWindowEx( WS_EX_CLOSEBOX,classMESA_GEARS, "Gears", WS_VISIBLE|WS_POPUP|WS_CAPTION,
			0, 0,
			240,
			320,
			0,0,hInstance,0 );
	ShowWindow(hWnd, TRUE); // ��ʾ����

	while( GetMessage(&msg, NULL, 0, 0) ) 
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	return msg.wParam;
#endif
}