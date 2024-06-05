#ifndef SHAPES_H
#define SHAPES_H

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cmath>

using namespace glm;
using namespace std;


struct Shape{
    vec3** vertices;
    vec4* colors;
    Shape** shapes;
    int numShapes;

    bool coloursInverted = false;

    // Colours
    vec4 frameColor = vec4(0.3, 0.3, 0.3,1);
    vec4 DividerColour = vec4(1, 1, 1, 1);
    vec4 CubicleColour = vec4(0.2, 0.2, 0.2, 1);
    vec4 CubicleSeatColour = vec4(1, 1, 0, 1);
    vec4 CubicleWoodColour = vec4(0.45, 0.25, 0.0, 1);
    vec4 PlantColour = vec4(0, 1, 0, 1);
    vec4 PlantPotColour = vec4(0.5, 0.5, 0.5, 1);
    vec4 ChairColour = vec4(0.5, 0.5, 0.5, 1);
    vec4 SofaColourBlue = vec4(0, 0, 1, 1);
    vec4 TableColour = vec4(1, 1, 1, 1);
    vec4 SofaColourRed = vec4(1, 0, 0, 1);
    vec4 SofaColourYellow = vec4(1, 1, 0, 1);
    vec4 ColourBlack = vec4(0, 0, 0, 1);
    vec4 ColourSilver = vec4(0.75, 0.75, 0.75, 1);
    vec4 DarkGrey = vec4(0.2, 0.2, 0.2, 1);

    ~Shape();

    virtual void applyMatrix(mat4x4);
    virtual GLfloat* toVertexArray();
    virtual GLfloat* toColorArray();
    virtual int numPoints();

    virtual int numVertices();
    virtual int numColors();

    void invertColours();


};

struct Triangle: public Shape{
    Triangle(vec3, vec3, vec3, vec4 = vec4(1.0f, 0.0f, 0.0f, 1.0f));
    int numVertices();
    int numColors();
    int numPoints();
};

struct Rectangle: public Shape{
    Rectangle(vec3, vec3, vec3, vec3, vec4 = vec4(0.0f, 1.0f, 0.0f, 1.0f));
};

struct Box: public Shape{
    Box(vec3 center, double height, double width, double length, vec4 = vec4(1.0f, 0.2f, 0.2f, 1.0f));
};

struct Boxes: public Shape{
    Boxes(int numBoxes, vec3* centers, double* heights, double* widths, double* lengths, vec4* colors);
};

struct NorthWall: public Shape{
    NorthWall(vec3);
};
struct EastWall: public Shape{
    EastWall(vec3);
};
struct SouthWall: public Shape{
    SouthWall(vec3);
};
struct WestWall: public Shape{
    WestWall(vec3);
};
struct WoodenDecoration: public Shape{
    WoodenDecoration(vec3);
};
struct Walls: public Shape{
    Walls();
};
struct Door: public Shape{
    Door(vec3);
};
struct WindowPane: public Shape{
    WindowPane(vec3 center, double height, double width, double length, bool stained);
};

struct Roof: public Shape{
    Roof();
};

struct Scene: public Shape{
    Scene();
};

struct Objects: public Shape{
    Objects();
};

struct Kiosk: public Shape{
    Kiosk();
};

struct Cylinder: public Shape{
    Cylinder(vec3 center, int numSidesOnBase, float height, float radius, vec4 color);
};

#endif /*SHAPES_H*/