#ifndef SHAPES_H
#define SHAPES_H

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

using namespace glm;
using namespace std;


struct Shape{
    vec3** vertices;
    vec4* colors;
    Shape** shapes;
    int numShapes;

    ~Shape();

    virtual void applyMatrix(mat4x4);
    virtual GLfloat* toVertexArray();
    virtual GLfloat* toColorArray();
    virtual int numPoints();

    virtual int numVertices();
    virtual int numColors();
};

struct Triangle: public Shape{
    Triangle(vec3, vec3, vec3, vec4 = vec4(1.0f, 0.0f, 0.0f, 0.5f));
    int numVertices();
    int numColors();
    int numPoints();
};

struct Rectangle: public Shape{
    Rectangle(vec3, vec3, vec3, vec3, vec4 = vec4(0.0f, 1.0f, 0.0f,0.5f));
};

struct Box: public Shape{
    Box(vec3 center, double height, double width, double length, vec4 = vec4(1.0f, 0.2f, 0.2f, 1.0f));
};

// struct Boxes: public Shape{
//     Boxes(int numBoxes, vec3* centers, double* heights, double* widths, double* lengths, vec3* colors);
// };

struct Roof: public Shape{
    Roof();
};

struct WindowPanel: public Shape{
    WindowPanel(vec3 center, double height, double width,vec3* colors);
};


#endif /*SHAPES_H*/