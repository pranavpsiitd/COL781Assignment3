#include "GL\glew.h"
#include "GL\freeglut.h"
#include <iostream>
#include <cmath>
#include <queue>

using namespace std;

GLint windowWidth = 1280;                    // Width of our window
GLint windowHeight = 720;                    // Height of our window

//Tree Parameters:-
float h1=(float)(25.0*(3.14/180.0)), h2=(float)(0.0f*(3.14/180.0));	//branching angles (for monopodial, h2=0)
float R1=0.7f, R2=0.9f;		//contraction ratios
float divergence = 0.0;				//divergence angle
float R=0.15f, L=6.0f;		//Radius and length of the trunk
int level = 9;				//number of growth levels
float windx = 0.0f, windy = 0.0f, windz = 0.0f;	//wind direction


//cursor position:-
int xpos, ypos;

//rendering parameters:-
float angle = 0.0f;
float yPos = 0.0f;

//struct to store co-ordinates of a branch
struct Branch {
	Branch(float r, float a, float b, float c, float d, float e, float f)
		: radius(r), xStart(a), yStart(b), zStart(c), xEnd(d), yEnd(e), zEnd(f) {
	}

	~Branch() {}

	float radius;
	float xStart;
	float yStart;
	float zStart;
	float xEnd;
	float yEnd;
	float zEnd;
};

void drawLeaf(void);

//update the cursor positions:-
void update(int x, int y) {
	xpos = x;
	ypos = y;
}

// Function to set flags according to which keys are pressed or released
void handleKeypressUp(unsigned char theKey, int x, int y){
	switch (theKey)
	{
	case 'a':
		angle = angle + 5.0f;
		glutPostRedisplay();
		break;
	case 'A':
		angle = angle - 5.0f;
		glutPostRedisplay();
		break;
	case 'm':
		yPos = yPos - 0.2f;
		glutPostRedisplay();
		break;
	case 'M':
		yPos = yPos + 0.2f;
		glutPostRedisplay();
		break;
	case 'x':
		windx = windx + 0.05f;
		glutPostRedisplay();
		break;
	case 'X':
		windx = windx - 0.05f;
		glutPostRedisplay();
		break;
	case 'y':
		windy = windy + 0.05f;
		glutPostRedisplay();
		break;
	case 'Y':
		windy = windy - 0.05f;
		glutPostRedisplay();
		break;
	case 'z':
		windz = windz + 0.05f;
		glutPostRedisplay();
		break;
	case 'Z':
		windz = windz - 0.05f;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

//for drawing a cylindrical branch from (0,0,0) to (x,y,z)
void drawBranch(float radius,float x, float y, float z) {
	float r = sqrt(x*x + y*y + z*z);
	float phi = atan2(x , z)*180/3.14f;
	float theta = asin(y / r)*180/3.14f;
	glRotatef((GLfloat)phi, 0.0, 1.0, 0.0);
	glRotatef((GLfloat)(-theta), 1.0, 0.0, 0.0);
	glutSolidCylinder(radius, r, 20, 20);
}

//for drawing GMT1 model of tree using the given parameters
void drawGMT1() {
	queue<Branch> branches;
	glPushMatrix();
		drawBranch(R, 0.0f, L, 0.0f);	//main trunk
		//seeding the first two child branches:-
		branches.push(Branch(R2*R, 0.0f, L, 0.0f, R2*L*sin(h2) + windx, L + R2*L*cos(h2) + windy, windz));
		branches.push(Branch(R1*R, 0.0f, L, 0.0f, R1*L*sin(h1) + windx, L + R1*L*cos(h1) + windy, windz));
	glPopMatrix();
	float H1 = h1;
	for (int i = 1; i < level; i++) {
		queue<Branch> temp;
		if (h2 == 0) {
			//MONOPODIAL CASE FOR GENERATING CHILD BRANCHES OF AXIAL BRANCH:-
			H1 = -H1;	//Alternate the sign of the banching angles
			Branch mother = branches.front();	
			branches.pop();
			float u = mother.xEnd - mother.xStart, v = mother.yEnd - mother.yStart, w = mother.zEnd - mother.zStart;
			glPushMatrix();
				glTranslatef(mother.xStart, mother.yStart, mother.zStart);
				drawBranch(mother.radius, u, v, w);
			glPopMatrix();
			temp.push(Branch(R2*mother.radius, mother.xEnd, mother.yEnd, mother.zEnd, mother.xEnd + windx, mother.yEnd + windy + R2*v, mother.zEnd + windz));
			temp.push(Branch(R1*mother.radius, mother.xEnd, mother.yEnd, mother.zEnd, R1*v*sin(H1) + windx + mother.xEnd, mother.yEnd + R1*v*cos(H1) + windy, mother.zEnd + windz));
		}
		while (!branches.empty()){
			Branch mother = branches.front();
			branches.pop();
			float u = mother.xEnd - mother.xStart, v = mother.yEnd - mother.yStart, w = mother.zEnd - mother.zStart;
			glPushMatrix();
				glTranslatef(mother.xStart, mother.yStart, mother.zStart);
				drawBranch(mother.radius,u, v, w);
			glPopMatrix();
			//child branches are in a plane perpendicular to the plane of mother branch and down vector
			float S = sqrt(u*u + w*w), T = sqrt(u*u + w*w + v*v);
			float dx = R1*(u*cos(H1) - (T / S)*w*sin(H1)) + windx;
			float dy = R1*v*cos(H1) + windy;
			float dz = R1*(w*cos(H1) + (T / S)*u*sin(H1)) + windz;
			temp.push(Branch(R1*mother.radius, mother.xEnd, mother.yEnd, mother.zEnd, mother.xEnd + dx, mother.yEnd + dy, mother.zEnd + dz));
			dx = R2*(u*cos(h2) - (T / S)*w*sin(h2)) + windx;
			dy = R2*v*cos(h2) + windy;
			dz = R2*(w*cos(h2) + (T / S)*u*sin(h2)) + windz;
			temp.push(Branch(R1*mother.radius, mother.xEnd, mother.yEnd, mother.zEnd, mother.xEnd + dx, mother.yEnd + dy, mother.zEnd + dz));
			
		}
		while (!temp.empty()){
			branches.push(temp.front());
			temp.pop();
		}
	}
	while (!branches.empty()) {
		Branch mother = branches.front();
		branches.pop();
		float u = mother.xEnd - mother.xStart, v = mother.yEnd - mother.yStart, w = mother.zEnd - mother.zStart;
		glPushMatrix();
			glTranslatef(mother.xStart, mother.yStart, mother.zStart);
			drawBranch(mother.radius, u, v, w);
		glPopMatrix();
	}
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	
	glPushMatrix();
		glRotatef((GLfloat)angle, 0.0f, 1.0f, 0.0f);
		glTranslatef(0.0f, yPos, 0.0f);
		glScalef(0.15f, 0.15f, 0.15f);
		drawGMT1();
		drawLeaf();
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

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
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

	cout << "Press A for changing angle" << endl;
	cout << "Press M for moving the model" << endl;
	cout << "Press X, Y and Z for changing the wind velocity vector" << endl;
	// enter GLUT event processing cycle
	//glutPassiveMotionFunc(update);		//to update mouse pointer positions
	glutMainLoop();//enter the event loop

	return 0;//unreachable return statement :P
}

void drawLeaf(void) {
	glColor3f(0.1, 0.9, 0.1);
	glBegin(GL_POLYGON);
	glVertex2f(0.0, 0.0);
	glVertex2f(1.0, 0.7);
	glVertex2f(1.3, 1.8);
	glVertex2f(1.0, 2.8);
	glVertex2f(0.0, 4.0);
	glVertex2f(-1.0, 2.8);
	glVertex2f(-1.3, 1.8);
	glVertex2f(-1.0, 0.7);
	glEnd();
}