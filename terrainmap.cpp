/*
	Assignment 3 - Terrain Map
		Due: November 28th, 2014 - 5:00PM

	Teacher: Robert Teather
	Course: 3GC3 - Graphics

	Student 1:
		Name: Jasman Gill
		Student ID: 1211554

	Student 2:
		Name: Jesse Truong
		Student ID: 1222722

	Some of the code was referenced off of other sites. These sites include:
		http://www.glprogramming.com/red/chapter05.html
		http://www.lighthouse3d.com/opengl/terrain/index.php3?introduction
*/


#include <stdio.h>
#include <stdlib.h>
#ifdef __APPLE__			//headers only included on a mac
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif
#include <time.h>
#include <math.h>

#include <list>
#include<iostream>
using namespace std;

float tsize;					//size of terrain
int cameraMode = 0;				//keeps track of toggled camera
int wireframe = 0;				//keeps track of toggled polygonmode
float temp;						//temp is used as guard/check
float scaleFactor = 1.0;		//used to zoom camera in on a point
float fovY = 45;				//used in zooming
float campos[]={50,25,75};		//camera position
float look[] = { 0, 0, 0 }; 	//camera position
float height[400][400];			//height values of vertices 
float normalArray[400][400][3];	//stores the normal of arrays
float maxHeight = 10;			//max height of y values
float light_position[] = { 10.0, -10.0, 10.0, 0 };			//position of light one
float light_positionTwo[] = { -10.0, -10.0, -10.0, 0 };		//position of light two

bool lighting = false;			//toggles lights 
bool smoothShading = true;		//toggles shading

bool snow=false;				//toggles snow effect
float size=0.1;					//size of snow flakes
bool wind=false;				//toggles wind

typedef struct atom{
	float px,py,pz;			//current atom position
	float dx,dy,dz;			//movement direction
	float rot;				//rotation angle
	float col,col2,col3;	//colors
	float age;				//particle age
	}atom;
	
std::list<atom> shape;

//Function to create the droplets for the rain
void makeSnow(){																								//attributes of rain particles
	shape.push_back(atom());
	shape.back().px= 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(tsize - 0)));			//particles are created randomly...
	shape.back().py= 10;																						//...over plain
	shape.back().pz= (0) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(tsize - 0)));
	shape.back().dx= 0;			
	shape.back().dy=(-0.1);																						//movement in the y direction
	shape.back().dz=0;																				
	shape.back().col=1;																							//colour of snowflake
	shape.back().age=90;																						//age of snowflake
	}

//Function to move the snowflakes
void moveSnow(){
	for(list<atom>::iterator count = shape.begin();count != shape.end();count++){	//goes through all of the particles in list
		if( ((count->age)<=0) || (count->py<2.1)) {
			count=shape.erase(count);					//deletes particles that leave set bounds
			}	
		if(wind==true){
			count->px=(count->px)+0.2;					//x direction is introdced if wind is on
			}
		count->py=(count->py)+(count->dy);				//only y direction is affected in rain
		count->age=(count->age)-1;						//age counter/decrement
		}
	}

//Function to render the snow
void renderSnow(){
	for(list<atom>::iterator counts = shape.begin();counts!= shape.end();counts++){	//goes through all particles in list
		glPushMatrix();											
		glTranslatef(counts->px,counts->py,counts->pz);			//positions particle
		glRotatef(counts->rot,1,1,1);							//rotates particle
		glColor3f(counts->col,counts->col,counts->col);			//color snow white
		glutSolidSphere(size,30,30);							//creates sphere shaped particles
		glPopMatrix();
		}
	glutPostRedisplay();
	}

//Function to make the terrain and colour it accordingly
void makeTerrain(){								
		
	float ambient[4] = {0.2, 0.8, 0.1, 1};					//ambient light values
	float diffuse[4] = {0.2, 0.8, 0.1, 1};					//diffuse light values
	float specular[4] = {1, 1, 1, 1};						//specular light values

	//Set the values for the materials
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20);

    
    if (wireframe == 2){
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        for(int x=0;x<=tsize-1;x++){
            for(int z=0;z<=tsize-1;z++){
                
                //When you do height / maxHeight, we get numbers according to how high up the mountain is
                //This enables having black near lower highest and gradually move towards white at the top
                glBegin(GL_QUADS);
                glColor3f(height[x][z] / maxHeight, height[x][z] / maxHeight, height[x][z] / maxHeight);
                glNormal3fv(normalArray[x][z]);
                glVertex3f(x,height[x][z],z);
                
                glColor3f(height[x + 1][z] / maxHeight, height[x + 1][z] / maxHeight, height[x+1][z] / maxHeight);
                glNormal3fv(normalArray[x + 1][z]);
                glVertex3f(x+1,height[x + 1][z],z);
                
                glColor3f(height[x + 1][z + 1] / maxHeight, height[x + 1][z + 1] / maxHeight, height[x + 1][z + 1] / maxHeight);
                glNormal3fv(normalArray[x + 1][z + 1]);
                glVertex3f(x+1,height[x+1][z+1],z+1);
                
                glColor3f(height[x][z + 1] / maxHeight, height[x][z + 1] / maxHeight, height[x][z + 1] / maxHeight);
                glNormal3fv(normalArray[x][z + 1]);
                glVertex3f(x,height[x][z+1],z+1);
                glEnd();
                
            }
        }
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
    }
    
    
	for(int x=0;x<=tsize-1;x++){
		for(int z=0;z<=tsize-1;z++){
			
			//When you do height / maxHeight, we get numbers according to how high up the mountain is
			//This enables having black near lower highest and gradually move towards white at the top
			glBegin(GL_QUADS);
				glColor3f(height[x][z] / maxHeight, height[x][z] / maxHeight, height[x][z] / maxHeight);
				glNormal3fv(normalArray[x][z]);
				glVertex3f(x,height[x][z],z);
				
				glColor3f(height[x + 1][z] / maxHeight, height[x + 1][z] / maxHeight, height[x+1][z] / maxHeight);
				glNormal3fv(normalArray[x + 1][z]);
				glVertex3f(x+1,height[x + 1][z],z);
				
				glColor3f(height[x + 1][z + 1] / maxHeight, height[x + 1][z + 1] / maxHeight, height[x + 1][z + 1] / maxHeight);
				glNormal3fv(normalArray[x + 1][z + 1]);
				glVertex3f(x+1,height[x+1][z+1],z+1);
				
				glColor3f(height[x][z + 1] / maxHeight, height[x][z + 1] / maxHeight, height[x][z + 1] / maxHeight);
				glNormal3fv(normalArray[x][z + 1]);
				glVertex3f(x,height[x][z+1],z+1);
			glEnd();
			
		}
	}
}

//Function to create the heights of all the peaks on the terrain
void heightmap(){
	
	int iterations = 1000;			//number of times the algorithm is run
	
	for (int i = 0; i < iterations; i++) {						//fault algorithm
		float v = rand();
		float a = sinf(v);
		float b = cosf(v);
		float d = sqrtf(tsize*tsize + tsize*tsize);
		float c = ((double) rand() / RAND_MAX) * d - d/2.0;

		float displacement = 0.5;
	
		for(int x=0;x<=tsize-1;x++){								//running through every vertices
			for(int z=0; z<=tsize-1;z++){
			
				if ((a*x + b*z - c) > 0){
					if (height[x][z]+displacement < maxHeight)		//height values, being changed
						height[x][z] += displacement;
				}
				else{
					if (height[x][z]-displacement > 0)
						height[x][z] -= displacement;
				}
			}
		}
	}
}

//Function to make normal vectors
void makeVector(){

	double l;
	float v1[3], v2[3], vv[3];   //vv will be the cross product of v1 and v2

	for (int i = 0; i < tsize - 1; i++){
		for (int j = 0; j < tsize - 1; j++){

			v1[0] = i + 1;                                 //Calculate the x,y,z coordinates of two vectors v1 v2
			v2[0] = i + 1;
			v1[1] = height[i + 1][j] - height[i][j];
			v2[1] = height[i + 1][j + 1] - height[i][j];
			v1[2] = j;
			v2[2] = j + 1;
			vv[0] = v1[1] * v2[2] - v1[2] * v2[1];            //Cross the two vectors for a new vector
			vv[1] = v1[2] * v2[0] - v1[0] * v2[2];
			vv[2] = v1[0] * v2[1] - v1[1] * v2[0];

			l = sqrtf(vv[0] * vv[0] + vv[1] * vv[1] + vv[2] * vv[2]);  //Determine the length to normalise vectors

			vv[0] = vv[0] / l;       //Make vectors into unit length
			vv[1] = vv[1] / l;
			vv[2] = vv[2] / l;

			normalArray[i][j][0] = vv[0];  //Store values in an array
			normalArray[i][j][1] = vv[1];
			normalArray[i][j][2] = vv[2];
		}
	}
}

//Display function
void display(void){

	//everything is displayed/rendered here
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(campos[0], campos[1], campos[2], look[0], look[1], look[2], 0, 1, 0);		//camera
	glColor3f(1, 1, 1);
	makeTerrain();																		//terrain created

	if (snow==true){										//toggle snow
		renderSnow();
		}

	glutSwapBuffers();										//double buffering

}

//Keyboard function
void keyboard(unsigned char key, int x, int y){
	switch (key){
		case 'q':					//exit program
		case 'Q':
		case 27:
			exit(0);
			break;
	
		//reset the terrain
		case 'r':
		case 'R':
			display();
			heightmap();
			makeVector();
			break;

		//Toggle between the three different wireframing modes
		case 'w':
		case 'W':
			if (wireframe == 0){
				wireframe = 1;

				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else if (wireframe == 1){
				wireframe = 2;
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			else{
				wireframe = 0;

				//BOTH OF THEM - PUT HEREEEEEE
			}
			break;

		//Switch between the two different types of shading
		case 's':
		case 'S':
			smoothShading = !smoothShading;
			if (smoothShading){
				glShadeModel(GL_SMOOTH);
			}
			else{
				glShadeModel(GL_FLAT);
			}
			break;
	
		//Enable and disable rain
		case 'y':					//rain toggle
		case 'Y':
			if (snow == false){
				snow = true;              //Turn rain on
				shape.clear();
			}
			else{
				snow = false;             //Turn rain off
				shape.clear();
			}
			break;
		
		//Enable and disable wind
		case 'u':
		case 'U':
			if (wind == false){
				wind = true;              //Enable wind
			}
			else{
				wind = false;            //Disable wind
			}
			break;
		
		//Enable and disable lighting
		case 'l':
		case 'L':
			if (lighting){
				lighting = false;
				glDisable(GL_LIGHTING);   //Disable the lighting
				glDisable(GL_LIGHT0);     //Disable first light
				glDisable(GL_LIGHT1);     //Disable second light
			}
			else{
				lighting = true;
				glEnable(GL_LIGHTING);   //Enable lighting
				glEnable(GL_LIGHT0);     //Enable first light
				glEnable(GL_LIGHT1);    //Enable second light
			}
			break;

		case '-':					//increase in z-axis direction
			campos[2] = campos[2] + 0.2;
			break;

		case '+':					//decrease in z-axis direction
			campos[2] = campos[2] - 0.2;
			break;

		//Zoom out hotkey
		case '[':
		case '{':
			if (scaleFactor >= 0.4){
				scaleFactor -= 0.2;
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				gluPerspective(fovY * scaleFactor, 1, 1, 100);  //multiply the field of view by a factor to zoom

			}
			break;

		//Zoom in hotkey
		case ']':
		case '}':
			if (scaleFactor <= 3){
				scaleFactor += 0.1;
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				gluPerspective(fovY * scaleFactor, 1, 1, 100);  //multiply the field of view by a factor to zoom
			}
				break;
		
		//check for which camera mode is currently operational
		case '1':
			cameraMode = 0;    //Normal camera mode
			break;

		case '2':
			cameraMode = 1;   //Change view (change where the eye is pointing)
			break;

		case '3':
			cameraMode = 2;   //Change position of light one
			break;

		case '4':
			cameraMode = 3;    //Change position of light two
	}
}
	
//Special keys function
void special(int key, int x, int y){	//special keys

	switch (key){
		case GLUT_KEY_LEFT:				//check for camera mode and then rotate the proper camera accordingly
			if (cameraMode == 0){
				campos[0] = campos[0] - 0.2;   //Rotates camera left
			}else if (cameraMode == 1){
				look[0] = look[0] - 0.8;       //Looks left
			}else if (cameraMode == 2){
				light_position[0] = light_position[0] - 0.5;             //Moves lighting source one left
				glLightfv(GL_LIGHT0, GL_POSITION, light_position);       //Change lighting position
			}else{
				light_positionTwo[0] = light_positionTwo[0] - 0.5;       //Moves lighting source two left
				glLightfv(GL_LIGHT1, GL_POSITION, light_positionTwo);    //Change lighting position
			}
			break;

		case GLUT_KEY_UP:				//Same as above but up
			if (cameraMode == 0){
				campos[1] = campos[1] + 0.2;
			}
			else if (cameraMode == 1){
				look[1] = look[1] + 0.8;
			}
			else if (cameraMode == 2){
				light_position[1] = light_position[1] + 0.5;
				glLightfv(GL_LIGHT0, GL_POSITION, light_position);
			}
			else{
				light_positionTwo[1] = light_positionTwo[1] + 0.5;
				glLightfv(GL_LIGHT1, GL_POSITION, light_positionTwo);
			}
			break;

		case GLUT_KEY_RIGHT:			//Same as above but to the right
			if (cameraMode == 0){
				campos[0] = campos[0] + 0.2;
			}
			else if (cameraMode == 1){
				look[0] = look[0] + 0.8;
			}
			else if (cameraMode == 2){
				light_position[0] = light_position[0] + 0.5;
				glLightfv(GL_LIGHT0, GL_POSITION, light_position);
			}
			else{
				light_positionTwo[0] = light_positionTwo[0] + 0.5;
				glLightfv(GL_LIGHT1, GL_POSITION, light_positionTwo);
			}
			break;

			break;

		case GLUT_KEY_DOWN:				//Same as above but down
			if (cameraMode == 0){
				campos[1] = campos[1] - 0.2;
			}
			else if (cameraMode == 1){
				look[1] = look[1] - 0.8;
			}
			else if (cameraMode == 2){
				light_position[1] = light_position[1] - 1;
				glLightfv(GL_LIGHT0, GL_POSITION, light_position);
			}
			else{
				light_positionTwo[1] = light_positionTwo[1] - 1;
				glLightfv(GL_LIGHT1, GL_POSITION, light_positionTwo);
			}
			break;

	}
}

//Initial function
void init(void){
	
	float ambient[4] = { 1, 0, 0, 1 };        //Ambient values
	float diffuse[4] = { 1, 0, 0, 1 };        //Diffuse values
	float specular[4] = { 1, 1, 1, 1 };       //Specular values

	glClearColor(0, 0, 0, 0);
	glShadeModel(GL_SMOOTH);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);  //Set light position
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);          //Change default values of GL_LIGHT1
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light_positionTwo);  //Set light two position
	glEnable(GL_DEPTH_TEST);

	glColor3f(1,1,1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovY,1,1,100);

}
	
//Timer function
void timer(int value){				//keeps track of internal clock
	
	if (snow==true){				//particle stream toggle
		makeSnow();
		moveSnow();
	}

	glutPostRedisplay();
    glutTimerFunc(20, timer, 0);   	//timer function
}	
	
//Reshape function
void reshape(int w, int h){			//used to reshape window

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);

	if (w <= h){
		glOrtho(-1.5, 1.5, -1.5*(GLfloat)h / (GLfloat)w,
			1.5*(GLfloat)h / (GLfloat)w, -10.0, 10.0);
	}else{
		glOrtho(-1.5*(GLfloat)w / (GLfloat)h,
			1.5*(GLfloat)w / (GLfloat)h, -1.5, 1.5, -10.0, 10.0);
	}

	glLoadIdentity();
	gluPerspective(fovY, 1, 1, 100);
	glMatrixMode(GL_MODELVIEW);
}
	
//Main function
int main(int argc, char** argv){
	
	srand(time(NULL));
	
	printf("Use the following commands(lower or upper case are both fine)\n\n");
	printf("1 - Mode 1: Camera Positioning(rotates camera around a point)\n");
	printf("2 - Mode 2: Camera Movement(rotates camera about itself\n");
	printf("3 - Mode 3: Light One Movement\n");
	printf("4 - Mode 4: Light Two Movement\n");
	printf("Arrow Keys - Movement of the Current Mode\n\n");
	printf("'[ ]' - Zoom In('[') Out(']')\n");
	printf("W - Toggle Wireframe Modes\n");
	printf("R - Reset The Terrain");
	printf("S - Toggle Shading\n");
	printf("L - Toggle Lighting\n");
	printf("Y - Toggle Snow\n");
	printf("U - Toggle Wind\n");
	printf("Q or ESC - Exit the Program\n\n");

	cout << "Enter terrain size y. (y by y wide): ";			//asking user for terrain size (prompt)
	cin >> temp;
	while (!(temp >= 50 && temp <= 300)) {
		cout << "Try again: ";
		cin >> temp;
	}
	tsize = temp;
	glutInit(&argc,argv);										
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800,800);		
	glutInitWindowPosition(100,0);
	glutCreateWindow("Terrain Map");
	
	glutDisplayFunc(display);					
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutTimerFunc(0.05, timer, 0);	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glutReshapeFunc(reshape);
	init();
	heightmap();
	makeVector();

	glutMainLoop();						//loop begins here
	return(0);
	}