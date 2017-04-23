#include "GL\glew.h"
#include "GL\freeglut.h"
#include <iostream>
#include <cmath>
#include <queue>
#include <random>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

//MACROS
#define epsilon 0.001

GLint windowWidth = 1280;                    // Width of our window
GLint windowHeight = 720;                    // Height of our window

//Tree Parameters:-
//float h1=45.0f, h2=0.0f;	//branching angles (for monopodial, h2=0)
//float R1=0.7f, R2=0.9f;		//contraction ratios
//float divergence = 140.0f;				//divergence angle
//float R=0.15f, L=6.0f;		//Radius and length of the trunk
//int level = 9;				//number of growth levels

//rendering parameters:-
float angle = 0.0f;
float yPos = 0.0f;

//Color declarations:-
GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat green[] = { 0.1f, 0.9f, 0.1f, 1.0 };
GLfloat brown[] = { 0.4f, 0.2f, 0.0f, 1.0 };
GLfloat red[] = { 1.0f,0.0f,0.0f,1.0 };
GLfloat blue[] = { 0.0,0.0,1.0,1.0 };

struct animation_parameters {
	float windx;
	float windy; 
	float windz;
};

//random number generator
default_random_engine generator;
normal_distribution<double> distribution(0.0, 0.03);

//Multiple Frogs
int numTrees = 0;

struct tree {
	int START_FRAME;
	int END_FRAME;
	float FPS;
	int TOTAL_FRAMES;

	vector<animation_parameters> keyFrames;

	float windx;
	float windy;
	float windz;

	float xPos, zPos;
	float h1, h2;	    //branching angles (for monopodial, h2=0)
	float hmin1, hmax1, hmin2, hmax2;
	float R1, R2;		//contraction ratios
	float divergence;	//divergence angle
	float R, L;		    //Radius and length of the trunk
	int level;			//number of growth levels

	int category;
	float K1, K2;//constant in characteristic function

	//global variables
	int startTime;
	int elapsedTime;
	int prevTime;
	int currentFrame;
	float t_interpolation;

};

vector<tree> trees;

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

//struct to store co-ordinates of an attractore or inhibitor:-
struct Point {
	Point(float xPos, float yPos, float zPos, float f)
		:x(xPos), y(yPos), z(zPos), factor(f) {
	}

	~Point() {}

	float x;
	float y;
	float z;
	float factor;
};


//Environment parameters:-
float windx_mean = 0.0f, windy_mean = 0.0f, windz_mean = 0.0f;	//wind direction
vector<Point> attractors;	//list of attractors
vector<Point> inhibitors;	//list of inhibitors
int ai = 0, ii = 0;

//function declarations
void drawLeaf(float x, float y, float z);
void animate();
float lerp(float x, float y, float t);
void readKeyFrames(tree& tree);

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
		windx_mean = windx_mean + 0.05f;
		glutPostRedisplay();
		break;
	case 'X':
		windx_mean = windx_mean - 0.05f;
		glutPostRedisplay();
		break;
	case 'y':
		windy_mean = windy_mean + 0.05f;
		glutPostRedisplay();
		break;
	case 'Y':
		windy_mean = windy_mean - 0.05f;
		glutPostRedisplay();
		break;
	case 'z':
		windz_mean = windz_mean + 0.05f;
		glutPostRedisplay();
		break;
	case 'Z':
		windz_mean = windz_mean - 0.05f;
		glutPostRedisplay();
		break;
	/*case 'd':
		divergence = divergence + 20;
		cout << divergence << endl;
		glutPostRedisplay();
		break;
	case 'D':
		divergence = divergence - 20;
		cout << divergence << endl;
		glutPostRedisplay();
		break;*/
	/*case '+':
		level = level + 1;
		glutPostRedisplay();
		break;
	case '-':
		level--;
		glutPostRedisplay();
		break;*/
	case 'n':
		cout << "Enter co-ordinates of attractor" << endl;
		float x1, y1, z1, f1;
		cin >> x1 >> y1 >> z1 >> f1;
		attractors.push_back(Point(x1, y1, z1, f1));
		glutPostRedisplay();
		break;
	case 'N':
		cout << "Enter co-ordinates of inhibitor" << endl;
		cin >> x1 >> y1 >> z1 >> f1;
		inhibitors.push_back(Point(x1, y1, z1, f1));
		glutPostRedisplay();
		break;
	case 'c':
		cout << "Enter index of attractor" << endl;
		cin >> ai;
		break;
	case 'C':
		cout << "Enter index of inhibitor" << endl;
		cin >> ii;
		break;
	case '1':
		if (ai >= 0 && ai < attractors.size()) {
			attractors[ai].x += 0.5f;
			glutPostRedisplay();
		}
		else
			cout << "This index does not exist" << endl;
		break;
	case '2':
		if (ai >= 0 && ai < attractors.size()) {
			attractors[ai].y += 0.5f;
			glutPostRedisplay();
		}
		else
			cout << "This index does not exist" << endl;
		break;
	case '3':
		if (ai >= 0 && ai < attractors.size()) {
			attractors[ai].z += 0.5f;
			glutPostRedisplay();
		}
		else
			cout << "This index does not exist" << endl;
		break;
	case 'l':
	{
		trees.push_back(tree());
		numTrees++;
		tree& tree = trees[numTrees - 1];
		//initialize tree
		tree.START_FRAME = 0;
		tree.END_FRAME = 0;//Updated in readFrames
		tree.FPS = 100.0f;//Speed of wind
		tree.TOTAL_FRAMES = 10.0f;

		//Make this read from a file
		ifstream infile;
		string str = "tree";
		str += to_string(numTrees);
		str += ".txt";
		infile.open(str.c_str());

		std::string line;
		
		getline(infile, line);
		istringstream iss(line);

		iss >> tree.xPos
			>> tree.zPos
			>> tree.h1
			>> tree.h2
			>> tree.category
			>> tree.hmin1
			>> tree.hmax1
			>> tree.hmin2
			>> tree.hmax2
			>> tree.R1
			>> tree.R2
			>> tree.divergence
			>> tree.R
			>> tree.L
			>> tree.level;

		infile.close();

		tree.K1 = -(tree.hmax1 - tree.hmin1) / (tree.level);
		tree.K2 = -(tree.hmax2 - tree.hmin2) / (tree.level);
		
		
		//initialization end

		tree.currentFrame = tree.START_FRAME;
		readKeyFrames(tree);
		tree.startTime = glutGet(GLUT_ELAPSED_TIME);
	}
	break;
	case 32://Space bar start animation
		glutIdleFunc(animate);//register idle callback
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
	glutSolidCylinder(radius, r, 5, 5);
}

//For uniform and non-uniform deformations of a branch
Point deform(float x, float y, float z, int ind) {
	float incx = 0.0f, incy = 0.0f, incz = 0.0f;
	//attractors:-
	for (int i = 0; i < attractors.size(); i++) {
		float dx = attractors[i].x - x, dy = attractors[i].y - y, dz = attractors[i].z - z;
		float factor = attractors[i].factor / sqrt(dx*dx + dy*dy + dz*dz);
		incx += factor*dx;
		incy += factor*dy;
		incz += factor*dz;
	}
	//inhibitors:-
	for (int i = 0; i < inhibitors.size(); i++) {
		float dx = inhibitors[i].x - x, dy = inhibitors[i].y - y, dz = inhibitors[i].z - z;
		float factor = inhibitors[i].factor / sqrt(dx*dx + dy*dy + dz*dz);
		incx -= factor*dx;
		incy -= factor*dy;
		incz -= factor*dz;
	}
	return Point(x + incx + windx_mean + trees[ind].windx, y + incy + windy_mean + trees[ind].windy, z + incz + trees[ind].windz + windz_mean, 0.0);
}

//to push children of a mother branch into the queue, after applying appropriate transformations
void pushChild(queue<Branch> &branches, Branch mother, float angle, float cont,int level,int tree) {
	float divergence = trees[tree].divergence;				//divergence angle
	float u = mother.xEnd - mother.xStart, v = mother.yEnd - mother.yStart, w = mother.zEnd - mother.zStart;
	float length = sqrt(u*u + v*v + w*w);
	float phi = atan2(u, w) * 180 / 3.14f;
	float theta = asin(v / length) * 180 / 3.14f;
	float mv[16];
	glPushMatrix();
		glLoadIdentity();
		glRotatef((GLfloat)phi, 0.0, 1.0, 0.0);
		glRotatef((GLfloat)(-theta), 1.0, 0.0, 0.0);
		glRotatef((GLfloat)divergence*level, 0.0, 0.0, 1.0);
		glRotatef((GLfloat)(angle), 0.0f, 1.0f, 0.0f);
		glGetFloatv(GL_MODELVIEW_MATRIX, mv);
	glPopMatrix();
	float xp = mv[8] * cont*length + mv[12];
	float yp = mv[9] * cont*length + mv[13];
	float zp = mv[10] * cont*length + mv[14];
	float wp = mv[11] * cont*length + mv[15];
	xp /= wp;
	yp /= wp;
	zp /= wp;
	Point p = deform(xp, yp, zp,tree);
	branches.push(Branch(cont*mother.radius, mother.xEnd, mother.yEnd, mother.zEnd, mother.xEnd + p.x , mother.yEnd + p.y, mother.zEnd + p.z ));
}

//for drawing GMT1 model of tree using the given parameters
void drawGMT1(int tree) {
	float h1 = trees[tree].h1 , h2 = trees[tree].h2;	//branching angles (for monopodial, h2=0)
	float R1 = trees[tree].R1, R2 = trees[tree].R2;		//contraction ratios
	float divergence = trees[tree].divergence;				//divergence angle
	float R = trees[tree].R, L = trees[tree].L;		//Radius and length of the trunk
	int level = trees[tree].level;				//number of growth levels
	queue<Branch> branches;
	glPushMatrix();
		drawBranch(R, 0.0f, L, 0.0f);	//main trunk
		//seeding the first two child branches:-
		Point p = deform(R2*L*sin(h2*3.14 / 180.0), L + R2*L*cos(h2*3.14 / 180.0), 0.0, tree);
		branches.push(Branch(R2*R, 0.0f, L, 0.0f, p.x, p.y, p.z));
		p = deform(R1*L*sin(h1*3.14 / 180.0), L + R1*L*cos(h1*3.14 / 180.0), 0.0, tree);
		branches.push(Branch(R1*R, 0.0f, L, 0.0f, p.x, p.y, p.z));
	glPopMatrix();
	float H1 = h1;
	for (int i = 1; i < level; i++) {
		//Code for GMT4 
		if (trees[tree].category == 1) {
			h1 += trees[tree].K1;
			h2 += trees[tree].K2;
		}
		else if (trees[tree].category == 2) {
			if (i <= (level / 2)) {
				h1 += trees[tree].K1*((float)i/(float)(level/2));
				h2 += trees[tree].K2*((float)i / (float)(level / 2));
			}
			else {
				h1 = trees[tree].hmin1*2 - (trees[tree].hmin1/exp(level/2)) * exp(i - (level/2));
				h2 = trees[tree].hmin2 * 2 - (trees[tree].hmin2 / exp(level / 2)) * exp(i - (level / 2));
			}
		}
		else if (trees[tree].category == 3) {
			if (i > (level / 2)) {
				h1 += trees[tree].K1*((float)i / (float)(level));
				h2 += trees[tree].K2*((float)i / (float)(level));
			}
			else {
				h1 = 10.0f + trees[tree].hmax1 - ( (trees[tree].hmax1 / (2.0f * exp(level / 2))) * exp(i));
				h2 = trees[tree].hmax2 - (trees[tree].hmax2 / 2 * exp(level / 2)) * exp(i);
			}
		}
		else if (trees[tree].category == 4) {
			if (i <= (level / 2)) {
				h1 += trees[tree].K1*((float)i / (float)(level / 2));
				h2 += trees[tree].K2*((float)i / (float)(level / 2));
			}
			else if(i <= 0.75*level){
				h1 = trees[tree].hmin1 * 2 - (trees[tree].hmin1 / exp(level*0.75)) * exp(i - (level / 2));
				h2 = trees[tree].hmin2 * 2 - (trees[tree].hmin2 / exp(level*0.75)) * exp(i - (level / 2));
			}
			else{
				h1 += trees[tree].K1*((float)i / (float)(level));
				h2 += trees[tree].K2*((float)i / (float)(level));
			}
		}
		else if (trees[tree].category == 5) {
			if (i <= (level / 2)) {
				h1 -= trees[tree].K1*((float)i / (float)(level / 2));
				h2 -= trees[tree].K2*((float)i / (float)(level / 2));
			}
			else if (i == (level / 2 + 1)) {
				h1 = trees[tree].hmax1;
				h2 = trees[tree].hmax2;
			}
			else {
				h1 = trees[tree].hmin1 * 2 - (trees[tree].hmin1 / exp(level / 2)) * exp(i - (level / 2));
				h2 = trees[tree].hmin2 * 2 - (trees[tree].hmin2 / exp(level / 2)) * exp(i - (level / 2));
			}
		}

		if ((H1 > 0 && h1 > 0) || (H1 < 0 && h1 < 0))
			H1 = h1;
		else
			H1 = -h1;
		//
		queue<Branch> temp;
		if (h2 == 0) {
			//MONOPODIAL CASE FOR GENERATING CHILD BRANCHES OF AXIAL BRANCH:-
			H1 = -H1;	//Alternate the sign of the branching angles
			Branch mother = branches.front();	
			branches.pop();
			float u = mother.xEnd - mother.xStart, v = mother.yEnd - mother.yStart, w = mother.zEnd - mother.zStart;
			glPushMatrix();
				glTranslatef(mother.xStart, mother.yStart, mother.zStart);
				drawBranch(mother.radius, u, v, w);
			glPopMatrix();
			p = deform(mother.xEnd, mother.yEnd + R2*v, mother.zEnd,tree);
			temp.push(Branch(R2*mother.radius, mother.xEnd, mother.yEnd, mother.zEnd, p.x, p.y, p.z));
			if (divergence == 0) {
				p = deform(R1*v*sin(H1*3.14 / 180.0) + mother.xEnd, mother.yEnd + R1*v*cos(H1*3.14 / 180.0), mother.zEnd,tree);
				temp.push(Branch(R1*mother.radius, mother.xEnd, mother.yEnd, mother.zEnd, p.x, p.y, p.z));
			}
			else {
				p = deform(R1*v*sin(h1*3.14 / 180.0)*cos(divergence*i*3.14 / 180.0) + mother.xEnd, mother.yEnd + R1*v*cos(h1*3.14 / 180.0), -R1*v*sin(H1*i*3.14 / 180.0)*sin(divergence*3.14 / 180.0) + mother.zEnd, tree);
				temp.push(Branch(R1*mother.radius, mother.xEnd, mother.yEnd, mother.zEnd, p.x, p.y, p.z));
			}
		}
		while (!branches.empty()){
			Branch mother = branches.front();
			branches.pop();
			float u = mother.xEnd - mother.xStart, v = mother.yEnd - mother.yStart, w = mother.zEnd - mother.zStart;
			glPushMatrix();
				glTranslatef(mother.xStart, mother.yStart, mother.zStart);
				drawBranch(mother.radius,u, v, w);
			glPopMatrix();
			pushChild(temp, mother, h2, R2, 0, tree);
			pushChild(temp, mother, H1, R1, 0, tree);
		}
		while (!temp.empty()){
			branches.push(temp.front());
			temp.pop();
		}
	}

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
	while (!branches.empty()) {
		Branch mother = branches.front();
		branches.pop();
		float u = mother.xEnd - mother.xStart, v = mother.yEnd - mother.yStart, w = mother.zEnd - mother.zStart;
		glPushMatrix();
			glTranslatef(mother.xStart, mother.yStart, mother.zStart);
			//drawBranch(mother.radius, u, v, w);
			drawLeaf(u,v,w);
		glPopMatrix();
	}
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
	for (int i = 0; i < attractors.size(); i++) {
		glPushMatrix();
			glTranslatef(attractors[i].x, attractors[i].y, attractors[i].z);
			glutSolidSphere(1.0f, 10, 10);
		glPopMatrix();
	}
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
	for (int i = 0; i < inhibitors.size(); i++) {
		glPushMatrix();
			glTranslatef(inhibitors[i].x, inhibitors[i].y, inhibitors[i].z);
			glutSolidSphere(1.0f, 10, 10);
		glPopMatrix();
	}
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, brown);
	//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	
	for (int i = 0; i < trees.size(); i++) {
		glPushMatrix();
			glRotatef((GLfloat)angle, 0.0f, 1.0f, 0.0f);
			glTranslatef(trees[i].xPos, yPos, trees[i].zPos);
			glScalef(0.15f, 0.15f, 0.15f);
			drawGMT1(i);
		glPopMatrix();
	}
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

	//if (w <= h) {
	//	// width is smaller, so stretch out the height
	//	gluPerspective(45.0 / 2.0f, h / w, 1.0, 1500.0f);
	//}
	//else {
	//	// height is smaller, so stretch out the width
	//	gluPerspective(45.0 / 2.0f, w / h, 1.0, 1500.0f);
	//}

}


void init() {
	GLfloat black[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat direction[] = { 1.0, 1.0, 1.0, 0.0 };

	//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, brown);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	//glMaterialf(GL_FRONT, GL_SHININESS, 30);

	glLightfv(GL_LIGHT0, GL_AMBIENT, black);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, white);
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

void drawLeaf(float x, float y, float z) {
	float r = sqrt(x*x + y*y + z*z);
	float phi = atan2(x, z) * 180 / 3.14f;
	float theta = asin(y / r) * 180 / 3.14f;
	glRotatef((GLfloat)phi, 0.0, 1.0, 0.0);
	glRotatef((GLfloat)(-theta), 1.0, 0.0, 0.0);
	glRotatef((GLfloat)(90),1.0,0.0,0.0);
	glScalef(1/4.0, 1/4.0, 1/4.0);

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

void animate() {
	for (int indexTree = 0; indexTree < trees.size(); indexTree++) {
		struct tree& tree = trees[indexTree];

		tree.elapsedTime = glutGet(GLUT_ELAPSED_TIME);
		//Sleep(max(0, 17 - (frog.elapsedTime - frog.prevTime)));
		tree.prevTime = tree.elapsedTime;
		tree.elapsedTime = glutGet(GLUT_ELAPSED_TIME);
		
		
		int currentFrame = tree.currentFrame;
		int next_frame = currentFrame + 1;
		//tree.FPS = abs((20.0f/0.2f)*(1.0f/3.0f)*(tree.keyFrames[next_frame].windx - tree.keyFrames[currentFrame].windx + tree.keyFrames[next_frame].windy - tree.keyFrames[currentFrame].windy + tree.keyFrames[next_frame].windz - tree.keyFrames[currentFrame].windz));
		tree.t_interpolation = ((tree.elapsedTime - tree.startTime)*(tree.FPS)) / (1000.0f * tree.TOTAL_FRAMES);
		float t_interpolation = tree.t_interpolation;

		vector<animation_parameters>& keyFrames = tree.keyFrames;
		//Interpolate all the parameters of the model
		tree.windx= lerp(keyFrames[currentFrame].windx, keyFrames[next_frame].windx, t_interpolation);
		tree.windy = lerp(keyFrames[currentFrame].windy, keyFrames[next_frame].windy, t_interpolation);
		tree.windz = lerp(keyFrames[currentFrame].windz, keyFrames[next_frame].windz, t_interpolation);

		if (t_interpolation > 1.0f - epsilon) {
			tree.currentFrame++;
			if (tree.currentFrame == tree.END_FRAME - 1) {
				tree.currentFrame = tree.START_FRAME;
				readKeyFrames(tree);
				tree.startTime = glutGet(GLUT_ELAPSED_TIME);
			}
			tree.startTime = glutGet(GLUT_ELAPSED_TIME);
		}
	}
	glutPostRedisplay();
}

void readKeyFrames(tree& tree) {
	vector<animation_parameters>& keyFrames = tree.keyFrames;
	
	keyFrames.clear();
	
	tree.END_FRAME = 100;

	/*keyFrames.push_back(animation_parameters());
	keyFrames[0].windx = 0;
	keyFrames[0].windy = 0;
	keyFrames[0].windz = 0;*/

	ifstream infile;
	infile.open("keyFrames.txt");

	std::string line;

	for (int i = 0; i < tree.END_FRAME; i++)
	{
		keyFrames.push_back(animation_parameters());
		getline(infile, line);
		istringstream iss(line);

		iss >> keyFrames[i].windx
			>> keyFrames[i].windy
			>> keyFrames[i].windz;

	}

	infile.close();

	/*ofstream outfile;
	outfile.open("keyFrames.txt");

	for (int i = 0; i < tree.END_FRAME; i++)
		outfile << keyFrames[i].windx << " " << keyFrames[i].windy << " " << keyFrames[i].windz << endl;

	outfile.close();*/
}

float lerp(float x, float y, float t) {
	if (t > 1 - epsilon)
		t = 1.0f;
	return x + t*(y - x);
}