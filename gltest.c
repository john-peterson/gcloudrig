#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>

#include <GL/gl.h>
#include <GL/glx.h>
//#include <GL/glew.h>
//#include <GL/glut.h>



int singleBufferAttributess[] = {
	GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
	GLX_RENDER_TYPE,   GLX_RGBA_BIT,
	GLX_RED_SIZE,      1,   /* Request a single buffered color buffer */
	GLX_GREEN_SIZE,    1,   /* with the maximum number of color bits  */
	GLX_BLUE_SIZE,     1,   /* for each component                     */
	None
};

int doubleBufferAttributes[] = {
	GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
	GLX_RENDER_TYPE,   GLX_RGBA_BIT,
	GLX_DOUBLEBUFFER,  True,  /* Request a double-buffered color buffer with */
	GLX_RED_SIZE,      1,     /* the maximum number of bits per component    */
	GLX_GREEN_SIZE,    1,
	GLX_BLUE_SIZE,     1,
	None
};

static Bool WaitForNotify( Display *dpy, XEvent *event, XPointer arg ) {
	return (event->type == MapNotify) && (event->xmap.window == (Window) arg);
}

int main() {
	// printf() displays the string inside quotation
	printf("Hello, World!");


	Display              *dpy;
	Window                xWin;
	XEvent                event;
	XVisualInfo          *vInfo;
	XSetWindowAttributes  swa;
	GLXFBConfig          *fbConfigs;
	GLXContext            context;
	GLXWindow             glxWin;
	int                   swaMask;
	int                   numReturned;
	int                   swapFlag = True;


	dpy = XOpenDisplay( NULL );
	if ( dpy == NULL ) {
		printf( "Unable to open a connection to the X servern" );
		exit( EXIT_FAILURE );
	}

	fbConfigs = glXChooseFBConfig( dpy, DefaultScreen(dpy), doubleBufferAttributes, &numReturned );

	if ( fbConfigs == NULL ) {  /* no double buffered configs available */
		fbConfigs = glXChooseFBConfig( dpy, DefaultScreen(dpy), singleBufferAttributess, &numReturned );
		swapFlag = False;
	}


	vInfo = glXGetVisualFromFBConfig( dpy, fbConfigs[0] );

	swa.border_pixel = 0;
	swa.event_mask = StructureNotifyMask;
	swa.colormap = XCreateColormap( dpy, RootWindow(dpy, vInfo->screen),
	                                vInfo->visual, AllocNone );

	swaMask = CWBorderPixel | CWColormap | CWEventMask;

	xWin = XCreateWindow( dpy, RootWindow(dpy, vInfo->screen), 0, 0, 256, 256,
	                      0, vInfo->depth, InputOutput, vInfo->visual,
	                      swaMask, &swa );

	/* Create a GLX context for OpenGL rendering */
	context = glXCreateNewContext( dpy, fbConfigs[0], GLX_RGBA_TYPE,
	                               NULL, True );

	printf("GL version string: '%s'\n", glGetString(GL_VERSION));


	/* Create a GLX window to associate the frame buffer configuration
	** with the created X window */
	glxWin = glXCreateWindow( dpy, fbConfigs[0], xWin, NULL );

	printf("GL version string: '%s'\n", glGetString(GL_VERSION));


	/* Map the window to the screen, and wait for it to appear */
	XMapWindow( dpy, xWin );
	XIfEvent( dpy, &event, WaitForNotify, (XPointer) xWin );

	printf("GL version string: '%s'\n", glGetString(GL_VERSION));


	/* Bind the GLX context to the Window */
	glXMakeContextCurrent( dpy, glxWin, glxWin, context );

	printf("GL version string: '%s'\n", glGetString(GL_VERSION));


	/* OpenGL rendering ... */
	glClearColor( 1.0, 1.0, 0.0, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT );

	glFlush();

	if ( swapFlag )
		glXSwapBuffers( dpy, glxWin );

	printf("GL version string: '%s'\n", glGetString(GL_VERSION));

	sleep(10);
	return 0;
}