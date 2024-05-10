#if defined (__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

#include <glm/glm.hpp>//core glm functionality
#include <glm/gtc/matrix_transform.hpp>//glm extension for generating common transformation matrices
#include <glm/gtc/type_ptr.hpp>
#include <string>

#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"

int glWindowWidth = 2000;
int glWindowHeight = 2000;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

glm::mat4 model;
GLint modelLoc;

gps::Camera myCamera(
	glm::vec3(0.0f, 0.0f, 3.0f),
	glm::vec3(0.0f, 0.0f, -10.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));


gps::Camera myCameraPresentation(
	glm::vec3(0.0f, 0.0f, 3.0f),
	glm::vec3(0.0f, 0.0f, -10.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));


bool enableFog = false;
float cameraSpeed = 1.7f;

bool pressedKeys[1024];
float angle = 0.0f;

gps::Model3D myModel;
gps::Shader myCustomShader;

GLuint verticesVBO;
GLuint verticesEBO;
GLuint objectVAO;
GLuint texture;


//mouse
bool firstMouse = true;
double lastX, lastY, mouseSensitivity = 0.1f;
double yaw = -90.0f, pitch = 0.0f;


//vertex position and UV coordinates
GLfloat vertexData[] = {
	//  first triangle
	0.0f
};

GLuint vertexIndices[] = {
		0
};

bool presentation = 0;

int steps;
void startPresentation()
{
	
	steps = 0;
	presentation = true;
	myCameraPresentation.set(glm::vec3(0.294602f, 0.238419f,0.206296f), glm::vec3(0.845735f, 0.157867f, 0.909576f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void progress()
{
	cameraSpeed = 0.4;

	 if(steps == 400)
	{

		myCameraPresentation.set(glm::vec3(-0.667712f, 0.340966f, 0.83341f), glm::vec3 (0.845735f, 0.157867f, 0.909576f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (steps == 800)
	{
		myCameraPresentation.set(glm::vec3(-0.667712f, 0.340966f, 0.83341f), glm::vec3(-0.842288f, 0.23734f, -0.621717f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (steps == 1200)
	{
		myCameraPresentation.set(glm::vec3(-0.658663, 0.231886, 0.962313), glm::vec3(0.371196f, 0.415112f, -0.708478f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (steps == 1600)
	{
		myCameraPresentation.rotate(0.0f, 180.0f);
		myCameraPresentation.set(glm::vec3(0.0914013f, 3.44026f, 0.8403f), glm::vec3(-0.0146392, 0.126679f, 0.204175f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	} 
	else  if (steps == 2000)
	{
		presentation = false;
		steps = 0;
	}


	if (steps < 400)
	{
		myCameraPresentation.move(gps::MOVE_BACKWARD, cameraSpeed);
	}
	else if (steps < 800)
	{
		myCameraPresentation.move(gps::MOVE_FORWARD, cameraSpeed);
	}
	else if (steps < 1200)
	{
		myCameraPresentation.move(gps::MOVE_FORWARD, cameraSpeed);
	}
	else if (steps < 1600)
	{
		myCameraPresentation.move(gps::MOVE_BACKWARD, cameraSpeed);
	}
	else if (steps < 2000)
	{
		myCameraPresentation.move(gps::MOVE_BACKWARD, cameraSpeed);
		myCameraPresentation.move(gps::MOVE_UP, cameraSpeed);
		
		//myCameraPresentation.move(gps::MOVE_BACKWARD, cameraSpeed);
		//myCameraPresentation.move(gps::MOVE_BACKWARD, cameraSpeed);
		//myCameraPresentation.move(gps::MOVE_BACKWARD, cameraSpeed);



	
	}

	steps++;
}

#define MAX_RAIN_DROPS 1000

struct Raindrop {
	float x;
	float y;
	float speed;
};

std::vector<Raindrop> raindrops;

// Initialize raindrop properties
void initRain() {
	raindrops.clear();
	srand(time(NULL));
	for (int i = 0; i < MAX_RAIN_DROPS; ++i) {
		Raindrop drop;
		drop.x = (float)(rand() % 1000);
		drop.y = (float)(rand() % 1000);
		drop.speed = 5.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 5.0f));
		raindrops.push_back(drop);
	}
}

void drawRain() {
	glColor3f(0.0, 0.0, 1.0); 
	glPointSize(4.0);
	glBegin(GL_POINTS);
	for (const auto& drop : raindrops) {
		glVertex2f(drop.x, drop.y);
	}
	glEnd();
}

// Function to update raindrop positions
void updateRain() {
	for (auto& drop : raindrops) {
		drop.y -= drop.speed;
		if (drop.y < 0) {
			drop.y = static_cast<float>(glWindowHeight);
			drop.x = static_cast<float>(rand() % glWindowWidth);
		}
	}
}



void windowResizeCallback(GLFWwindow* window, int width, int height) {

	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		initRain();
	}

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		presentation = !presentation;
		startPresentation();
	}

	if (key == GLFW_KEY_O && action == GLFW_PRESS)
	{
	
		enableFog = !enableFog;
		if (enableFog)
		{
			// Fog density
			float fogDensity = 0.01f;
			GLint fogDensityLoc = glGetUniformLocation(myCustomShader.shaderProgram, "fogDensity");
			glUniform1f(fogDensityLoc, fogDensity);

			
			glm::vec3 fogColor = glm::vec3(0.8f, 0.8f, 0.8f);  
			GLint fogColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "fogColor");
			glUniform3fv(fogColorLoc, 1, glm::value_ptr(fogColor));
		}
	}


	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			pressedKeys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			pressedKeys[key] = false;
		}
	}
}
void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (presentation) return;
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (yaw > 360.0f)
		yaw -= 360.0f;
	else if (yaw < 0.0f)
		yaw += 360.0f;

	if (pitch > 89.0f)
		pitch = 89.0f;
	else if (pitch < -89.0f)
		pitch = -89.0f;

	myCamera.rotate(pitch, yaw);

	
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		glm::vec3 cameraPosition = myCamera.cameraPosition;
		std::cout << "Camera Position - X: " << cameraPosition.x << ", Y: " << cameraPosition.y << ", Z: " << cameraPosition.z << std::endl;
		
		std::cout << "Camera Front Direction - X: " << myCameraPresentation.cameraPosition.x << ", Y: " << myCameraPresentation.cameraPosition.y << ", Z: " << myCameraPresentation.cameraPosition.z << std::endl;

	}
}



GLfloat angleY, lightAngle, windAngle;
void processMovement()
{
	if (presentation) return;
	if (pressedKeys[GLFW_KEY_W])
	{
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S])
	{
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A])
	{
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D])
	{
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_SPACE])
	{
		myCamera.move(gps::MOVE_UP, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_M])
	{
		myCamera.move(gps::MOVE_DOWN, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_Q])
	{
		lightAngle -= 1.00f;
		if (lightAngle < 0.0f) lightAngle += 360.0f;
	}

	if (pressedKeys[GLFW_KEY_E])
	{
		lightAngle += 1.00f;
		if (lightAngle > 360.0f) lightAngle -= 360.0f;
	}

	if (pressedKeys[GLFW_KEY_Z])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (pressedKeys[GLFW_KEY_X])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (pressedKeys[GLFW_KEY_C])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
}



void setWindowCallbacks()
{
	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
}



bool initOpenGLWindow() {

	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//window scaling for HiDPI displays
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

	// for multisampling/antialising
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Texturing", NULL, NULL);

	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	//glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

#if not defined (__APPLE__)
	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();
#endif

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

GLuint ReadTextureFromFile(const char* file_name) {

	int x, y, n;
	int force_channels = 4;
	unsigned char* image_data = stbi_load(file_name, &x, &y, &n, force_channels);

	if (!image_data) {
		fprintf(stderr, "ERROR: could not load %s\n", file_name);
		return false;
	}
	// NPOT check
	if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
		fprintf(
			stderr, "WARNING: texture %s is not power-of-2 dimensions\n", file_name
		);
	}

	int width_in_bytes = x * 4;
	unsigned char* top = NULL;
	unsigned char* bottom = NULL;
	unsigned char temp = 0;
	int half_height = y / 2;

	for (int row = 0; row < half_height; row++) {

		top = image_data + row * width_in_bytes;
		bottom = image_data + (y - row - 1) * width_in_bytes;

		for (int col = 0; col < width_in_bytes; col++) {

			temp = *top;
			*top = *bottom;
			*bottom = temp;
			top++;
			bottom++;
		}
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_SRGB, //GL_SRGB,//GL_RGBA,
		x,
		y,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		image_data
	);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

void renderScene()
{
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawRain();
	glViewport(0, 0, retina_width, retina_height);

	//initialize the model matrix
	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");

	myCustomShader.useShaderProgram();





	//initialize the view matrix
	glm::mat4 view = myCamera.getViewMatrix();


	if (!presentation)view = myCamera.getViewMatrix();
	else
	{
		progress();
		cameraSpeed = 1.7;
		
		view = myCameraPresentation.getViewMatrix();
	}


	//send matrix data to shader
	GLint viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));


	GLint enableFogLoc = glGetUniformLocation(myCustomShader.shaderProgram, "enableFog");
	glUniform1i(enableFogLoc, enableFog);


	//create rotation matrix
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//send matrix data to vertex shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));


	myModel.Draw(myCustomShader);
}

void loadTriangleData() {

	glGenVertexArrays(1, &objectVAO);

	glBindVertexArray(objectVAO);

	glGenBuffers(1, &verticesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &verticesEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, verticesEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertexIndices), vertexIndices, GL_STATIC_DRAW);

	//vertex position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//vertex texture
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}




void cleanup() {

	// GPU cleanup
	glDeleteBuffers(1, &verticesVBO);
	glDeleteBuffers(1, &verticesEBO);
	glDeleteVertexArrays(1, &objectVAO);
	glDeleteTextures(1, &texture);

	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

int main(int argc, const char* argv[]) {






	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	setWindowCallbacks();


	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_PROGRAM_POINT_SIZE);

	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();


	//initialize the projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(55.0f), (float)retina_width / (float)retina_height, 0.1f, 5000.0f);
	//send matrix data to shader
	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	myModel.LoadModel("objects/Proiect.obj", "textures/");

	initRain();

	while (!glfwWindowShouldClose(glWindow)) {
		processMovement();
		updateRain();
		renderScene();
		drawRain();


		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}