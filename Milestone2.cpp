/*
 *  Milestone2.cpp
 *
 *  Created on: Dec 5, 2020
 *  Author: 1659515_snhu (Spencer Runde)
 */

// Header Inclusions
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

// GLM Math Header inclusions
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace std; // Standard namespace

#define WINDOW_TITLE "Milestone 2" // Window title Macro

// Shader program Macro
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

// Variable declarations for shader, window size initialization, buffer and array objects
GLint shaderProgram, WindowWidth = 800, WindowHeight = 600;
GLuint VBO, VAO;

GLfloat cameraSpeed = 0.0005f; // Movement speed per frame

GLfloat lastMouseX = 400, lastMouseY = 300; // Locks mouse cursor at the center of screen
GLfloat mouseXOffset, mouseYOffset, yaw = 0.0f, pitch = 0.0f; // mouse offset, yaw, and pitch variables
GLfloat sensitivity = 0.005f; // Used for mouse / camera rotation sensitivity
bool mouseDetected = true; // Initially true when mouse movement is detected

// Global vector declarations
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f); // Initial camera position. Placed 5 units in Z
glm::vec3 cameraUpY = glm::vec3(0.0f, 1.0f, 0.0f); // Temporary Y unit vector
glm::vec3 cameraForwardZ = glm::vec3(0.0f, 0.0f, -1.0f); // Temporary Z unit vector
glm::vec3 front; // Temporary Z unit vector for mouse

// Function prototypes
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateShader(void);
void UCreateBuffers(void);

void UMouseMove(int x, int y);

// Vertex Shader Source Code
const GLchar* vertexShaderSource = GLSL(330,
	layout(location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
	layout(location = 1) in vec3 color; // Color data from Vertex Attrib Pointer 1

	out vec3 mobileColor; // Variable to transfer color data to the fragment shader

	// Global variables for transform matrices
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

void main() {
		gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices to clip coordinates
		mobileColor = color; // References incoming color data
	}
);

// Fragment Shader Source Code
const GLchar* fragmentShaderSource = GLSL(330,

		in vec3 mobileColor; // Variable to hold incoming color data from vertex shader

		out vec4 gpuColor; // Variable to pass color data to the GPU

	void main() {

		gpuColor = vec4(mobileColor, 1.0); // Sends color data to the GPU for rendering

	}
);

// Main Program
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutCreateWindow(WINDOW_TITLE);

	glutReshapeFunc(UResizeWindow);

	glewExperimental = GL_TRUE;
			if (glewInit() != GLEW_OK)
			{
				std::cout << "Failed to initialize GLEW" << std::endl;
				return -1;
			}

	UCreateShader();

	UCreateBuffers();

	// Use the shader program
	glUseProgram(shaderProgram);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color

	glutDisplayFunc(URenderGraphics);

	glutPassiveMotionFunc(UMouseMove); // Detects mouse movement

	glutMainLoop();

	// Destroys Buffer objects once used
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	return 0;
}

// Resizes the window
void UResizeWindow(int w, int h)
{
	WindowWidth = w;
	WindowHeight = h;
	glViewport(0, 0, WindowWidth, WindowHeight);
}

// Renders graphics
void URenderGraphics(void)
{
	glEnable(GL_DEPTH_TEST); // Enable z-depth

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clears the screen

	glBindVertexArray(VAO); // Activate the Vertex Array Object before rendering and transforming them

	cameraForwardZ = front; // Replaces camera forward vector with Radians normalized as a unit vector

	// Transforms the object
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0, 0.0f, 0.0f)); // Places the object at the center of the viewport
	model = glm::rotate(model, 45.0f, glm::vec3(0.0, 1.0f, 0.0f)); // Rotates 45 degrees on the X axis
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f)); // Increase the object size by a scale of 2

	// Transforms the camera
	glm::mat4 view;
	view = glm::lookAt(cameraForwardZ, cameraPosition, cameraUpY);

	// Creates a perspective projection
	glm::mat4 projection;
	projection = glm::perspective(45.0f, (GLfloat)WindowWidth / (GLfloat)WindowHeight, 0.1f, 100.0f);

	// Retrieves and passes transform matrices to the Shader program
	GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glutPostRedisplay();

	// Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, 228);

	glBindVertexArray(0); // Deactivate the Vertex Array Object

	glutSwapBuffers(); // Flips the back buffer with the front buffer every frame. Similar to GL Flush

}

// Creates the Shader program
void UCreateShader()
{
	// Vertex shader
	GLint vertexShader = glCreateShader(GL_VERTEX_SHADER); // Creates the Vertex shader
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // Attaches the Vertex shader to the source code
	glCompileShader(vertexShader); // Compiles the Vertex shader

	// Fragment shader
	GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // Creates the Fragment shader
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL); // Attaches the Fragment shader to the source code
	glCompileShader(fragmentShader); // Compiles the Fragment shader

	// Shader program
	shaderProgram = glCreateProgram(); // Creates the Shader program and returns an id
	glAttachShader(shaderProgram, vertexShader); // Attach Vertex shader to the Shader program
	glAttachShader(shaderProgram, fragmentShader); // Attach Fragment shader to the Shader program
	glLinkProgram(shaderProgram); // Link Vertex and Fragment shaders to Shader program

	// Delete the vertex and fragment shaders once linked
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

void UCreateBuffers()
{

	GLfloat vertices[] = {
							// Position				// Colors
							// Base of Table
							// Bottom Surface
						   -0.25f, 0.0f, -0.25f,		1.0f, 0.0f, 0.0f,
						   -0.25f, 0.0f,  -0.25f,		1.0f, 0.0f, 0.0f,
							1.25f, 0.0f, -0.25f,		1.0f, 0.0f, 0.0f,
							1.25f, 0.0f,  2.25f,		1.0f, 0.0f, 0.0f,
						   -0.25f, 0.0f,  2.25f,		1.0f, 0.0f, 0.0f,
							1.25f, 0.0f, -0.25f,		1.0f, 0.0f, 0.0f,

							// Top Surface
							-0.25f, 0.25f, -0.25f,		1.0f, 0.0f, 0.0f,
							-0.25f, 0.25f,  2.25f,		1.0f, 0.0f, 0.0f,
							1.25f,  0.25f, -0.25f,		1.0f, 0.0f, 0.0f,
							1.25f,  0.25f,  2.25f,		1.0f, 0.0f, 0.0f,
							-0.25f, 0.25f,  2.25f,		1.0f, 0.0f, 0.0f,
							1.25f,  0.25f, -0.25f,		1.0f, 0.0f, 0.0f,

							// Left Side
							-0.25f,  0.0f, -0.25f,		1.0f, 0.0f, 0.0f,
							-0.25f,  0.0f, 2.25f,		1.0f, 0.0f, 0.0f,
							-0.25f, 0.25f, -0.25f,		1.0f, 0.0f, 0.0f,
							-0.25f, 0.25f, -0.25f,		1.0f, 0.0f, 0.0f,
							-0.25f, 0.25f, 2.25f,		1.0f, 0.0f, 0.0f,
							-0.25f,  0.0f, 2.25f,		1.0f, 0.0f, 0.0f,

							// Right Side
							1.25f,  0.0f, -0.25f,		1.0f, 0.0f, 0.0f,
							1.25f,  0.0f, 2.25f,		1.0f, 0.0f, 0.0f,
							1.25f, 0.25f, -0.25f,		1.0f, 0.0f, 0.0f,
							1.25f, 0.25f, -0.25f,		1.0f, 0.0f, 0.0f,
							1.25f, 0.25f, 2.25f,		1.0f, 0.0f, 0.0f,
							1.25f,  0.0f, 2.25f,		1.0f, 0.0f, 0.0f,

							// Forward Side
						   -0.25f,  0.0f, -0.25f,		1.0f, 0.0f, 0.0f,
							1.25f,  0.0f, -0.25f,		1.0f, 0.0f, 0.0f,
						   -0.25f, 0.25f, -0.25f,		1.0f, 0.0f, 0.0f,
						   -0.25f, 0.25f, -0.25f,		1.0f, 0.0f, 0.0f,
							1.25f, 0.25f, -0.25f,		1.0f, 0.0f, 0.0f,
							1.25f,  0.0f, -0.25f,		1.0f, 0.0f, 0.0f,
							// Back side
						   -0.25f,  0.0f, 2.25f,		1.0f, 0.0f, 0.0f,
							1.25f,  0.0f, 2.25f,		1.0f, 0.0f, 0.0f,
						   -0.25f, 0.25f, 2.25f,		1.0f, 0.0f, 0.0f,
						   -0.25f, 0.25f, 2.25f,		1.0f, 0.0f, 0.0f,
							1.25f, 0.25f, 2.25f,		1.0f, 0.0f, 0.0f,
							1.25f,  0.0f, 2.25f,		1.0f, 0.0f, 0.0f,

							// Legs
							// Front
							0.0f,  0.25f, 0.0f,		1.0f, 0.0f, 0.0f,
							0.25f, 0.25f, 0.0f,		1.0f, 0.0f, 0.0f,
							0.0f,  -1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
							0.0f,  -1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
							0.25f, 0.25f, 0.0f,		1.0f, 0.0f, 0.0f,
							0.25f, -1.0f, 0.0f,     1.0f, 0.0f, 0.0f,
							// Back
							0.0f,  0.25f, 0.25f,	1.0f, 0.0f, 0.0f,
							0.25f, 0.25f, 0.25f,	1.0f, 0.0f, 0.0f,
							0.0f,  -1.0f, 0.25f,	1.0f, 0.0f, 0.0f,
							0.0f,  -1.0f, 0.25f,	1.0f, 0.0f, 0.0f,
							0.25f, 0.25f, 0.25f,	1.0f, 0.0f, 0.0f,
							0.25f, -1.0f, 0.25f,    1.0f, 0.0f, 0.0f,
							// Bottom
							0.0f,  -1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
							0.25f, -1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
							0.0f,  -1.0f, 0.25f,	1.0f, 0.0f, 0.0f,
							0.25f, -1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
							0.0f,  -1.0f, 0.25f,	1.0f, 0.0f, 0.0f,
							0.25f, -1.0f, 0.25f,	1.0f, 0.0f, 0.0f,
							// Left
							0.0f,  0.25f, 0.0f,		1.0f, 0.0f, 0.0f,
							0.0f,  0.25f, 0.25f,	1.0f, 0.0f, 0.0f,
							0.0f,  -1.0f, 0.25f,	1.0f, 0.0f, 0.0f,
							0.0f,  -1.0f, 0.25f,	1.0f, 0.0f, 0.0f,
							0.0f,  -1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
							0.0f,  0.25f, 0.0f,		1.0f, 0.0f, 0.0f,
							// Right
							0.25f,  0.25f, 0.0f,	1.0f, 0.0f, 0.0f,
							0.25f,  0.25f, 0.25f,	1.0f, 0.0f, 0.0f,
							0.25f,  -1.0f, 0.25f,	1.0f, 0.0f, 0.0f,
							0.25f,  -1.0f, 0.25f,	1.0f, 0.0f, 0.0f,
							0.25f,  -1.0f, 0.0f,	1.0f, 0.0f, 0.0f,
							0.25f,  0.25f, 0.0f,	1.0f, 0.0f, 0.0f,

							// Front
							0.0f,  0.25f, 1.75f,	1.0f, 0.0f, 0.0f,
							0.25f, 0.25f, 1.75f,	1.0f, 0.0f, 0.0f,
							0.0f,  -1.0f, 1.75f,	1.0f, 0.0f, 0.0f,
							0.0f,  -1.0f, 1.75f,	1.0f, 0.0f, 0.0f,
							0.25f, 0.25f, 1.75f,	1.0f, 0.0f, 0.0f,
							0.25f, -1.0f, 1.75f,    1.0f, 0.0f, 0.0f,
							// Back
							0.0f,  0.25f, 2.0f,	    1.0f, 0.0f, 0.0f,
							0.25f, 0.25f, 2.0f,		1.0f, 0.0f, 0.0f,
							0.0f,  -1.0f, 2.0f,		1.0f, 0.0f, 0.0f,
							0.0f,  -1.0f, 2.0f,		1.0f, 0.0f, 0.0f,
							0.25f, 0.25f, 2.0f,		1.0f, 0.0f, 0.0f,
							0.25f, -1.0f, 2.0f,     1.0f, 0.0f, 0.0f,
							// Bottom
							0.0f,  -1.0f, 1.75f,	1.0f, 0.0f, 0.0f,
							0.25f, -1.0f, 1.75f,	1.0f, 0.0f, 0.0f,
							0.0f,  -1.0f, 2.0f,		1.0f, 0.0f, 0.0f,
							0.25f, -1.0f, 1.75f,	1.0f, 0.0f, 0.0f,
							0.0f,  -1.0f, 2.0f,		1.0f, 0.0f, 0.0f,
							0.25f, -1.0f, 2.0f,		1.0f, 0.0f, 0.0f,
							// Left
							0.0f,  0.25f, 1.75f,	1.0f, 0.0f, 0.0f,
							0.0f,  0.25f, 2.0f,		1.0f, 0.0f, 0.0f,
							0.0f,  -1.0f, 2.0f,		1.0f, 0.0f, 0.0f,
							0.0f,  -1.0f, 2.0f,		1.0f, 0.0f, 0.0f,
							0.0f,  -1.0f, 1.75f,	1.0f, 0.0f, 0.0f,
							0.0f,  0.25f, 1.75f,	1.0f, 0.0f, 0.0f,
							// Right
							0.25f,  0.25f, 1.75f,	1.0f, 0.0f, 0.0f,
							0.25f,  0.25f, 2.0f,	1.0f, 0.0f, 0.0f,
							0.25f,  -1.0f, 2.0f,	1.0f, 0.0f, 0.0f,
							0.25f,  -1.0f, 2.0f,	1.0f, 0.0f, 0.0f,
							0.25f,  -1.0f, 1.75f,	1.0f, 0.0f, 0.0f,
							0.25f,  0.25f, 1.75f,	1.0f, 0.0f, 0.0f,

							// Front
							0.75f, 0.25f, 1.75f,	1.0f, 0.0f, 0.0f,
							1.0f,  0.25f, 1.75f,	1.0f, 0.0f, 0.0f,
							0.75f, -1.0f, 1.75f,	1.0f, 0.0f, 0.0f,
							0.75f, -1.0f, 1.75f,	1.0f, 0.0f, 0.0f,
							1.0f,  0.25f, 1.75f,	1.0f, 0.0f, 0.0f,
							1.0f,  -1.0f, 1.75f,    1.0f, 0.0f, 0.0f,
							// Back
							0.75f, 0.25f, 2.0f,		1.0f, 0.0f, 0.0f,
							1.0f,  0.25f, 2.0f,		1.0f, 0.0f, 0.0f,
							0.75f, -1.0f, 2.0f,		1.0f, 0.0f, 0.0f,
							0.75f, -1.0f, 2.0f,		1.0f, 0.0f, 0.0f,
							1.0f,  0.25f, 2.0f,		1.0f, 0.0f, 0.0f,
							1.0f,  -1.0f, 2.0f,     1.0f, 0.0f, 0.0f,
							// Bottom
							0.75f, -1.0f, 1.75f,	1.0f, 0.0f, 0.0f,
							1.0f,  -1.0f, 1.75f,	1.0f, 0.0f, 0.0f,
							0.75f, -1.0f, 2.0f,		1.0f, 0.0f, 0.0f,
							1.0f,  -1.0f, 1.75f,	1.0f, 0.0f, 0.0f,
							0.75f, -1.0f, 2.0f,		1.0f, 0.0f, 0.0f,
							1.0f,  -1.0f, 2.0f,		1.0f, 0.0f, 0.0f,
							// Left
							0.75f,  0.25f, 1.75f,	1.0f, 0.0f, 0.0f,
							0.75f,  0.25f, 2.0f,	1.0f, 0.0f, 0.0f,
							0.75f,  -1.0f, 2.0f,	1.0f, 0.0f, 0.0f,
							0.75f,  -1.0f, 2.0f,	1.0f, 0.0f, 0.0f,
							0.75f,  -1.0f, 1.75f,	1.0f, 0.0f, 0.0f,
							0.75f,  0.25f, 1.75f,	1.0f, 0.0f, 0.0f,
							// Right
							1.0f,  0.25f, 1.75f,	1.0f, 0.0f, 0.0f,
							1.0f,  0.25f, 2.0f,		1.0f, 0.0f, 0.0f,
							1.0f,  -1.0f, 2.0f,		1.0f, 0.0f, 0.0f,
							1.0f,  -1.0f, 2.0f,		1.0f, 0.0f, 0.0f,
							1.0f,  -1.0f, 1.75f,	1.0f, 0.0f, 0.0f,
							1.0f,  0.25f, 1.75f,	1.0f, 0.0f, 0.0f,

							// Front
							0.75f, 0.25f, 0.0f,		1.0f, 0.0f, 0.0f,
							1.0f,  0.25f, 0.0f,		1.0f, 0.0f, 0.0f,
							0.75f, -1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
							0.75f, -1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
							1.0f,  0.25f, 0.0f,		1.0f, 0.0f, 0.0f,
							1.0f,  -1.0f, 0.0f,     1.0f, 0.0f, 0.0f,
							// Back
							0.75f, 0.25f, 0.25f,	1.0f, 0.0f, 0.0f,
							1.0f,  0.25f, 0.25f,	1.0f, 0.0f, 0.0f,
							0.75f, -1.0f, 0.25f,	1.0f, 0.0f, 0.0f,
							0.75f, -1.0f, 0.25f,	1.0f, 0.0f, 0.0f,
							1.0f,  0.25f, 0.25f,	1.0f, 0.0f, 0.0f,
							1.0f,  -1.0f, 0.25f,    1.0f, 0.0f, 0.0f,
							// Bottom
							0.75f, -1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
							1.0f,  -1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
							0.75f, -1.0f, 0.25f,	1.0f, 0.0f, 0.0f,
							1.0f,  -1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
							0.75f, -1.0f, 0.25f,	1.0f, 0.0f, 0.0f,
							1.0f,  -1.0f, 0.25f,	1.0f, 0.0f, 0.0f,
							// Left
							0.75f,  0.25f, 0.0f,	1.0f, 0.0f, 0.0f,
							0.75f,  0.25f, 0.25f,	1.0f, 0.0f, 0.0f,
							0.75f,  -1.0f, 0.25f,	1.0f, 0.0f, 0.0f,
							0.75f,  -1.0f, 0.25f,	1.0f, 0.0f, 0.0f,
							0.75f,  -1.0f, 0.0f,	1.0f, 0.0f, 0.0f,
							0.75f,  0.25f, 0.0f,	1.0f, 0.0f, 0.0f,
							// Right
							1.0f,  0.25f, 0.0f,		1.0f, 0.0f, 0.0f,
							1.0f,  0.25f, 0.25f,	1.0f, 0.0f, 0.0f,
							1.0f,  -1.0f, 0.25f,	1.0f, 0.0f, 0.0f,
							1.0f,  -1.0f, 0.25f,	1.0f, 0.0f, 0.0f,
							1.0f,  -1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
							1.0f,  0.25f, 0.0f,		1.0f, 0.0f, 0.0f,

							// Support Braces
							// Front brace
							0.25f,  0.0f,  0.25f,	1.0f, 1.0f, 0.0f,
							0.75f,  0.0f,  0.25f,	1.0f, 1.0f, 0.0f,
							0.25f, -0.25f, 0.25f,	1.0f, 1.0f, 0.0f,
							0.25f, -0.25f, 0.25f,	1.0f, 1.0f, 0.0f,
							0.75f,  0.0f,  0.25f,	1.0f, 1.0f, 0.0f,
							0.75f, -0.25f, 0.25f,	1.0f, 1.0f, 0.0f,

							0.25f,  0.0f,  0.0f,	1.0f, 1.0f, 0.0f,
							0.75f,  0.0f,  0.0f,	1.0f, 1.0f, 0.0f,
							0.25f, -0.25f, 0.0f,	1.0f, 1.0f, 0.0f,
							0.25f, -0.25f, 0.0f,	1.0f, 1.0f, 0.0f,
							0.75f,  0.0f,  0.0f,	1.0f, 1.0f, 0.0f,
							0.75f, -0.25f, 0.0f,	1.0f, 1.0f, 0.0f,

							0.25f, -0.25f, 0.25f,	1.0f, 1.0f, 0.0f,
							0.75f, -0.25f, 0.25f,	1.0f, 1.0f, 0.0f,
							0.25f, -0.25f, 0.0f,	1.0f, 1.0f, 0.0f,
							0.25f, -0.25f, 0.0f,	1.0f, 1.0f, 0.0f,
							0.75f, -0.25f, 0.25f,	1.0f, 1.0f, 0.0f,
							0.75f, -0.25f, 0.0f,	1.0f, 1.0f, 0.0f,

							// Back brace
							0.25f,  0.0f,  2.0f,	1.0f, 1.0f, 0.0f,
							0.75f,  0.0f,  2.0f,	1.0f, 1.0f, 0.0f,
							0.25f, -0.25f, 2.0f,	1.0f, 1.0f, 0.0f,
							0.25f, -0.25f, 2.0f,	1.0f, 1.0f, 0.0f,
							0.75f,  0.0f,  2.0f,	1.0f, 1.0f, 0.0f,
							0.75f, -0.25f, 2.0f,	1.0f, 1.0f, 0.0f,

							0.25f,  0.0f,  1.75f,	1.0f, 1.0f, 0.0f,
							0.75f,  0.0f,  1.75f,	1.0f, 1.0f, 0.0f,
							0.25f, -0.25f, 1.75f,	1.0f, 1.0f, 0.0f,
							0.25f, -0.25f, 1.75f,	1.0f, 1.0f, 0.0f,
							0.75f,  0.0f,  1.75f,	1.0f, 1.0f, 0.0f,
							0.75f, -0.25f, 1.75f,	1.0f, 1.0f, 0.0f,

							0.25f, -0.25f, 2.0f,	1.0f, 1.0f, 0.0f,
							0.75f, -0.25f, 2.0f,	1.0f, 1.0f, 0.0f,
							0.25f, -0.25f, 1.75f,	1.0f, 1.0f, 0.0f,
							0.25f, -0.25f, 1.75f,	1.0f, 1.0f, 0.0f,
							0.75f, -0.25f, 2.0f,	1.0f, 1.0f, 0.0f,
							0.75f, -0.25f, 1.75f,	1.0f, 1.0f, 0.0f,

							// Left brace
							0.0f,   0.0f, 0.25f,	1.0f, 1.0f, 0.0f,
							0.0f,   0.0f, 1.75f,	1.0f, 1.0f, 0.0f,
							0.0f, -0.25f, 1.75f,	1.0f, 1.0f, 0.0f,
							0.0f, -0.25f, 1.75f,	1.0f, 1.0f, 0.0f,
							0.0f,   0.0f, 0.25f,	1.0f, 1.0f, 0.0f,
							0.0f, -0.25f, 0.25f,	1.0f, 1.0f, 0.0f,

							0.25f,   0.0f, 0.25f,	1.0f, 1.0f, 0.0f,
							0.25f,   0.0f, 1.75f,	1.0f, 1.0f, 0.0f,
							0.25f, -0.25f, 1.75f,	1.0f, 1.0f, 0.0f,
							0.25f, -0.25f, 1.75f,	1.0f, 1.0f, 0.0f,
							0.25f,   0.0f, 0.25f,	1.0f, 1.0f, 0.0f,
							0.25f, -0.25f, 0.25f,	1.0f, 1.0f, 0.0f,

							0.0f,  -0.25f, 0.25f,	1.0f, 1.0f, 0.0f,
							0.25f, -0.25f, 0.25f,	1.0f, 1.0f, 0.0f,
							0.0f,  -0.25f, 1.75f,	1.0f, 1.0f, 0.0f,
							0.0f,  -0.25f, 1.75f,	1.0f, 1.0f, 0.0f,
							0.25f, -0.25f, 0.25f,	1.0f, 1.0f, 0.0f,
							0.25f, -0.25f, 1.75f,	1.0f, 1.0f, 0.0f,

							// Right brace
							1.0f,   0.0f, 0.25f,	1.0f, 1.0f, 0.0f,
							1.0f,   0.0f, 1.75f,	1.0f, 1.0f, 0.0f,
							1.0f, -0.25f, 1.75f,	1.0f, 1.0f, 0.0f,
							1.0f, -0.25f, 1.75f,	1.0f, 1.0f, 0.0f,
							1.0f,   0.0f, 0.25f,	1.0f, 1.0f, 0.0f,
							1.0f, -0.25f, 0.25f,	1.0f, 1.0f, 0.0f,

							0.75f,   0.0f, 0.25f,	1.0f, 1.0f, 0.0f,
							0.75f,   0.0f, 1.75f,	1.0f, 1.0f, 0.0f,
							0.75f, -0.25f, 1.75f,	1.0f, 1.0f, 0.0f,
							0.75f, -0.25f, 1.75f,	1.0f, 1.0f, 0.0f,
							0.75f,   0.0f, 0.25f,	1.0f, 1.0f, 0.0f,
							0.75f, -0.25f, 0.25f,	1.0f, 1.0f, 0.0f,

							0.75f, -0.25f, 0.25f,	1.0f, 1.0f, 0.0f,
							1.0f,  -0.25f, 0.25f,	1.0f, 1.0f, 0.0f,
							0.75f, -0.25f, 1.75f,	1.0f, 1.0f, 0.0f,
							0.75f, -0.25f, 1.75f,	1.0f, 1.0f, 0.0f,
							1.0f,  -0.25f, 0.25f,	1.0f, 1.0f, 0.0f,
							1.0f,  -0.25f, 1.75f,	1.0f, 1.0f, 0.0f,

						  };

	// Generate buffer ids
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Activate the Vertex Array Object before binding and setting any VBOs or Vertex Attribute Pointers
	glBindVertexArray(VAO);

	// Activate the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Copy vertices to VBO

	// Set attribute pointer 0 to hold Position data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0); // Enables vertex attribute

	// Set attribute pointer 1 to hold Color data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1); // Enables vertex attribute

	glBindVertexArray(0); // Deactivates the VAO which is good practice
}

// Implements the UMouseMove function
void UMouseMove(int x, int y)
{
	// Immediately replaces center locked coordinates with new mouse coordinates
	if(mouseDetected)
	{
		lastMouseX = x;
		lastMouseY = y;
		mouseDetected = false;
	}

	// Gets the direction the mouse was moved in x and y
	mouseXOffset = x - lastMouseX;
	mouseYOffset = lastMouseY - y; // Inverted Y

	// Updates with new mouse coordinates
	lastMouseX = x;
	lastMouseY = y;

	// Applies sensitivity to mouse direction
	mouseXOffset *= sensitivity;
	mouseYOffset *= sensitivity;

	// Accumulates the yaw and pitch variables
	yaw += mouseXOffset;
	pitch += mouseYOffset;

	// Orbits around the center
	front.x = 10.0f * cos(yaw);
	front.y = 10.0f * sin(pitch);
	front.z = sin(yaw) * cos(pitch) * 10.0f;

}
