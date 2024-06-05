#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <chrono>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "shapes.h"
#include "camera.h"

#define timeDT std::chrono::_V2::steady_clock::time_point

using namespace glm;
using namespace std;

const GLuint SCR_WIDTH = 1920, SCR_HEIGHT = 1080;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 2.5f, 0.02f);

const char *getError()
{
    const char *errorDescription;
    glfwGetError(&errorDescription);
    return errorDescription;
}

inline void startUpGLFW()
{
    glewExperimental = true; // Needed for core profile
    if (!glfwInit())
    {
        throw getError();
    }
}

inline void startUpGLEW()
{
    glewExperimental = true; // Needed in core profile
    if (glewInit() != GLEW_OK)
    {
        glfwTerminate();
        throw getError();
    }
}

inline GLFWwindow *setUp()
{
    startUpGLFW();
    glfwWindowHint(GLFW_SAMPLES, 4);               // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL
    GLFWwindow *window;                                            // (In the accompanying source code, this variable is global for simplicity)
    window = glfwCreateWindow(1920, 1080, "CantCode.com", NULL, NULL);
    if (window == NULL)
    {
        cout << getError() << endl;
        glfwTerminate();
        throw "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n";
    }
    glfwMakeContextCurrent(window); // Initialize GLEW
    startUpGLEW();
    return window;
}
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    static double lastX = 0.0f;
    static double lastY = 0.0f;
    static bool firstMouse = true;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.mouseControl(xoffset, yoffset);
}
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.scrollControl(yoffset);
}
int main()
{
    GLFWwindow *window;
    try
    {
        window = setUp();
    }
    catch (const char *e)
    {
        cout << e << endl;
        throw;
    }

    glClearColor(0.529, 0.808, 0.922, 1.0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_NEAREST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Here we create a VAO
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // This is needed for sticky keys
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Here we compile and load the shaders. First we pass the vertex shader then the fragment shader.
    GLuint programID = LoadShaders("vertexShader.glsl", "fragmentShader.glsl");

    timeDT lastChanged = chrono::steady_clock::now();

    // Here we create two VBOs
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);

    double lastTime;
    lastTime = glfwGetTime();
    bool spacePressed = false;
    bool wireframe = false;

    bool rightCTRLPressed = false;
    bool dayTime = true;

    bool tabPressed = false;

    Shape *shp = new Scene();

    do
    {
        

        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;

        // Here we clear the color and depth buffer bits.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(programID);

        // Here we obtain the vertices and colors for the house as two dynamic arrays.
        GLfloat *vertices = shp->toVertexArray();
        GLfloat *colors = shp->toColorArray();

        //  Here we bind the VBOs
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat[shp->numVertices()]), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat[shp->numColors()]), colors, GL_STATIC_DRAW);

        // Here we enable the VAO and populate it.
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);

        // Calculate the projection matrix
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // Get the location of the 'projection' uniform variable in the shader
        GLint projLoc = glGetUniformLocation(programID, "projection");

        // Pass the projection matrix to the shader
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glDrawArrays(GL_TRIANGLES, 0, shp->numPoints()); // Starting from vertex 0; 3 vertices total -> 1 triangle

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        // Here we swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        bool keys[1024];
        GLfloat lastFrame = 0.0f;
        GLfloat currentFrame = glfwGetTime();
        lastFrame = currentFrame;

        // glfwSetCursorPosCallback(window, mouse_callback);
        // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetScrollCallback(window, scroll_callback);
        keys[GLFW_KEY_W] = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
        keys[GLFW_KEY_S] = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
        keys[GLFW_KEY_A] = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
        keys[GLFW_KEY_D] = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
        keys[GLFW_KEY_SPACE] = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        keys[GLFW_KEY_LEFT_CONTROL] = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
        keys[GLFW_KEY_I] = glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS;
        keys[GLFW_KEY_K] = glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS;
        keys[GLFW_KEY_J] = glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS;
        keys[GLFW_KEY_L] = glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS;
        keys[GLFW_KEY_U] = glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS;
        keys[GLFW_KEY_O] = glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS;
        keys[GLFW_KEY_LEFT_SHIFT] = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && !spacePressed)
        {
            if (wireframe)
            {
                wireframe = false;
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            else
            {
                wireframe = true;
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            spacePressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE)
        {
            spacePressed = false;
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS && !rightCTRLPressed)
        {
            if (dayTime)
            {
                glClearColor(0.529, 0.808, 0.922, 1.0);
            }
            else
            {
                glClearColor(0.0, 0.0, 0.0, 0);
            }
            dayTime = !dayTime;
            rightCTRLPressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_RELEASE)
        {
            rightCTRLPressed = false;
        }

        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !tabPressed)
        {
            shp->invertColours();
            tabPressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
        {
            tabPressed = false;
        }

        camera.keyControl(keys, deltaTime);
        // Calculate the view matrix
        glm::mat4 view = camera.calculateViewMatrix();

        // Get the location of the 'view' uniform variable in the shade
        GLint viewLoc = glGetUniformLocation(programID, "view");

        // Pass the view matrix to the shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // Reminder: The examples use GLM but for the practicals you may not use GLM and all the matrix calculations needs to be done in the application not the shaders.

        delete[] vertices;
        delete[] colors;

        lastTime = currentTime;
        cout << "FPS: " << 1 / deltaTime << endl;
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0);

    delete shp;
}
