/*
 * 2DPrimitive.cpp
 *
 *  Created on: Nov 5, 2020
 *      Author: 1659515_snhu (Spencer Runde)
 */

#include <GL/freeglut.h> // Include the freeGLUT header file

/*Implements Display Callback Handler*/
void displayGraphics() {
	/*Specify the red, green, blue, and alpha values used when the buffers are cleared. Color is set to Black with full Opacity*/
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT); // Uses the color buffer and sets the background color

	glBegin(GL_POLYGON);  // Starting delimiter for Pentagon primitive
		glColor3f(1.0f, 1.0f, 0.0f); // Sets vertex color to Yellow

		/*Sets 5 vertices to form a Pentagon*/
		glVertex2f(-1.0f, 0.8f);
		glVertex2f(-0.7f, 1.0f);
		glVertex2f(-0.4f, 0.8f);
		glVertex2f(-0.5f, 0.5f);
		glVertex2f(-0.9f, 0.5f);
	glEnd(); // Ending delimiter for Pentagon primitive

	glBegin(GL_POLYGON);  // Start delimiter for Heptagon primitive
		glColor3f(0.0f, 1.0f,  1.0f); // Sets vertex color to Cyan

		/*Sets 7 vertices to form a Heptagon*/
		glVertex2f(0.5f, 0.9f);
		glVertex2f(0.7f, 1.0f);
		glVertex2f(0.9f, 0.9f);
		glVertex2f(1.0f, 0.7f);
		glVertex2f(0.85f, 0.5f);
		glVertex2f(0.55f, 0.5f);
		glVertex2f(0.4f, 0.7f);
	glEnd(); // Ending delimiter for Heptagon primitive

	glBegin(GL_POLYGON);  // Start delimiter for Star primitive
		glColor3f(1.0f, 0.0f, 0.0f); // Sets vertex color to Red

		/*Sets 10 vertices to form a Star*/
		glVertex2f(-0.1f, -0.3f);
		glVertex2f(-0.3f, -0.2f);
		glVertex2f(-0.1f, -0.2f);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(0.1f, -0.2f);
		glVertex2f(0.3f, -0.2f);
		glVertex2f(0.1f, -0.3f);
		glVertex2f(0.2f, -0.5f);
		glVertex2f(0.0f, -0.4f);
		glVertex2f(-0.2f, -0.5f);
	glEnd(); // Ending delimiter for Star primitive

	glFlush(); // Empties all buffers and executes all commands to be accepted by rendering engine
}

/*Main function for Immediate Mode */
int main (int argc, char** argv){
	glutInit(&argc, argv);  // Initializes the freeglut library
	glutCreateWindow("2D Shapes"); // Create a window and title
	glutInitWindowSize(1280, 720); // Specifies window's width and height
	glutInitWindowPosition(0,0); // Specifies the position of the window's top left corner
	glutDisplayFunc(displayGraphics); // Sets the display callback for the current window
	glutMainLoop();  // Enters the GLUT event processing loop
	return 0; // Exits main function
}
