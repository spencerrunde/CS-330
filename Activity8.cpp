/*
 * Activity8.cpp
 *
 *  Created on: Dec 3, 2020
 *      Author: 1659515_snhu (Spencer Runde)
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

#define WINDOW_TITLE "Practice Activity 8" // Window title Macro

// Shader program macro
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

// Var declaration for shader, window size, initialization, buffer and array objects
GLint cubeShaderProgram, lampShaderProgram, WindowWidth = 800, WindowHeight = 600;
GLuint VBO, CubeVAO, LightVAO;

// Subject postion and scale
glm::vec3 cubePosition(0.0f, 0.0f, 0.0f);
glm::vec3 cubeScale(2.0f);

// Cube and light color
glm::vec3 objectColor(0.6f, 0.5f, 0.75f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

// Light position and scale
glm::vec3 lightPosition(0.5f, 0.5f, -3.0f);
glm::vec3 lightScale(0.3f);

// Camera postion
glm::vec3 cameraPosition(0.0f, 0.0f, -6.0f);

// Camera rotation
float cameraRotation = glm::radians(-25.0f);



// Function prototypes
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateShader(void);
void UCreateBuffers(void);

// Cube Vertex shader source code
const GLchar* cubeVertexShaderSource = GLSL(330,

		layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
		layout(location = 1) in vec3 normal; // VAP position 1 for normals

		out vec3 Normal; // For outgoing normals to fragment shader
		out vec3 FragmentPos; // For outgoing color / pixels to fragment shader

		// Uniform / Global variables for the transform matrices
		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

void main() {
	
		gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates
		
		FragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection) 
		
		Normal = mat3(transpose(inverse(model))) * normal; // Get normal vectors in world space only and exclude normal translation properties
		
	}
);

// Cube Fragment shader source  code
const GLchar* cubeFragmentShaderSource = GLSL(330,

		in vec3 Normal; // For incoming normals
		in vec3 FragmentPos; // For incoming fragment position

		out vec4 cubeColor; // For outgoing cube color to the GPU

		// Uniform / Global variables for object color, light color, light position, and camera/view position
		uniform vec3 objectColor;
		uniform vec3 lightColor;
		uniform vec3 lightPos;
		uniform vec3 viewPosition;

	void main() {

		// Phong lighting model calculation to generate ambient, diffuse, and specular components
		
		// Calculate Ambient lighting
		float ambientStrength = 0.1f; // Set ambient or global lighting stength
		vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

		// Calculate Diffuse lighting
		vec3 norm = normalize(Normal); // Normalize vectors to 1 unit
		vec3 lightDirection = normalize(lightPos - FragmentPos); // Calculate distance
		float impact = max(dot(norm, lightDirection), 0.0); // Calculate diffuse impact by generating dot product of normal and light
		vec3 diffuse = impact * lightColor; // Generate diffuse light color

		// Calculate Specular lighting
		float specularIntensity = 0.8f; // Set specular light strength
		float highlightSize = 16.0f; // Set specular highlight size
		vec3 viewDir = normalize(viewPosition - FragmentPos); // Calculate view direction
		vec3 reflectDir = reflect(-lightDirection, norm); // Calculate reflection vector

		// Calculate Specular component
		float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
		vec3 specular = specularIntensity * specularComponent * lightColor;

		// Calculate phong result
		vec3 phong = (ambient + diffuse + specular) * objectColor;
		cubeColor = vec4(phong, 1.0f); // Send lighting results to GPU
	}
);

// Lamp shader source code
const GLchar * lampVertexShaderSource = GLSL(330,

		layout (location = 0) in vec3 position; //VAP position 0 for vertex position data

		// Uniform / Global variables for the transform matrices
		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

		void main()
		{
			gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms verticies into clip coordinates
		}
);

// Fragment shader source code
const GLchar * lampFragmentShaderSource = GLSL(330,

		out vec4 color; // For outgoing lamp color (smaller cube) to the GPU

		void main()
		{
			color = vec4(1.0f); // Set color to white (1.0f, 1.0f, 1.0f)
		}
);

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

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color

	glutDisplayFunc(URenderGraphics);

	glutMainLoop();

	// Destroys buffer objects once used
	glDeleteVertexArrays(1, &CubeVAO);
	glDeleteVertexArrays(1, &LightVAO);
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

	GLint modelLoc, viewLoc, projLoc, objectColorLoc, lightColorLoc, lightPositionLoc, viewPositionLoc;

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	// Use the Cube Shader and activate the Cube Vertex Array Object for rendering and transforming
	glUseProgram(cubeShaderProgram);
	glBindVertexArray(CubeVAO);

	// Transform the cube
	model = glm::translate(model, cubePosition);
	model = glm::scale(model, cubeScale);

	// Transform the camera
	view = glm::translate(view, cameraPosition);
	view = glm::rotate(view, cameraRotation, glm::vec3(0.0, 1.0f, 0.0f));

	// Set camera projection to perspective
	projection = glm::perspective(45.0f, (GLfloat)WindowWidth / (GLfloat)WindowHeight, 0.1f, 100.0f);

	// Reference matrix uniforms from the Cube Shader Program
	modelLoc = glGetUniformLocation(cubeShaderProgram, "model");
	viewLoc = glGetUniformLocation(cubeShaderProgram, "view");
	projLoc = glGetUniformLocation(cubeShaderProgram, "projection");

	// Pass matrix data to the Cube Shader Program's matrix uniforms
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// Feference matrix uniforms from the Cube Shader Program for the cube color, light color, light position, and camera position
	objectColorLoc = glGetUniformLocation(cubeShaderProgram, "objectColor");
	lightColorLoc =  glGetUniformLocation(cubeShaderProgram, "lightColor");
	lightPositionLoc = glGetUniformLocation(cubeShaderProgram, "lightPosition");
	viewPositionLoc = glGetUniformLocation(cubeShaderProgram, "viewPosition");

	// Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
	glUniform3f(objectColorLoc, objectColor.r, objectColor.g, objectColor.b);
	glUniform3f(lightColorLoc, lightColor.r, lightColor.g, lightColor.b);
	glUniform3f(lightPositionLoc, lightPosition.x, lightPosition.y, lightPosition.z);
	glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	glDrawArrays(GL_TRIANGLES, 0, 36); // Draw the primitives / cube

	glBindVertexArray(0); // Deactivate the Cube Vertex Array Object

	// Use the Lamp Shader and activate the Lamp Vertex Array Object for rendering and transforming
	glUseProgram(lampShaderProgram);
	glBindVertexArray(LightVAO);

	// Transform the smaller cube used as a visual que for the light source
	model = glm::translate(model, lightPosition);
	model = glm::scale(model, lightScale);

	// Reference matrix uniforms from the Lamp Shader program
	modelLoc = glGetUniformLocation(lampShaderProgram, "model");
	viewLoc = glGetUniformLocation(lampShaderProgram, "view");
	projLoc = glGetUniformLocation(lampShaderProgram, "projection");

	// Pass matrix data to the Lamp shader program's matrix uniforms
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glDrawArrays(GL_TRIANGLES, 0, 36); // Draw the primitives / small cube (lamp)

	glBindVertexArray(0); // Deactivates the Lamp Vertex Array Object

	glutPostRedisplay(); // Marks the current window to be redisplayed
	glutSwapBuffers(); // Flips the back buffer with the front buffer every frame. Similar to GL Flush

}

// Creates the Shader program
void UCreateShader()
{
	// Cube Vertex shader
	GLint cubeVertexShader = glCreateShader(GL_VERTEX_SHADER); // Creates the Vertex shader
	glShaderSource(cubeVertexShader, 1, &cubeVertexShaderSource, NULL); // Attaches the Vertex shader to the source code
	glCompileShader(cubeVertexShader); // Compiles the Vertex shader

	// Cube Fragment shader
	GLint cubeFragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // Creates the Fragment shader
	glShaderSource(cubeFragmentShader, 1, &cubeFragmentShaderSource, NULL); // Attaches the Fragment shader to the source code
	glCompileShader(cubeFragmentShader); // Compiles the Fragment shader

	// Cube Shader program
	cubeShaderProgram = glCreateProgram(); // Creates the Shader program and returns an id
	glAttachShader(cubeShaderProgram, cubeVertexShader); // Attach Vertex shader to the Shader program
	glAttachShader(cubeShaderProgram, cubeFragmentShader);; // Attach Fragment shader the Shader program
	glLinkProgram(cubeShaderProgram); // Link Vertex and Fragment shaders to Shader program

	// Delete the cube shaders once linked
	glDeleteShader(cubeVertexShader);
	glDeleteShader(cubeFragmentShader);

	// Lamp Vertex shader
	GLint lampVertexShader = glCreateShader(GL_VERTEX_SHADER); // Creates the Vertex shader
	glShaderSource(lampVertexShader, 1, &lampVertexShaderSource, NULL); // Attaches the Vertex shader to the source code
	glCompileShader(lampVertexShader); // Compiles the Vertex shader

	// Lamp Fragment shader
	GLint lampFragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // Creates the Fragment shader
	glShaderSource(lampFragmentShader, 1, &lampFragmentShaderSource, NULL); // Attaches the Fragment shader to the source code
	glCompileShader(lampFragmentShader); // Compiles the Fragment shader

	// Lamp Shader program
	cubeShaderProgram = glCreateProgram(); // Creates the Shader program and returns an id
	glAttachShader(lampShaderProgram, lampVertexShader); // Attach Vertex shader to the Shader program
	glAttachShader(lampShaderProgram, lampFragmentShader);; // Attach Fragment shader the Shader program
	glLinkProgram(lampShaderProgram); // Link Vertex and Fragment shaders to Shader program

	// Delete the lamp shaders once linked
	glDeleteShader(lampVertexShader);
	glDeleteShader(lampFragmentShader);
}

// Creates the Buffer and Array objects
void UCreateBuffers()
{
	// Position and Texture coordinate data for 36 triangles
	GLfloat verticies[] = {
							// Positions		     // Normals

							//Back Face		         //Negative Z normals
							-0.5f, -0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,
							 0.5f, -0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,
							 0.5f,  0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,
							 0.5f,  0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,
							-0.5f,  0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,
							-0.5f, -0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,

							//Front Face			 //Positive Z normals
							-0.5f, -0.5f,  0.5f,	 0.0f,  0.0f,  1.0f,
							 0.5f, -0.5f,  0.5f,	 0.0f,  0.0f,  1.0f,
							 0.5f,  0.5f,  0.5f,	 0.0f,  0.0f,  1.0f,
							 0.5f,  0.5f,  0.5f,	 0.0f,  0.0f,  1.0f,
							-0.5f,  0.5f,  0.5f,	 0.0f,  0.0f,  1.0f,
							-0.5f, -0.5f,  0.5f,	 0.0f,  0.0f,  1.0f,

							//Left Face				//Negative X normals
							-0.5f,  0.5f,  0.5f,	-1.0f,  0.0f,  0.0f,
							-0.5f,  0.5f, -0.5f,	-1.0f,  0.0f,  0.0f,
							-0.5f, -0.5f, -0.5f,	-1.0f,  0.0f,  0.0f,
							-0.5f, -0.5f, -0.5f,	-1.0f,  0.0f,  0.0f,
							-0.5f, -0.5f,  0.5f,	-1.0f,  0.0f,  0.0f,
							-0.5f,  0.5f,  0.5f,	-1.0f,  0.0f,  0.0f,

							//Right Face 			//Positive X normals
							 0.5f,  0.5f,  0.5f,	 1.0f,  0.0f,  0.0f,
							 0.5f,  0.5f, -0.5f,	 1.0f,  0.0f,  0.0f,
							 0.5f, -0.5f, -0.5f,	 1.0f,  0.0f,  0.0f,
							 0.5f, -0.5f, -0.5f,	 1.0f,  0.0f,  0.0f,
							 0.5f, -0.5f,  0.5f,	 1.0f,  0.0f,  0.0f,
							 0.5f,  0.5f,  0.5f,	 1.0f,  0.0f,  0.0f,

							 //Bottom Face			//Negative Y normals
							-0.5f, -0.5f, -0.5f,	 0.0f, -1.0f,  0.0f,
							 0.5f, -0.5f, -0.5f,	 0.0f, -1.0f,  0.0f,
							 0.5f, -0.5f,  0.5f,	 0.0f, -1.0f,  0.0f,
							 0.5f, -0.5f,  0.5f,	 0.0f, -1.0f,  0.0f,
							-0.5f, -0.5f,  0.5f,	 0.0f, -1.0f,  0.0f,
							-0.5f, -0.5f, -0.5f,	 0.0f, -1.0f,  0.0f,

							//Top Face				//Positive Y normals
							-0.5f,  0.5f, -0.5f,	 0.0f,  1.0f,  0.0f,
							 0.5f,  0.5f, -0.5f,	 0.0f,  1.0f,  0.0f,
							 0.5f,  0.5f,  0.5f,	 0.0f,  1.0f,  0.0f,
							 0.5f,  0.5f,  0.5f,	 0.0f,  1.0f,  0.0f,
							-0.5f,  0.5f,  0.5f,	 0.0f,  1.0f,  0.0f,
							-0.5f,  0.5f, -0.5f,	 0.0f,  1.0f,  0.0f,

				  };


			// Generate buffer ids for cube
			glGenVertexArrays(1, &CubeVAO); // Vertex Array Object for Cube Vertices
			glGenBuffers(1, &VBO);

			// Activate the Vertex Array Object before binding and setting any VBOs or Vertex Attribute Pointers
			glBindVertexArray(CubeVAO);

			// Activate the VBO
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW); // Copy vertices to VBO

			// Set attribute pointer 0 to hold Position data
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0); // Enables vertex attribute

			// Set attribute pointer 1 to hold Normal data
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);

			glBindVertexArray(0); // Deactivates the VAO which is good practice

			// Generate buffer ids for lamp (smaller cube)
			glGenVertexArrays(1, &LightVAO); // Vertex Array Object for Cube vertex copies to serve as light source

			// Activate the Vertex Array Object before binding and setting any VBOs or Vertex Attribute Pointers
			glBindVertexArray(LightVAO);

			// Referencing the same VBO for its vertices
			glBindBuffer(GL_ARRAY_BUFFER, VBO);

			// Set attribute pointer 0 to hold position data (used for the lamp)
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			
			glBindVertexArray(0); // Deactivates the VAO which is good practice
}
