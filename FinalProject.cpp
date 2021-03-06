/*
 *  FinalProject.cpp
 *
 *  Created on: Dec 10, 2020
 *  Author: 1659515_snhu (Spencer Runde)
 */

// Header Inclusions
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

// GLM Math Header Inclusions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// SOIL Image Loader Inclusion
#include "SOIL2/SOIL2.h"

using namespace std; // Standard namespace

#define WINDOW_TITLE "Final Project" // Window title Macro

// Shader Program Macro
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

// Variable declarations for shader, window size initialization, buffer and array objects
GLint tableShaderProgram, lampShaderProgram, WindowWidth = 800, WindowHeight = 600;
GLuint VBO, TableVAO, LightVAO, texture;

// Subject position and scale
glm::vec3 tablePosition(0.0f, 0.0f, 0.0f);
glm::vec3 tableScale(1.0f);

// Object and light color
glm::vec3 objectColor(1.0f, 1.0f, 1.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f); // Key light color
glm::vec3 fillLightColor(0.0f, 0.0f, 0.0f); // Fill light color

// Key Light position and scale
glm::vec3 lightPosition(-15.0f, 8.0f, 15.0f);
glm::vec3 lightScale(0.3f);

// Fill Light position and scale
glm::vec3 fillLightPosition(5.0f, 5.0f, -5.0f);
glm::vec3 fillLightScale(0.3f);

// Camera position
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, -6.0f); // Initial camera position

// Camera rotation
float cameraRotation = glm::radians(-25.0f);

GLfloat cameraSpeed = 0.0005f; // Movement speed per frame

GLchar currentKey; // Will store key pressed

GLfloat lastMouseX = 400, lastMouseY = 300; // Locks mouse cursor at center of screen
GLfloat mouseXOffset, mouseYOffset, yaw = 0.0f, pitch = 0.0f; // Mouse offset, yaw, and pitch variables
GLfloat sensitivity = 0.005f; // Used for mouse / camera rotation sensitivity
bool mouseDetected = true; // Initially true when mouse movement is detected
bool mouseLeft = true; // Initially true when mouse left click is detected
bool mouseRight = true; // Initially true when mouse right click is detected
bool altKey = true; // Initially true when left alt press is detected

// Global vector declarations
glm::vec3 CameraUpY = glm::vec3(0.0f, 1.0f, 0.0f); // Temporary Y unit vector
glm::vec3 CameraForwardZ = glm::vec3(0.0f, 0.0f, -1.0f); // Temporary Z unit vector

// Function prototypes
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateShader(void);
void UCreateBuffers(void);
void UGenerateTexture(void);
void UMouseClick(int button, int state, int x, int y);
void UMousePressedMove(int x, int y);
void UKeyboard(unsigned char key, int x, int y);

// Table Vertex Shader Source Code
const GLchar * tableVertexShaderSource = GLSL(330,
        layout (location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
        layout (location = 1) in vec3 normal; // VAP position 1 for normals
        layout (location = 2) in vec2 textureCoordinate; // VAP position 2 for textures

        out vec3 FragmentPos; // For outgoing color / pixels to fragment shader
        out vec3 Normal; // For outgoing normals to fragment shader
        out vec2 mobileTextureCoordinate;

        // Global variables for transform matrices
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

void main(){
        gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices to clip coordinates

        FragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

        Normal = mat3(transpose(inverse(model))) *  normal; // Get normal vectors in world space only and exclude normal translation properties

        mobileTextureCoordinate = vec2(textureCoordinate.x, 1 - textureCoordinate.y); // Flips the texture horizontal
    }
);


// Table Fragment Shader Source Code
const GLchar * TableFragmentShaderSource = GLSL(330,

        in vec3 FragmentPos; // For incoming fragment position
        in vec3 Normal; // For incoming normals
        in vec2 mobileTextureCoordinate;

        out vec4 TableColor; // For outgoing table color to the GPU

        // Uniform / Global variables for object color, light color, light position, and camera / view position
        uniform vec3 lightColor; // Key light color
        uniform vec3 lightPos; // Key light position
        uniform vec3 fillLightColor; // Fill light color
        uniform vec3 fillLightPos; // Fill light position
        uniform vec3 viewPosition; // Key light view

        uniform sampler2D uTexture; // Useful when working with multiple textures

        void main(){
        // Phong lighting model calculations to generate ambient, diffuse, and specular components
        	vec3 norm = normalize(Normal); // Normalize vectors to 1 unit
            vec3 viewDir = normalize(viewPosition - FragmentPos); // Calculate view direction

        // Key Lighting
            // Calculate Ambient Lighting
            float ambientStrength = 0.5f; // Set ambient or global lighting strength
            vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

            // Calculate Diffuse Lighting
            vec3 lightDirection = normalize(lightPos - FragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on
            float impact = max(dot(norm, lightDirection), 0.0); // Calculate diffuse impact by generating dot product of normal and light
            vec3 diffuse = impact * lightColor; // Generate diffuse light color

            // Calculate Specular lighting
            float specularIntensity = 1.0f; // Set specular light strength
            float highlightSize = 10.0f; // Set specular highlight size
            vec3 reflectDir = reflect(-lightDirection, norm); // Calculate reflection vector

            // Calculate specular component
            float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
            vec3 specular = specularIntensity * specularComponent * lightColor;

		// Fill Lighting
			// Calculate Ambient Lighting
			float fillAmbientStrength = 0.5f; // Set ambient or global lighting strength
			vec3 fillAmbient = fillAmbientStrength * fillLightColor; // Generate ambient light color

			// Calculate Diffuse Lighting
			vec3 fillLightDirection = normalize(fillLightPos - FragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on
			float fillImpact = max(dot(norm, fillLightDirection), 0.0); // Calculate diffuse impact by generating dot product of normal and light
			vec3 fillDiffuse = fillImpact * fillLightColor; // Generate diffuse light color

			// Calculate Specular lighting
			float fillSpecularIntensity = 0.1f; // Set specular light strength
			float fillHighlightSize = 5.0f; // Set specular highlight size
			vec3 fillReflectDir = reflect(-fillLightDirection, norm); // Calculate reflection vector

			// Calculate specular component
			float fillSpecularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
			vec3 fillSpecular = fillSpecularIntensity * fillSpecularComponent * fillLightColor;

            // Calculate phong result
            vec3 objectColor = texture(uTexture, mobileTextureCoordinate).xyz;
            vec3 keyLightResult = (ambient + diffuse + specular);
            vec3 fillLightResult = (fillAmbient + fillDiffuse + fillSpecular);
            vec3 phong = (keyLightResult + fillLightResult) * objectColor;
            TableColor = vec4(phong, 1.0f); // Send lighting results to GPU

        }
);


// Lamp Shader Source Code
const GLchar * lampVertexShaderSource = GLSL(330,

        layout (location = 0) in vec3 position; // VAP position 0 for vertex position data

        // Uniform / Global variables for the transform matrices
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * view *model * vec4(position, 1.0f); // Transforms vertices into clip coordinates
        }
);

// Fragment Shader Source Code
const GLchar * lampFragmentShaderSource = GLSL(330,

        out vec4 color; // For outgoing lamp color

        void main()
        {
            color = vec4(1.0f); // Set color to white (1.0f, 1.0f, 1.0f) with Alpha 1.0

        }
);


// Main Program
int main(int argc, char* argv[]){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WindowWidth, WindowHeight);
    glutCreateWindow(WINDOW_TITLE);

    glutReshapeFunc(UResizeWindow);

    glewExperimental = GL_TRUE;
            if (glewInit() != GLEW_OK) {
                std::cout<< "Failed to initialize GLEW" << std::endl;
                return -1;
            }

    UCreateShader();

    UCreateBuffers();

    UGenerateTexture();

	glutMouseFunc(UMouseClick); // Detects mouse click

	glutMotionFunc(UMousePressedMove); // Detects mouse press and movement

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color

    glutDisplayFunc(URenderGraphics);

	glutKeyboardFunc(UKeyboard); // Detects key press

	glutMainLoop();

    // Destroys Buffer objects once used
    glDeleteVertexArrays(1, &TableVAO);
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

    GLint modelLoc, viewLoc, projLoc, uTextureLoc, lightColorLoc, lightPositionLoc, viewPositionLoc,
			fillLightColorLoc, fillLightPositionLoc;

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    // Use the Table Shader to activate the Table Vertex Array Object for rendering and transforming
    glUseProgram(tableShaderProgram);
    glBindVertexArray(TableVAO);

    // Camera movement logic
	CameraForwardZ = cameraPosition; // Replaces camera forward vector with radians normalized as a unit vector

    // Transform the Table
    model = glm::translate(model, tablePosition);
    model = glm::scale(model, tableScale);

    // Transform the camera
    view = glm::translate(view, cameraPosition);
    view = glm::rotate(view, cameraRotation, glm::vec3(0.0f, 1.0f, 0.0f));
	view = glm::lookAt(cameraPosition, glm::vec3(0.0f, 0.0f, 0.0f), CameraUpY);

	// Set the camera projection to orthographic (2D)
	if(currentKey != '3'){
		projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
	}

	// Set the camera projection to perspective (3D)
	if(currentKey !=  '2'){
		projection = glm::perspective(45.0f,(GLfloat)WindowWidth / (GLfloat)WindowHeight, 0.1f, 100.0f);
	}

    // Reference matrix uniforms from the Table Shader program
    modelLoc = glGetUniformLocation(tableShaderProgram, "model");
    viewLoc = glGetUniformLocation(tableShaderProgram, "view");
    projLoc = glGetUniformLocation(tableShaderProgram, "projection");

    // Pass matrix data to the Table Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Table Shader program for the Table color, light color, light position, and camera position
    uTextureLoc = glGetUniformLocation(tableShaderProgram, "uTexture");
    lightColorLoc = glGetUniformLocation(tableShaderProgram, "lightColor"); // Key light color
    lightPositionLoc = glGetUniformLocation(tableShaderProgram, "lightPos"); // Key light position
    fillLightColorLoc = glGetUniformLocation(tableShaderProgram, "fillLightColor"); // Fill light color
    fillLightPositionLoc = glGetUniformLocation(tableShaderProgram, "fillLightPos"); // Fill light position
    viewPositionLoc = glGetUniformLocation(tableShaderProgram, "viewPosition");

    // Pass color, light, and camera data to the Table Shader programs corresponding uniforms
    glUniform1i(uTextureLoc, 0); // Texture unit 0
    glUniform3f(lightColorLoc, lightColor.r, lightColor.g, lightColor.b); // Key light color
    glUniform3f(lightPositionLoc, lightPosition.x, lightPosition.y, lightPosition.z); // Key light position
    glUniform3f(fillLightColorLoc, fillLightColor.r, fillLightColor.g, fillLightColor.b); // Fill light color
    glUniform3f(fillLightPositionLoc, fillLightPosition.x, fillLightPosition.y, fillLightPosition.z); // Fill light position
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    glDrawArrays(GL_TRIANGLES, 0, 228); // Draw the primitives / Table

    glBindVertexArray(0); // Deactivate the Table Vertex Array Object

    // Use the Lamp Shader and activate the Lamp Vertex Array Object for rendering and transforming
    glUseProgram(lampShaderProgram);
    glBindVertexArray(LightVAO);

    // Transform the smaller Table used as a visual cue for the light source
    model = glm::translate(model, lightPosition);
    model = glm::scale(model, lightScale);

    // Reference matrix uniforms from the Lamp Shader program
    modelLoc = glGetUniformLocation(lampShaderProgram, "model");
    viewLoc = glGetUniformLocation(lampShaderProgram, "view");
    projLoc = glGetUniformLocation(lampShaderProgram, "projection");

    // Pass matrix uniforms from the Lamp Shader Program
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLES, 0, 36); // Draws the triangles

    glBindVertexArray(0); // Deactivate the Lamp Vertex Array Object

	glutPostRedisplay();
    glutSwapBuffers(); // Flips the back buffer with the front buffer every frame. Similar to GL Flush

}

// Create the Shader program
void UCreateShader()
{
    // Table Vertex shader
    GLint TableVertexShader = glCreateShader(GL_VERTEX_SHADER); // Creates the Vertex shader
    glShaderSource(TableVertexShader, 1, &tableVertexShaderSource, NULL); // Attaches the Vertex shader to the source code
    glCompileShader(TableVertexShader); // Compiles the Vertex shader

    // Table Fragment Shader
    GLint TableFragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // Creates the Fragment Shader
    glShaderSource(TableFragmentShader, 1, &TableFragmentShaderSource, NULL); // Attaches the Fragment shader to the source code
    glCompileShader(TableFragmentShader); // Compiles the Fragment Shader

    // Table Shader program
    tableShaderProgram = glCreateProgram(); // Creates the Shader program and returns an id
    glAttachShader(tableShaderProgram, TableVertexShader); // Attaches Vertex shader to the Shader program
    glAttachShader(tableShaderProgram, TableFragmentShader); // Attaches Fragment shader to the Shader program
    glLinkProgram(tableShaderProgram); // Link Vertex and Fragment shaders to the Shader program

    // Delete the Vertex and Fragment shaders once linked
    glDeleteShader(TableVertexShader);
    glDeleteShader(TableFragmentShader);

    // Lamp Vertex shader
    GLint lampVertexShader = glCreateShader(GL_VERTEX_SHADER); // Creates the Vertex shader
    glShaderSource(lampVertexShader, 1, &lampVertexShaderSource, NULL); // Attaches the Vertex shader to the source code
    glCompileShader(lampVertexShader); // Compiles the Vertex shader

    // Lamp Fragment shader
    GLint lampFragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // Creates the Fragment shader
    glShaderSource(lampFragmentShader, 1, &lampFragmentShaderSource, NULL); // Attaches the Fragment shader to the source code
    glCompileShader(lampFragmentShader); // Compiles the Fragment shader

    // Lamp Shader Program
    lampShaderProgram = glCreateProgram(); // Creates the Shader program and returns an id
    glAttachShader(lampShaderProgram, lampVertexShader); // Attach Vertex shader to the Shader program
    glAttachShader(lampShaderProgram, lampFragmentShader); // Attach Fragment shader to the Shader program
    glLinkProgram(lampShaderProgram); // Link Vertex and Fragment shaders to the Shader program

    // Delete the lamp shaders once linked
    glDeleteShader(lampVertexShader);
    glDeleteShader(lampFragmentShader);

}

// Creates the Buffer and Array Objects
void UCreateBuffers()
{
    // Position, Normals, and Texture coordinate data for table triangles
	GLfloat vertices[] = {
							// Position					// Normals				//Textures
							// Base of Table
							// Bottom Surface
						   -0.25f, 0.0f, -0.25f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f,
						   -0.25f, 0.0f,  2.25f,		0.0f, -1.0f, 0.0f,		0.0f, 1.0f,
							1.25f, 0.0f, -0.25f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,
							1.25f, 0.0f,  2.25f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,
						   -0.25f, 0.0f,  2.25f,		0.0f, -1.0f, 0.0f,		0.0f, 1.0f,
							1.25f, 0.0f, -0.25f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,

							// Top Surface
							-0.25f, 0.25f, -0.25f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f,
							-0.25f, 0.25f,  2.25f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,
							1.25f,  0.25f, -0.25f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
							1.25f,  0.25f,  2.25f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f,
							-0.25f, 0.25f,  2.25f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,
							1.25f,  0.25f, -0.25f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,

							// Left Side
							-0.25f,  0.0f, -0.25f,		-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
							-0.25f,  0.0f,  2.25f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
							-0.25f, 0.25f, -0.25f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
							-0.25f, 0.25f, -0.25f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
							-0.25f, 0.25f,  2.25f,		-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
							-0.25f,  0.0f,  2.25f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,

							// Right Side
							1.25f,  0.0f, -0.25f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
							1.25f,  0.0f,  2.25f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
							1.25f, 0.25f, -0.25f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
							1.25f, 0.25f, -0.25f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
							1.25f, 0.25f,  2.25f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
							1.25f,  0.0f,  2.25f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,

							// Forward Side
						   -0.25f,  0.0f, -0.25f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f,
							1.25f,  0.0f, -0.25f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f,
						   -0.25f, 0.25f, -0.25f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
						   -0.25f, 0.25f, -0.25f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
							1.25f, 0.25f, -0.25f,		0.0f, 0.0f, -1.0f,		1.0f, 0.0f,
							1.25f,  0.0f, -0.25f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f,
							// Back side
						   -0.25f,  0.0f, 2.25f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,
							1.25f,  0.0f, 2.25f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,
						   -0.25f, 0.25f, 2.25f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
						   -0.25f, 0.25f, 2.25f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
							1.25f, 0.25f, 2.25f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
							1.25f,  0.0f, 2.25f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,

							// Legs
							// Front
							0.0f,  0.25f, 0.0f,			0.0f, 0.0f, -1.0f,		0.0f, 1.0f,
							0.25f, 0.25f, 0.0f,			0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
							0.0f,  -1.0f, 0.0f,			0.0f, 0.0f, -1.0f,		0.0f, 0.0f,
							0.0f,  -1.0f, 0.0f,			0.0f, 0.0f, -1.0f,		0.0f, 0.0f,
							0.25f, 0.25f, 0.0f,			0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
							0.25f, -1.0f, 0.0f,     	0.0f, 0.0f, -1.0f,		1.0f, 0.0f,
							// Back
							0.0f,  0.25f, 0.25f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,
							0.25f, 0.25f, 0.25f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
							0.0f,  -1.0f, 0.25f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,
							0.0f,  -1.0f, 0.25f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,
							0.25f, 0.25f, 0.25f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
							0.25f, -1.0f, 0.25f,    	0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
							// Bottom
							0.0f,  -1.0f,  0.0f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f,
							0.25f, -1.0f,  0.0f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,
							0.0f,  -1.0f, 0.25f,		0.0f, -1.0f, 0.0f,		0.0f, 1.0f,
							0.25f, -1.0f,  0.0f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,
							0.0f,  -1.0f, 0.25f,		0.0f, -1.0f, 0.0f,		0.0f, 1.0f,
							0.25f, -1.0f, 0.25f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,
							// Left
							0.0f,  0.25f,  0.0f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
							0.0f,  0.25f, 0.25f,		-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
							0.0f,  -1.0f, 0.25f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
							0.0f,  -1.0f, 0.25f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
							0.0f,  -1.0f,  0.0f,		-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
							0.0f,  0.25f,  0.0f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
							// Right
							0.25f,  0.25f,  0.0f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
							0.25f,  0.25f, 0.25f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
							0.25f,  -1.0f, 0.25f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
							0.25f,  -1.0f, 0.25f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
							0.25f,  -1.0f,  0.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
							0.25f,  0.25f,  0.0f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,

							// Front
							0.0f,  0.25f, 1.75f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f,
							0.25f, 0.25f, 1.75f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
							0.0f,  -1.0f, 1.75f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f,
							0.0f,  -1.0f, 1.75f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f,
							0.25f, 0.25f, 1.75f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
							0.25f, -1.0f, 1.75f,    	0.0f, 0.0f, -1.0f,		1.0f, 0.0f,
							// Back
							0.0f,  0.25f, 2.0f,	    	0.0f, 0.0f, 1.0f,		0.0f, 1.0f,
							0.25f, 0.25f, 2.0f,			0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
							0.0f,  -1.0f, 2.0f,			0.0f, 0.0f, 1.0f,		0.0f, 0.0f,
							0.0f,  -1.0f, 2.0f,			0.0f, 0.0f, 1.0f,		0.0f, 0.0f,
							0.25f, 0.25f, 2.0f,			0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
							0.25f, -1.0f, 2.0f,     	0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
							// Bottom
							0.0f,  -1.0f, 1.75f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f,
							0.25f, -1.0f, 1.75f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,
							0.0f,  -1.0f,  2.0f,		0.0f, -1.0f, 0.0f,		0.0f, 1.0f,
							0.25f, -1.0f, 1.75f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,
							0.0f,  -1.0f,  2.0f,		0.0f, -1.0f, 0.0f,		0.0f, 1.0f,
							0.25f, -1.0f,  2.0f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,
							// Left
							0.0f,  0.25f, 1.75f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
							0.0f,  0.25f,  2.0f,		-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
							0.0f,  -1.0f,  2.0f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
							0.0f,  -1.0f,  2.0f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
							0.0f,  -1.0f, 1.75f,		-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
							0.0f,  0.25f, 1.75f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
							// Right
							0.25f,  0.25f, 1.75f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
							0.25f,  0.25f,  2.0f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
							0.25f,  -1.0f,  2.0f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
							0.25f,  -1.0f,  2.0f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
							0.25f,  -1.0f, 1.75f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
							0.25f,  0.25f, 1.75f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,

							// Front
							0.75f, 0.25f, 1.75f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f,
							1.0f,  0.25f, 1.75f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
							0.75f, -1.0f, 1.75f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f,
							0.75f, -1.0f, 1.75f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f,
							1.0f,  0.25f, 1.75f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
							1.0f,  -1.0f, 1.75f,   		0.0f, 0.0f, -1.0f,		1.0f, 0.0f,
							// Back
							0.75f, 0.25f, 2.0f,			0.0f, 0.0f, 1.0f,		0.0f, 1.0f,
							1.0f,  0.25f, 2.0f,			0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
							0.75f, -1.0f, 2.0f,			0.0f, 0.0f, 1.0f,		0.0f, 0.0f,
							0.75f, -1.0f, 2.0f,			0.0f, 0.0f, 1.0f,		0.0f, 0.0f,
							1.0f,  0.25f, 2.0f,			0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
							1.0f,  -1.0f, 2.0f,     	0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
							// Bottom
							0.75f, -1.0f, 1.75f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f,
							1.0f,  -1.0f, 1.75f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,
							0.75f, -1.0f,  2.0f,		0.0f, -1.0f, 0.0f,		0.0f, 1.0f,
							1.0f,  -1.0f, 1.75f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,
							0.75f, -1.0f,  2.0f,		0.0f, -1.0f, 0.0f,		0.0f, 1.0f,
							1.0f,  -1.0f,  2.0f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,
							// Left
							0.75f,  0.25f, 1.75f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
							0.75f,  0.25f,  2.0f,		-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
							0.75f,  -1.0f,  2.0f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
							0.75f,  -1.0f,  2.0f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
							0.75f,  -1.0f, 1.75f,		-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
							0.75f,  0.25f, 1.75f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
							// Right
							1.0f,  0.25f, 1.75f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
							1.0f,  0.25f,  2.0f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
							1.0f,  -1.0f,  2.0f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
							1.0f,  -1.0f,  2.0f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
							1.0f,  -1.0f, 1.75f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
							1.0f,  0.25f, 1.75f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,

							// Front
							0.75f, 0.25f, 0.0f,			0.0f, 0.0f, -1.0f,		0.0f, 1.0f,
							1.0f,  0.25f, 0.0f,			0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
							0.75f, -1.0f, 0.0f,			0.0f, 0.0f, -1.0f,		0.0f, 0.0f,
							0.75f, -1.0f, 0.0f,			0.0f, 0.0f, -1.0f,		0.0f, 0.0f,
							1.0f,  0.25f, 0.0f,			0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
							1.0f,  -1.0f, 0.0f,     	0.0f, 0.0f, -1.0f,		1.0f, 0.0f,
							// Back
							0.75f, 0.25f, 0.25f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,
							1.0f,  0.25f, 0.25f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
							0.75f, -1.0f, 0.25f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,
							0.75f, -1.0f, 0.25f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,
							1.0f,  0.25f, 0.25f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
							1.0f,  -1.0f, 0.25f,    	0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
							// Bottom
							0.75f, -1.0f,  0.0f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f,
							1.0f,  -1.0f,  0.0f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,
							0.75f, -1.0f, 0.25f,		0.0f, -1.0f, 0.0f,		0.0f, 1.0f,
							1.0f,  -1.0f,  0.0f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,
							0.75f, -1.0f, 0.25f,		0.0f, -1.0f, 0.0f,		0.0f, 1.0f,
							1.0f,  -1.0f, 0.25f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,
							// Left
							0.75f,  0.25f,  0.0f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
							0.75f,  0.25f, 0.25f,		-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
							0.75f,  -1.0f, 0.25f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
							0.75f,  -1.0f, 0.25f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
							0.75f,  -1.0f,  0.0f,		-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
							0.75f,  0.25f,  0.0f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
							// Right
							1.0f,  0.25f,  0.0f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
							1.0f,  0.25f, 0.25f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
							1.0f,  -1.0f, 0.25f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
							1.0f,  -1.0f, 0.25f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
							1.0f,  -1.0f,  0.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
							1.0f,  0.25f,  0.0f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,

							// Support Braces
							// Front brace
							// Back
							0.25f,   0.0f, 0.25f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
							0.75f,   0.0f, 0.25f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
							0.25f, -0.25f, 0.25f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,
							0.25f, -0.25f, 0.25f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,
							0.75f,   0.0f, 0.25f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
							0.75f, -0.25f, 0.25f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,

							// Front
							0.25f,   0.0f, 0.0f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
							0.75f,   0.0f, 0.0f,		0.0f, 0.0f, -1.0f,		1.0f, 0.0f,
							0.25f, -0.25f, 0.0f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f,
							0.25f, -0.25f, 0.0f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f,
							0.75f,   0.0f, 0.0f,		0.0f, 0.0f, -1.0f,		1.0f, 0.0f,
							0.75f, -0.25f, 0.0f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f,

							// Bottom
							0.25f, -0.25f, 0.25f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f,
							0.75f, -0.25f, 0.25f,		0.0f, -1.0f, 0.0f,		0.0f, 1.0f,
							0.25f, -0.25f,  0.0f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,
							0.25f, -0.25f,  0.0f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,
							0.75f, -0.25f, 0.25f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f,
							0.75f, -0.25f,  0.0f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,

							// Back brace
							// Back
							0.25f,   0.0f, 2.0f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
							0.75f,   0.0f, 2.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
							0.25f, -0.25f, 2.0f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,
							0.25f, -0.25f, 2.0f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,
							0.75f,   0.0f, 2.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
							0.75f, -0.25f, 2.0f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,

							// Front
							0.25f,   0.0f, 1.75f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
							0.75f,   0.0f, 1.75f,		0.0f, 0.0f, -1.0f,		1.0f, 0.0f,
							0.25f, -0.25f, 1.75f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f,
							0.25f, -0.25f, 1.75f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f,
							0.75f,   0.0f, 1.75f,		0.0f, 0.0f, -1.0f,		1.0f, 0.0f,
							0.75f, -0.25f, 1.75f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f,

							// Bottom
							0.25f, -0.25f,  2.0f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f,
							0.75f, -0.25f,  2.0f,		0.0f, -1.0f, 0.0f,		0.0f, 1.0f,
							0.25f, -0.25f, 1.75f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,
							0.25f, -0.25f, 1.75f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,
							0.75f, -0.25f,  2.0f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f,
							0.75f, -0.25f, 1.75f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,

							// Left brace
							// Back
							0.0f,   0.0f, 0.25f,		-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
							0.0f,   0.0f, 1.75f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
							0.0f, -0.25f, 1.75f,		-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
							0.0f, -0.25f, 1.75f,		-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
							0.0f,   0.0f, 0.25f,		-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
							0.0f, -0.25f, 0.25f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,

							// Front
							0.25f,   0.0f, 0.25f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
							0.25f,   0.0f, 1.75f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
							0.25f, -0.25f, 1.75f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
							0.25f, -0.25f, 1.75f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
							0.25f,   0.0f, 0.25f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
							0.25f, -0.25f, 0.25f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,

							// Bottom
							0.0f,  -0.25f, 0.25f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f,
							0.25f, -0.25f, 0.25f,		0.0f, -1.0f, 0.0f,		0.0f, 1.0f,
							0.0f,  -0.25f, 1.75f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,
							0.0f,  -0.25f, 1.75f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,
							0.25f, -0.25f, 0.25f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f,
							0.25f, -0.25f, 1.75f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,

							// Right brace
							// Back
							1.0f,   0.0f, 0.25f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
							1.0f,   0.0f, 1.75f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
							1.0f, -0.25f, 1.75f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
							1.0f, -0.25f, 1.75f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
							1.0f,   0.0f, 0.25f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
							1.0f, -0.25f, 0.25f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,

							// Front
							0.75f,   0.0f, 0.25f,		-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
							0.75f,   0.0f, 1.75f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
							0.75f, -0.25f, 1.75f,		-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
							0.75f, -0.25f, 1.75f,		-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
							0.75f,   0.0f, 0.25f,		-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
							0.75f, -0.25f, 0.25f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,

							// Bottom
							0.75f, -0.25f, 0.25f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f,
							1.0f,  -0.25f, 0.25f,		0.0f, -1.0f, 0.0f,		0.0f, 1.0f,
							0.75f, -0.25f, 1.75f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,
							0.75f, -0.25f, 1.75f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,
							1.0f,  -0.25f, 0.25f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f,
							1.0f,  -0.25f, 1.75f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,

						  };

    // Generate buffer ids
    glGenVertexArrays(1, &TableVAO);
    glGenBuffers(1, &VBO);

    // Activate the TableVAO before binding and setting VBOs and VAPs
    glBindVertexArray(TableVAO);

    // Activate the VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Copy vertices to VBO

    // Set attribute pointer 0 to hold position data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0); // Enables vertex attribute

    // Set attribute pointer 1 to hold Normal data
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Set attribute pointer 2 to hold Texture coordinate data
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind the object VAO

    // Generate buffer ids for lamp
    glGenVertexArrays(1, &LightVAO); // Vertex Array for object vertex copies to serve as light source

    // Activate the Vertex Array Object before binding and setting any VBOs and Vertex Attribute Pointers
    glBindVertexArray(LightVAO);

    // Referencing the same VBO for its vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Set attribute pointer to 0 to hold Position data (used for the lamp)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

}

// Implement UMouseClick Function
void UMouseClick(int button, int state, int x, int y){

	if((button == GLUT_LEFT_BUTTON)	&&(state == GLUT_DOWN)){
		mouseLeft = true;
	}
	if((button == GLUT_LEFT_BUTTON)	&&(state == GLUT_UP)){
		mouseLeft = false;
	}

	if((button == GLUT_RIGHT_BUTTON) &&(state == GLUT_DOWN)){
		mouseRight = true;
	}
	if((button == GLUT_RIGHT_BUTTON) &&(state == GLUT_UP)){
		mouseRight = false;
	}
}

// Implements UMouseMove function
void UMousePressedMove(int x, int y) {

	if(glutGetModifiers() == GLUT_ACTIVE_ALT){
		altKey = true;
	}
	else{
		altKey = false;
	}

	// Immediately replaces center locked coordinates with new mouse coordinates
	if(mouseDetected) {
		lastMouseX = x;
		lastMouseY = y;
		mouseDetected = false;
	}

	// Get direction mouse was moved in x and y
	mouseXOffset = x - lastMouseX;
	mouseYOffset = lastMouseY - y; // Inverted y

	// Updates with new mouse coordinates
	lastMouseX = x;
	lastMouseY = y;

	// Applies sensitivity to mouse direction
	mouseXOffset *= sensitivity;
	mouseYOffset *= sensitivity;

	// Accumulates yaw and pitch variables
	yaw += mouseXOffset;
	pitch += mouseYOffset;

	// Maintains a 90 degree pitch for gimbal lock
	if(pitch > 89.0f)
		pitch = 89.0f;

	if(pitch < -89.0f)
		pitch = -89.0f;

	// Orbit
	if(mouseLeft == true && altKey == true){
		// Orbits around center
		cameraPosition.x = 3.0f * cos(yaw);
		cameraPosition.y = 3.0f * sin(pitch);
		cameraPosition.z = sin(yaw) * cos(pitch) * 10.0f;

		if(mouseXOffset < 0)
			cameraPosition -= glm::normalize(glm::cross(CameraForwardZ, CameraUpY)) * cameraSpeed; // Orbit Right
		if(mouseXOffset > 0)
			cameraPosition += glm::normalize(glm::cross(CameraForwardZ, CameraUpY)) * cameraSpeed; // Orbit Left
		if(mouseYOffset < 0)
			cameraPosition -= CameraUpY * cameraSpeed; // Orbit Up
		if(mouseYOffset > 0)
			cameraPosition += CameraUpY * cameraSpeed; // Orbit down

	}
}

// Generate and load texture
void UGenerateTexture(){
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	int width, height;

	unsigned char* image = SOIL_load_image("woodTexture.jpg", &width, &height, 0, SOIL_LOAD_RGB); //Load texture file

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture
}

// Implement UKeyboard Function
void UKeyboard(unsigned char key, GLint x, GLint y) {
	switch(key){

		case '2':
			currentKey = key;
			cout << "Switching to 2D!" << endl;
			break;

		case '3':
			currentKey = key;
			cout << "Switching to 3D!" << endl;
			break;

		default:
			cout << "Press 2 for 2D and 3 for 3D!" << endl;
	}
}
