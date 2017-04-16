#include "GL\glew.h"
#include "GL\freeglut.h"
#include <iostream>
#include <cmath>

using namespace std;

GLint windowWidth = 1280;                    // Width of our window
GLint windowHeight = 720;                    // Height of our window

//cursor position:-
int xpos, ypos;

//update the cursor positions:-
void update(int x, int y) {
	xpos = x;
	ypos = y;
}

// Function to set flags according to which keys are pressed or released
void handleKeypressUp(unsigned char theKey, int x, int y){
	switch (theKey)
	{
	default:
		break;
	}
}

//for drawing a cylindrical branch from (0,0,0) to (x,y,z)
void drawBranch(float x, float y, float z) {
	float r = sqrt(x*x + y*y + z*z);
	float phi = atan2(x , z)*180/3.14f;
	float theta = asin(y / r)*180/3.14f;
	//cout << phi << " " << theta << endl;
	glRotatef((GLfloat)phi, 0.0, 1.0, 0.0);
	glRotatef((GLfloat)(-theta), 1.0, 0.0, 0.0);
	glutSolidCylinder(0.3f, r, 20, 20);

}

void drawModel() {
	// Add a sphere to the scene.
	glPushMatrix();
		drawBranch(1.0, 1.0, 1.0);
	glPopMatrix();
	glPushMatrix();
		drawBranch(0.0, 1.0, 0.0);
	glPopMatrix();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	
	glPushMatrix();
		drawModel();
	glPopMatrix();

	glutSwapBuffers();
}


void reshape(GLint w, GLint h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	GLfloat aspect = GLfloat(w) / GLfloat(h);
	glLoadIdentity();
	if (w <= h) {
		// width is smaller, so stretch out the height
		glOrtho(-2.5, 2.5, -2.5 / aspect, 2.5 / aspect, -10.0, 10.0);
	}
	else {
		// height is smaller, so stretch out the width
		glOrtho(-2.5*aspect, 2.5*aspect, -2.5, 2.5, -10.0, 10.0);
	}
}


void init() {
	GLfloat black[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat yellow[] = { 1.0, 1.0, 0.0, 1.0 };
	GLfloat brown[] = { 0.4f, 0.2f, 0.0f, 1.0 };
	GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat direction[] = { 1.0, 1.0, 1.0, 0.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, brown);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glMaterialf(GL_FRONT, GL_SHININESS, 30);

	glLightfv(GL_LIGHT0, GL_AMBIENT, black);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
	glLightfv(GL_LIGHT0, GL_POSITION, direction);

	glEnable(GL_LIGHTING);                // so the renderer considers light
	glEnable(GL_LIGHT0);                  // turn LIGHT0 on
	glEnable(GL_DEPTH_TEST);              // so the renderer considers depth
	glEnable(GL_NORMALIZE);
}

// The usual application statup code.
int main(int argc, char** argv) {
	// init GLUT and create window
	glutInit(&argc, argv);
	//double buffer window, color model - RGB, depth buffer
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowPosition(40, 20);// First we establish the windows position, i.e. its top left corner.
	glutInitWindowSize(windowWidth, windowHeight);//prototype - void glutInitWindowSize(int width, int height)
	//create Window
	glutCreateWindow("Botanical Tree");

	//register callbacks
	glutReshapeFunc(reshape);//register reshape callback function
	glutDisplayFunc(display);//register display callback function
	glutKeyboardUpFunc(handleKeypressUp);//register keyboard press callback function

	//initializing the light sources and enabling the hidden surface removal
	init();

	cout << "Press Space Bar to begin the Game!!" << endl;
	// enter GLUT event processing cycle
	//glutPassiveMotionFunc(update);		//to update mouse pointer positions
	glutMainLoop();//enter the event loop

	return 0;//unreachable return statement :P
}