#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include <iostream>

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;

// camera
gps::Camera myCamera(
    glm::vec3(0.0f, 0.3f, 0.0f),
    glm::vec3(0.0f, 0.0f, -10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 0.05f;

GLboolean pressedKeys[1024];

// models
gps::Model3D teapot;
gps::Model3D bear;
gps::Model3D bearCage;
gps::Model3D ibex;
gps::Model3D ibexCage;
gps::Model3D deer;
gps::Model3D deerCage;
gps::Model3D moose;
gps::Model3D mooseCage;
gps::Model3D muskDeer;
gps::Model3D muskDeerCage;
gps::Model3D sheep;
gps::Model3D sheepCage;
gps::Model3D otter;
gps::Model3D otterCage;
gps::Model3D monkey;
gps::Model3D monkeyCage;
gps::Model3D koala;
gps::Model3D koalaCage;
gps::Model3D toucan;
gps::Model3D toucanCage;
gps::Model3D turkey;
gps::Model3D turkeyCage;
gps::Model3D turtle;
gps::Model3D turtleCage;
gps::Model3D seaTurtle;
gps::Model3D seaTurtleCage;
gps::Model3D penguin;
gps::Model3D penguinCage;
gps::Model3D bison;
gps::Model3D bisonCage;
gps::Model3D hyena;
gps::Model3D hyenaCage;
gps::Model3D wolf;
gps::Model3D wolfCage;
gps::Model3D fox;
gps::Model3D foxCage;
gps::Model3D ground;
GLfloat angle;

// shaders
gps::Shader myBasicShader;

std::vector<const GLchar*> faces;

gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	//TODO
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

bool firstMouse = true;
double lastX = 0, lastY = 0;
double yaw = -90.0f, pitch = 0.0f;
void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
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

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

    myCamera.rotate(pitch, yaw);
    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
}

void processMovement() {
	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		//update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

    if (pressedKeys[GLFW_KEY_Q]) {
        angle -= 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle += 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
    }
}

void initOpenGLWindow() {
    myWindow.Create(1024, 768, "OpenGL Project Core");
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initOpenGLState() {
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    teapot.LoadModel("models/teapot/teapot20segUT.obj");
    bear.LoadModel("models/animals/bear/bear.obj");
    bearCage.LoadModel("models/fences/fence2/fence2.obj");
    ibex.LoadModel("models/animals/ibex/ibex.obj");
    ibexCage.LoadModel("models/fences/fence1/fence1.obj");
    deer.LoadModel("models/animals/deer/deer.obj");
    deerCage.LoadModel("models/fences/fence1/fence1.obj");
    moose.LoadModel("models/animals/moose/moose.obj");
    mooseCage.LoadModel("models/fences/fence2/fence2.obj");
    muskDeer.LoadModel("models/animals/musk_deer/musk_deer.obj");
    muskDeerCage.LoadModel("models/fences/fence1/fence1.obj");
    sheep.LoadModel("models/animals/sheep/sheep.obj");
    sheepCage.LoadModel("models/fences/fence1/fence1.obj");
    otter.LoadModel("models/animals/otter/otter.obj");
    otterCage.LoadModel("models/fences/fence1/fence1.obj");
    monkey.LoadModel("models/animals/monkey/monkey.obj");
    monkeyCage.LoadModel("models/fences/fence1/fence1.obj");
    koala.LoadModel("models/animals/koala/koala.obj");
    koalaCage.LoadModel("models/fences/fence1/fence1.obj");
    toucan.LoadModel("models/animals/toucan/toucan.obj");
    toucanCage.LoadModel("models/fences/fence1/fence1.obj");
    turkey.LoadModel("models/animals/turkey/turkey.obj");
    turkeyCage.LoadModel("models/fences/fence1/fence1.obj");
    turtle.LoadModel("models/animals/turtle/turtle.obj");
    turtleCage.LoadModel("models/fences/fence1/fence1.obj");
    seaTurtle.LoadModel("models/animals/sea_turtle/sea_turtle.obj");
    seaTurtleCage.LoadModel("models/fences/fence1/fence1.obj");
    penguin.LoadModel("models/animals/penguin/penguin.obj");
    penguinCage.LoadModel("models/fences/fence1/fence1.obj");
    bison.LoadModel("models/animals/bison/bison.obj");
    bisonCage.LoadModel("models/fences/fence1/fence1.obj");
    hyena.LoadModel("models/animals/hyena/hyena.obj");
    hyenaCage.LoadModel("models/fences/fence1/fence1.obj");
    wolf.LoadModel("models/animals/wolf/wolf.obj");
    wolfCage.LoadModel("models/fences/fence1/fence1.obj");
    fox.LoadModel("models/animals/fox/fox.obj");
    foxCage.LoadModel("models/fences/fence1/fence1.obj");
    ground.LoadModel("models/ground/ground.obj");
}

void initShaders() {
	myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");
}

void initUniforms() {
	myBasicShader.useShaderProgram();

    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	// send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
                               (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
                               0.1f, 40.0f);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	// send light dir to shader
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
	// send light color to shader
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
}

void renderTeapot(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(10, 0, 0));

    //send teapot model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw teapot
    teapot.Draw(shader);
}

void renderBear(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(5, 0, 15));
    model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));
    
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    bear.Draw(shader);
}

void renderBearCage(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(5, 0, 15));
    model = glm::scale(model, glm::vec3(0.1, 0.05, 0.1));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    bearCage.Draw(shader);
}

void renderIbex(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-5, 0, -5));
    model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    ibex.Draw(shader);
}

void renderIbexCage(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-5, 0, -5));
    model = glm::scale(model, glm::vec3(0.1, 0.05, 0.1));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    ibexCage.Draw(shader);
}

void renderDeer(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(5, 0, -5));
    model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    deer.Draw(shader);
}

void renderDeerCage(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(5, 0, -5));
    model = glm::scale(model, glm::vec3(0.1, 0.05, 0.1));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    deerCage.Draw(shader);
}

void renderMoose(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-10, 0, -10));
    model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    moose.Draw(shader);
}

void renderMooseCage(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-10, 0, -10));
    model = glm::scale(model, glm::vec3(0.1, 0.05, 0.1));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    mooseCage.Draw(shader);
}

void renderMuskDeer(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0, 0, -10));
    model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    muskDeer.Draw(shader);
}

void renderMuskDeerCage(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0, 0, -10));
    model = glm::scale(model, glm::vec3(0.1, 0.05, 0.1));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    muskDeerCage.Draw(shader);
}

void renderSheep(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(10, 0, -10));
    model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    sheep.Draw(shader);
}

void renderSheepCage(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(10, 0, -10));
    model = glm::scale(model, glm::vec3(0.1, 0.05, 0.1));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    sheepCage.Draw(shader);
}

void renderOtter(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-5, 0, 5));
    model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    otter.Draw(shader);
}

void renderOtterCage(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-5, 0, 5));
    model = glm::scale(model, glm::vec3(0.1, 0.05, 0.1));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    otterCage.Draw(shader);
}

void renderMonkey(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0, 0, 10));
    model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    monkey.Draw(shader);
}

void renderMonkeyCage(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0, 0, 10));
    model = glm::scale(model, glm::vec3(0.1, 0.05, 0.1));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    monkeyCage.Draw(shader);
}

void renderKoala(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(10, 0, 10));
    model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    koala.Draw(shader);
}

void renderKoalaCage(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(10, 0, 10));
    model = glm::scale(model, glm::vec3(0.1, 0.05, 0.1));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    koalaCage.Draw(shader);
}

void renderToucan(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(10, 0, -4));
    model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    toucan.Draw(shader);
}

void renderToucanCage(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(10, 0, -4));
    model = glm::scale(model, glm::vec3(0.1, 0.05, 0.1));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    toucanCage.Draw(shader);
}

void renderTurkey(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(5, 0, -2));
    model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    turkey.Draw(shader);
}

void renderTurkeyCage(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(5, 0, -2));
    model = glm::scale(model, glm::vec3(0.1, 0.05, 0.1));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    turkeyCage.Draw(shader);
}

void renderPenguin(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(10, 0, 0));
    model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    penguin.Draw(shader);
}

void renderPenguinCage(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(10, 0, 0));
    model = glm::scale(model, glm::vec3(0.1, 0.05, 0.1));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    penguinCage.Draw(shader);
}

void renderTurtle(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(5, 0, 2));
    model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    turtle.Draw(shader);
}

void renderTurtleCage(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(5, 0, 2));
    model = glm::scale(model, glm::vec3(0.1, 0.05, 0.1));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    turtleCage.Draw(shader);
}

void renderSeaTurtle(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(10, 0, 4));
    model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    seaTurtle.Draw(shader);
}

void renderSeaTurtleCage(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(10, 0, 4));
    model = glm::scale(model, glm::vec3(0.1, 0.05, 0.1));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    seaTurtleCage.Draw(shader);
}

void renderBison(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-10, 0, 0));
    model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    bison.Draw(shader);
}

void renderBisonCage(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-10, 0, 0));
    model = glm::scale(model, glm::vec3(0.1, 0.05, 0.1));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    bisonCage.Draw(shader);
}

void renderHyena(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-5, 0, -2));
    model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    hyena.Draw(shader);
}

void renderHyenaCage(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-5, 0, -2));
    model = glm::scale(model, glm::vec3(0.1, 0.05, 0.1));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    hyenaCage.Draw(shader);
}

void renderWolf(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-10, 0, 4));
    model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    wolf.Draw(shader);
}

void renderWolfCage(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-10, 0, 4));
    model = glm::scale(model, glm::vec3(0.1, 0.05, 0.1));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    wolfCage.Draw(shader);
}

void renderFox(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-5, 0, 2));
    model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    fox.Draw(shader);
}

void renderFoxCage(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-5, 0, 2));
    model = glm::scale(model, glm::vec3(0.1, 0.05, 0.1));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    foxCage.Draw(shader);
}

void renderGround(gps::Shader shader) {
    shader.useShaderProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    ground.Draw(shader);
}

void renderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//render the scene

	// render the teapot
	renderTeapot(myBasicShader);
    renderBear(myBasicShader);
    renderBearCage(myBasicShader);
    renderIbex(myBasicShader);
    renderIbexCage(myBasicShader);
    renderMoose(myBasicShader);
    renderMooseCage(myBasicShader);
    renderMuskDeer(myBasicShader);
    renderMuskDeerCage(myBasicShader);
    renderSheep(myBasicShader);
    renderSheepCage(myBasicShader);
    renderDeer(myBasicShader);
    renderDeerCage(myBasicShader);
    renderOtter(myBasicShader);
    renderOtterCage(myBasicShader);
    renderMonkey(myBasicShader);
    renderMonkeyCage(myBasicShader);
    renderKoala(myBasicShader);
    renderKoalaCage(myBasicShader);
    renderToucan(myBasicShader);
    renderToucanCage(myBasicShader);
    renderTurkey(myBasicShader);
    renderTurkeyCage(myBasicShader);
    renderPenguin(myBasicShader);
    renderPenguinCage(myBasicShader);
    renderTurtle(myBasicShader);
    renderTurtleCage(myBasicShader);
    renderSeaTurtle(myBasicShader);
    renderSeaTurtleCage(myBasicShader);
    renderBison(myBasicShader);
    renderBisonCage(myBasicShader);
    renderHyena(myBasicShader);
    renderHyenaCage(myBasicShader);
    renderWolf(myBasicShader);
    renderWolfCage(myBasicShader);
    renderFox(myBasicShader);
    renderFoxCage(myBasicShader);
    renderGround(myBasicShader);

    mySkyBox.Draw(skyboxShader, view, projection);

}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char * argv[]) {
    faces.push_back("skybox/posx.jpg");
    faces.push_back("skybox/negx.jpg");
    faces.push_back("skybox/posy.jpg");
    faces.push_back("skybox/negy.jpg");
    faces.push_back("skybox/posz.jpg");
    faces.push_back("skybox/negz.jpg");
    try {
        initOpenGLWindow();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
	initModels();
	initShaders();
	initUniforms();
    setWindowCallbacks();
    mySkyBox.Load(faces);
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();
    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
        glm::value_ptr(view));

    projection = glm::perspective(glm::radians(45.0f), (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height, 0.1f, 1000.0f);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
        glm::value_ptr(projection));

	glCheckError();

	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
	    renderScene();

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}
