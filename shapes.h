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
    vec3* colors;
    Shape** shapes;
    int numShapes;

    ~Shape();

    virtual void applyMatrix(mat4x4);
    virtual GLfloat* toVertexArray();
    virtual GLfloat* toColorArray();
    virtual int numPoints();

    virtual int numVertices();
    virtual int numColors();
    
    void RotateY(float angle);
    void RotateX(float angle);
};

struct Triangle: public Shape{
    Triangle(vec3, vec3, vec3, vec3 = vec3(1.0f, 0.0f, 0.0f));
    int numVertices();
    int numColors();
    int numPoints();
};

struct Rectangle: public Shape{
    Rectangle(vec3, vec3, vec3, vec3, vec3 = vec3(0.0f, 1.0f, 0.0f));
};

struct Box: public Shape{
    Box(vec3 center, double height, double width, double length, vec3 = vec3(1.0f, 0.2f, 0.2f));
};

struct Boxes: public Shape{
    Boxes(int numBoxes, vec3* centers, double* heights, double* widths, double* lengths, vec3* colors);
};

struct House: public Shape{
    House();
};

struct Walls: public Shape{
    Walls();
};

class Cylinder: public Shape{
public:
    Cylinder();
    Cylinder(vec3 center, int numSidesOnBase, float height, float radius, vec3 color = vec3(0.0f, 0.0f, 1.0f));
    void Circles(vec3 center, int numSidesOnBase, float height, float radius, vec3 color = vec3(0.0f, 0.0f, 1.0f));
    
};

struct Objects: public Shape{
    Objects();
};
#endif /*SHAPES_H*/