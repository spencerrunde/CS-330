/*
 * Modern2DShapes.cpp
 *
 *  Created on: Nov 12, 2020
 *      Author: 1659515_snhu (Spencer Runde)
 */

#include <iostream> // Includes C++ i/o stream
#include <GL/glew.h> // Includes glew header
#include <GL/freeglut.h> // Includes freeglut header

using namespace std; // Uses the standard namespace

#define WINDOW_TITLE "Assignment 3-1" // Macro for window title

// Vertex and Fragment Shader Source Macro
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

// Variables for window width and height
int WindowWidth = 800, WindowHeight = 600;

/* User-defined Function prototypes to:
 * initialize the program, set window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
void UInitialize(int, char*[]);
void UInitWindow(int, char*[]);
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateVBO(void);
void UCreateShaders(void);

/*Vertex Shader Program Source Code*/
const GLchar* VertexShader =
{
  "#version 400\n" // Declare version number

  "layout(location=0) in vec4 in_Position;\n" // Receive vertex coordinates from attribute 0. i.e. 2 floats per vertex.

   /*Get the vertex colors from the Vertex Buffer Object*/
  "layout(location=1) in vec4 in_Color;\n" // for attribute 1 expect vec(4) floats passed into the Vertex shader.
  "out vec4 ex_Color;\n" // declare a vec 4 variable that will reference the vertex colors passed into the Vertex shader from the buffer.

  "void main(void)\n"
  "{\n"
  "  gl_Position = in_Position;\n" // Sends vertex positions to gl_position vec 4
  "  ex_Color = in_Color;\n" // References vertex colors sent from the buffer
  "}\n"
};


/*Fragment Shader Program Source Code*/
const GLchar* FragmentShader =
{
  "#version 400\n" // Declare version number

  "in vec4 ex_Color;\n" // Vertex colors from the vertex shader
  "out vec4 out_Color;\n" // vec4 variable that will reference the vertex colors passed into the fragment shader the vertex shader

  "void main(void)\n"
  "{\n"
  "  out_Color = ex_Color;" // Send the vertex colors to the GPU
  "}\n"
};

// Main function. Entry point to the OpenGl Program
int main (int argc, char* argv[])
{
   UInitialize(argc, argv); // Initialize the OpenGL program
   glutMainLoop(); // Starts the OpenGL loop in background
   exit(EXIT_SUCCESS); // Terminates the program successfully
}

// Implements the UInitialize function
void UInitialize (int argc, char* argv[])
{
	// glew status variable
	GLenum GlewInitResult;

	UInitWindow (argc, argv); // Creates the window

	// Checks glew status
	GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult)
	{
		fprintf(stderr, "ERROR: %s\n", glewGetErrorString(GlewInitResult));
		exit(EXIT_FAILURE);
	}

	// Displays GPU OpenGL version
	fprintf(stdout, "INFO: OpenGL version: %s\n", glGetString(GL_VERSION));

	UCreateVBO(); // Calls the function to create the Vertex Buffer Object

	UCreateShaders(); // Calls the function to create the Shader Program

	// Sets the background color of the window to black. Optional
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

// Implements the UInitWindow Function
void UInitWindow(int argc, char* argv[])
{
	//Initializes freeglut
	glutInit(&argc, argv);

	// Sets the window size
   glutInitWindowSize(WindowWidth, WindowHeight);

   // Memory Buffer setup for display
   glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

   // Creates a window with the macro placeholder title
   glutCreateWindow(WINDOW_TITLE);


   glutReshapeFunc(UResizeWindow); // Called when the windows is resized
   glutDisplayFunc(URenderGraphics); // Renders graphics on the screen
}

// Implements the UResizeWindow function
void UResizeWindow(int Width, int Height)
{
	glViewport(0, 0, Width, Height);
}

// Implements the URenderGraphics Function
void URenderGraphics(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clears the screen

	/* Creates the triangle*/
	GLuint totalVerticies = 6;
	//glDrawArrays(GL_TRIANGLES, 0, totalVerticies); // Draws the triangle

	// Draw triangle using indicies
	glDrawElements(GL_TRIANGLES, totalVerticies, GL_UNSIGNED_SHORT, NULL);

	glutSwapBuffers(); // Flips the back buffer with front buffer every frame. Similar to GL Flush
}

//Implements the CreateVBO Function
void UCreateVBO(void)
{
	//Specifies coordinates for triangle vertices on x and y
	GLfloat verts[] =
	{
			-1.0f, 1.0f, // top-left of the screen
			1.0f, 0.0f, 0.0f, 1.0f, // Red vertex

			-1.0f, 0.0f, // center-left of the screen
			0.0f, 0.0f, 1.0f, 1.0f, // Blue vertex

			-0.5f, 0.0f, // shared vertex
			0.0f, 1.0f, 0.0f, 1.0f, // Green vertex

			0.0f, 0.0f, // center of the screen
			1.0f, 0.0f, 0.0f, 1.0f, // Red vertex

			0.0f, -1.0f, // bottom-center of the screen
			0.0f, 1.0f, 0.0f, 1.0f, // Green vertex
	};

	// Stores the size of the verts array / number of coordinates needed for the triangle i.e, 6
	float numVerticies = sizeof(verts);

	GLuint myBufferID; // Variable for vertex buffer object id
	glGenBuffers(1, &myBufferID); // Creates 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID); // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, numVerticies, verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to GPU

	/*Creates the Vertex Attribute Pointer*/
	GLuint floatsPerVertex = 2; // Number of coordinates per vertex
	glEnableVertexAttribArray(0);// Specifies the inital position of the coordinates in the buffer

	/*Strides between vertex coordinates is 6 (x, y, r, g, b, a). A tightly packed stride is 0.*/
	GLint vertexStride = sizeof(float) * 6;

	/*Instructs the GPU on how to handle the vertex buffer object data.
	Parameters: atrribPointerPosition | coordinates per vertex | data type | deavtivate normaliztions | 0 strides | 0 offset
	 */
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, vertexStride, 0);

	/* Sets an attribute pointer position for the vertex colors i.e. Attribute 1 for rgba floats. Attribute - was for position x, y*/
	glEnableVertexAttribArray(1); // Specifices position 1 for the color values in the buffer

	GLint colorStride = sizeof(float) * 6; // The number of floats before each color is 6 i.e. rgba xy

	// Paramaters: attribPointerPosition 1 | floats per color is 4 i.e rgba | data type | deactivate normalization | 6 strides until the next color i.
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, colorStride, (char*)(sizeof(float) * 2));

	/* Creates a buffer object for indices*/
	GLushort indicies[] = {0, 1, 2, 2, 3, 4};
	float numIndicies = sizeof(indicies);
	GLuint indexBufferID;
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndicies, indicies, GL_STATIC_DRAW);
}

// Implements the UCreateShaders function
void UCreateShaders(void)
{
	// Create a Shader program object
	GLuint ProgramId = glCreateProgram();

	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER); // Create a vertex shader object
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER); // Create a fragment shader object

	glShaderSource(vertexShaderId, 1, &VertexShader, NULL); // Retrieves the vertex shader source code
	glShaderSource(fragmentShaderId, 1, &FragmentShader, NULL); // Retrieves the fragment shader source code

	glCompileShader(vertexShaderId); // Compile the vertex shader
	glCompileShader(fragmentShaderId); // Compile the fragment shader

	// Attaches the vertex and fragment shaders to the shader program
	glAttachShader(ProgramId, vertexShaderId);
	glAttachShader(ProgramId, fragmentShaderId);

	glLinkProgram(ProgramId); // Links the shader program
	glUseProgram(ProgramId); // Uses the shader program
}
