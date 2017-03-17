
#include <math.h>
#include <gl\glut.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tga.h"
#include "terrain.h"


int bla = 0;

// stuff for lighting
GLfloat lAmbient[] = {0.2,0.2,0.2,1.0};
GLfloat lDiffuse[] = {1.0,1.0,1.0,1.0};
GLfloat lSpecular[] = {1.0,1.0,1.0,1.0};

GLfloat lPosition[] = {0.0, 100.0, 0.0, 0.0};

// materials
GLfloat mSpecular[] = {0.0,0.0,0.0,0.0};
// the smaller the larger the specular area is
GLfloat mShininess[] = {128.0};

//colors
GLfloat cBlack[] = {0.0,0.0,0.0,1.0};
GLfloat cOrange[] = {1.0,0.5,0.5,1.0}; 
GLfloat cWhite[] = {1.0,1.0,1.0,1.0}; 
GLfloat cGrey[] = {0.1,0.1,0.1,1.0};
GLfloat cLightGrey[] = {0.9,0.9,0.9,1.0};

int lighting=1,simulateLighting = 0;

/* this is not doing anything at the moment */
#define FLY		1
#define WALK	2
int navigationMode = WALK;


float angle=0.0,deltaAngle = 0.0,ratio;
float x=0.0f,y=1.75f,z=5.0f;
float lx=0.0f,ly=0.0f,lz=-1.0f,deltaMove=0.0;
int h,w;
int font=(int)GLUT_BITMAP_8_BY_13;
static GLint snowman_display_list;
int bitmapHeight=13;
int mode;
float angle2,angle2Y,angleY;
static int deltaX=-1000,deltaY;

int terrainDL,iterations = 0,totalIterations = 0;
char s[100];

int frame,time,timebase=0;
char currentMode[100];

// this string keeps the last good setting 
// for the game mode
char gameModeString[40] = "640x480";



void init();

void changeSize(int w1, int h1)
	{

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h1 == 0)
		h1 = 1;

	w = w1;
	h = h1;
	ratio = 1.0f * w / h;
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set the clipping volume
	gluPerspective(45,ratio,0.1,1000);

	// setting the camera now
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x, y, z, 
		      x + lx,y + ly,z + lz,
			  0.0f,1.0f,0.0f);
}


void initScene() {

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	terrainSimulateLighting(simulateLighting);
	terrainDiffuseColor(1.0, 1.0, 1.0);
	terrainAmbientColor(0.04, 0.04, 0.04);
	terrainLightPosition(lPosition[0],lPosition[1],lPosition[2],lPosition[3]);
	terrainDL = terrainCreateDL(0,0,0,lighting);
	y = terrainGetHeight(0,0) + 1.75;
	
	glLightfv(GL_LIGHT0,GL_AMBIENT,lAmbient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,lDiffuse);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

void orientMe(float ang) {

	lx = cos(angle2Y) * sin(ang);
	lz = - cos(angle2Y) * cos(ang);
}


void moveMeFlat(float i) {

	x = x + i*lx;
	z = z + i*lz;
	y = y + i*ly;
}

void setOrthographicProjection() {

	// switch to projection mode
	glMatrixMode(GL_PROJECTION);
	// save previous matrix which contains the 
	//settings for the perspective projection
	glPushMatrix();
	// reset matrix
	glLoadIdentity();
	// set a 2D orthographic projection
	gluOrtho2D(0, w, 0, h);
	// invert the y axis, down is positive
	glScalef(1, -1, 1);
	// mover the origin from the bottom left corner
	// to the upper left corner
	glTranslatef(0, -h, 0);
	glMatrixMode(GL_MODELVIEW);
}

void resetPerspectiveProjection() {
	// set the current matrix to GL_PROJECTION
	glMatrixMode(GL_PROJECTION);
	// restore previous settings
	glPopMatrix();
	// get back to GL_MODELVIEW matrix
	glMatrixMode(GL_MODELVIEW);
}

void renderBitmapString(float x, float y, void *font,char *string)
{
  
  char *c;
  // set position to start drawing fonts
  glRasterPos2f(x, y);
  // loop all the characters in the string
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(font, *c);
  }
}


void renderScene(void) {

	float modelview[16];

	if (deltaMove)
		moveMeFlat(deltaMove);
	if (deltaAngle) {
		angle += deltaAngle;
		orientMe(angle);
	}
	glLoadIdentity();
	gluLookAt(x, y, z, 
		      x + 10*lx,y + 10*ly,z + 10*lz,
			  0.0f,1.0f,0.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (lighting)
		glLightfv(GL_LIGHT0,GL_POSITION,lPosition);

// Draw ground

	glMaterialfv(GL_FRONT, GL_SPECULAR, mSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS,mShininess);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, cWhite);
	if (lighting) {
		glColorMaterial(GL_FRONT, GL_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);
	}
	else
		glDisable(GL_COLOR_MATERIAL);

	glColor3f(1,1,1);
	glCallList(terrainDL);


	glColor3f(1,0,0);
	glRotatef(90, 0,1,0);
	glTranslatef(0, 20, -15);



	frame++;
	time=glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		sprintf(s,"FPS:%4.2f",frame*1000.0/(time-timebase));
		timebase = time;		
		frame = 0;
	}
	glPushAttrib(GL_LIGHTING);
	glDisable(GL_LIGHTING);
	glColor3f(0.0f,1.0f,1.0f);
	setOrthographicProjection();
	glPushMatrix();
	glLoadIdentity();
	renderBitmapString(30,15,(void *)font,"Terrain Tutorial @ 3D Tech"); 
	renderBitmapString(30,30,(void *)font,s); 
	renderBitmapString(30,45,(void *)font,"F1  - Game Mode  640x480 32 bits");
	renderBitmapString(30,60,(void *)font,"F2  - Game Mode  800x600 32 bits");
	renderBitmapString(30,75,(void *)font,"F3  - Game Mode 1024x768 32 bits");
	renderBitmapString(30,90,(void *)font,"F4  - Window Mode");
	renderBitmapString(30,105,(void *)font,"F10 - Simulate Lighting ON/OFF");
	renderBitmapString(30,120,(void *)font,"F11 - OpenGL Lighting ON/OFF");
	renderBitmapString(30,135,(void *)font,"F12 - Grab Screen");
	renderBitmapString(30,150,(void *)font,"Esc - Quit");
	renderBitmapString(30,165,(void *)font,currentMode);
	glPopMatrix();
	resetPerspectiveProjection();
	glPopAttrib();
	glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int y) {

	if (key == 27) {
		terrainDestroy();
		exit(0);
	}
}

void pressKey(int key, int x, int y) {

	switch (key) {
		case GLUT_KEY_LEFT : deltaAngle = -0.005f;break;
		case GLUT_KEY_RIGHT : deltaAngle = 0.005f;break;
		case GLUT_KEY_UP : 
			if (navigationMode == FLY)
				deltaMove = 1;
			else
				deltaMove = 0.1;
			break;
		case GLUT_KEY_DOWN : 			
			if (navigationMode == FLY)
				deltaMove = -1;
			else
				deltaMove = -0.1;
			break;
		case GLUT_KEY_F1:  
			
			// define resolution, color depth
			glutGameModeString("640x480:32");
			// enter full screen
			if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)) {
				glutEnterGameMode();
				sprintf(gameModeString,"640x480:32");
				w = 640;
				h = 480;
				// register callbacks again 
				// and init OpenGL context
				init();
			}
			else
				glutGameModeString(gameModeString);
			break;
		case GLUT_KEY_F2:     
			// define resolution, color depth
			glutGameModeString("800x600:32");
			// enter full screen
			if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)) {
				glutEnterGameMode();
				sprintf(gameModeString,"800x600:32");
				w = 800;
				h = 600;
				// register callbacks again 
				// and init OpenGL context
				init();
			}
			else
				glutGameModeString(gameModeString);
			break;
		case GLUT_KEY_F3:  
			// define resolution, color depth
			glutGameModeString("1024x768:32");
			// enter full screen
			if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)) {
				glutEnterGameMode();
				w = 1024;
				h = 768;
				sprintf(gameModeString,"1024x768:32");
				// register callbacks again 
				// and init OpenGL context
				init();
			}
			else
				glutGameModeString(gameModeString);
			break;
		case GLUT_KEY_F4:  
			// return to default window
			w = 640;h = 360;
			if (glutGameModeGet(GLUT_GAME_MODE_ACTIVE) != 0)
				glutLeaveGameMode();
			break;
		case GLUT_KEY_F5:
			bla = !bla;
			break;
		case GLUT_KEY_F10:
			simulateLighting = !simulateLighting;
			terrainSimulateLighting(simulateLighting);
			glDeleteLists(terrainDL,1);
			if (simulateLighting)
				lighting = 0;
			if (lighting) {
				glEnable(GL_LIGHTING);
				glEnable(GL_LIGHT0);
			}
			else {
				glDisable(GL_LIGHT0);
				glDisable(GL_LIGHTING);
			}
			terrainDL = terrainCreateDL(0,0,0,0);
			break;
		case GLUT_KEY_F11:
			lighting = !lighting;
			if (lighting) {
				glEnable(GL_LIGHTING);
				glEnable(GL_LIGHT0);
			}
			else {
				glDisable(GL_LIGHT0);
				glDisable(GL_LIGHTING);
			}
			glDeleteLists(terrainDL,1);
			if (lighting)
				simulateLighting = 0;
			terrainDL = terrainCreateDL(0,0,0,lighting);
			break;
		case GLUT_KEY_F12:
			tgaGrabScreenSeries("3dtechscreen",0,0,w,h);
			break;
	}
	if (glutGameModeGet(GLUT_GAME_MODE_ACTIVE) == 0)
		sprintf(currentMode,"Current Mode: Window - lighting : %d simulate lighting %d",lighting,simulateLighting);
	else
		sprintf(currentMode,
			"Current Mode: Game Mode %dx%d at %d hertz, %d bpp - lighting : %d simulate lighting %d",
			glutGameModeGet(GLUT_GAME_MODE_WIDTH),
			glutGameModeGet(GLUT_GAME_MODE_HEIGHT),
			glutGameModeGet(GLUT_GAME_MODE_REFRESH_RATE),
			glutGameModeGet(GLUT_GAME_MODE_PIXEL_DEPTH),
			lighting,simulateLighting);
}

void releaseKey(int key, int x, int y) {

	switch (key) {
		case GLUT_KEY_LEFT : if (deltaAngle < 0.0f) 
								 deltaAngle = 0.0f;
							 break;
		case GLUT_KEY_RIGHT : if (deltaAngle > 0.0f) 
								 deltaAngle = 0.0f;
							 break;
		case GLUT_KEY_UP :	 if (deltaMove > 0) 
								 deltaMove = 0;
							 break;
		case GLUT_KEY_DOWN : if (deltaMove < 0) 
								 deltaMove = 0;
							 break;
	}
}


void activeMouseMotion(int x, int y) {

		angle2 = angle + (x-deltaX)*0.001;
		angle2Y = angleY + (y-deltaY) * 0.001;
		if (angle2Y > 1.57)
			angle2Y = 1.57;
		else if (angle2Y < -1.57)
			angle2Y = -1.57;
		lx = cos(angle2Y)*sin(angle2);
		lz = -cos(angle2Y)*cos(angle2);
		ly = -sin(angle2Y);
}


void mousePress(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) {
//		angle2 = 0;
		deltaX = x;
		deltaY = y;
//		angle2Y = 0;
		navigationMode = FLY;
	} 
	else if (state == GLUT_UP) {
		angleY = angle2Y;
		angle = angle2;
		navigationMode = WALK;
	}
}





void init() {
	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(pressKey);
	glutSpecialUpFunc(releaseKey);
	glutMotionFunc(activeMouseMotion);
	glutMouseFunc(mousePress);
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	initScene();

}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(640,360);
	glutCreateWindow("SnowMen from 3D-Tech");

	// init terrain structures
	if (terrainLoadFromImage("3dtech.tga",1) != TERRAIN_OK)
		return(-1);
	terrainScale(0,20);
	// register all callbacks and
	// create display lists
	init();

	glutMainLoop();

	return(0);
}
