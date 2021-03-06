/*
 * 3DPrimitive.cpp
 *
 *  Created on: Nov 5, 2020
 *      Author: 1659515_snhu (Spencer Runde)
 */

#include <GL/glut.h> // Include the freeGLUT header file

/*Depth and tessellation settings*/
void applyDepthSettings() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Set background to black and opaque
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

/*Implements Display Callback Handler*/
void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clears color and depth buffers
	glMatrixMode(GL_MODELVIEW); // Activates the Model-View matrix

	glLoadIdentity(); //Reset the model-view matrix
	glTranslatef(-0.0f, 0.0f, -8.0f); // Move model to the center of the world with a z depth of -7
	glRotatef(45.0f, 0.4f, -1.0f, -0.1f); // Rotate the model on x, y, and z

	/*Creates the Cube*/
	glBegin(GL_QUADS);

		//Set Front face color (Red) and vertices
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.5f, 1.0f, 1.0f);
		glVertex3f(-0.5f, 1.0f, 1.0f);
		glVertex3f(-1.0f, -1.0f ,1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);

		//Set Right face color (Green) and vertices
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.5f, 1.0f, -1.0f);
		glVertex3f(0.5f, 1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);

		//Set Top face color (Blue) and vertices
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.5f, 1.0f, 1.0f);
		glVertex3f(0.5f, 1.0f, -1.0f);
		glVertex3f(-0.5f, 1.0f, -1.0f);
		glVertex3f(-0.5f, 1.0f, 1.0f);

		//Set Left face color (Yellow) and vertices
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
		glVertex3f(-0.5f, 1.0f, 1.0f);
		glVertex3f(-0.5f, 1.0f, -1.0f);

		//Set Back face color (Dull Green) and vertices
		glColor3f(0.0f, 0.5f, 0.4f);
		glVertex3f(0.5f, 1.0f, -1.0f);
		glVertex3f(-0.5f, 1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);

		//Set Bottom face color (Grey) and vertices
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(1.0f, -1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);

	glEnd(); //Ending delimiter for Cube primitive

	glutSwapBuffers(); //Swap front and back frame buffers (similar to glFlush)

}

void reshape(GLsizei width, GLsizei height){

	if(height == 0) height = 1;
		GLfloat aspect = (GLfloat)width / (GLfloat)height;

	glViewport(0, 0, width, height);

	//Set the aspect ratio of clipping volume to match the viewport
	glMatrixMode(GL_PROJECTION); // Create a projection matrix
	glLoadIdentity(); // Reset projection matrix
	gluPerspective(45.0f, aspect, 0.1f, 100.0f); // Set perspective projection, fov, aspect, zNear and zFar clipping

}

/*Main function required for Immediate Mode*/
int main(int argc, char** argv){
	glutInit(&argc, argv);  // Initializes the freeglut library
	glutInitDisplayMode(GLUT_DOUBLE); // Enable double buffered mode
	glutInitWindowSize(640, 480); // Specifies the window's height and width
	glutInitWindowPosition(0, 0); // Specifies the position of the window's top left corner
	glutCreateWindow("3D Cube"); // Create a window and title
	glutDisplayFunc(display); // Sets display callback for screen re-paint event
	glutReshapeFunc(reshape); // Sets display callback for window re-size event
	applyDepthSettings(); //Apply depth settings
	glutMainLoop();  // Enter the infinite event-processing loop
	return 0;
}
