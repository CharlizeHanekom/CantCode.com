#include "shapes.h"

Shape::~Shape()
{
}

int Shape::numPoints()
{
    int count = 0;
    for (int i = 0; i < numShapes; i++)
    {
        count += shapes[i]->numPoints();
    }
    return count;
}

GLfloat *Shape::toVertexArray()
{
    int n = numVertices();
    GLfloat *result = new GLfloat[numVertices()];
    if (numShapes > 0)
    {
        int count = 0;
        for (int i = 0; i < numShapes; i++)
        {
            GLfloat *temp = shapes[i]->toVertexArray();
            for (int j = 0; j < shapes[i]->numVertices(); j++)
            {
                result[count++] = temp[j];
            }
            delete[] temp;
        }
    }
    else
    {
        int count = 0;
        for (int i = 0; i < numPoints(); i++)
        {
            result[count++] = (*vertices[i])[0];
            result[count++] = (*vertices[i])[1];
            result[count++] = (*vertices[i])[2];
        }
    }

    return result;
}

GLfloat *Shape::toColorArray()
{
    GLfloat *result = new GLfloat[numColors()];
    if (numShapes > 0)
    {
        int count = 0;
        for (int i = 0; i < numShapes; i++)
        {
            GLfloat *temp = shapes[i]->toColorArray();
            for (int j = 0; j < shapes[i]->numColors(); j++)
            {
                result[count++] = temp[j];
            }
            delete[] temp;
        }
    }
    else
    {
        int count = 0;
        for (int i = 0; i < numPoints(); i++)
        {
            result[count++] = colors[i][0];
            result[count++] = colors[i][1];
            result[count++] = colors[i][2];
            result[count++] = colors[i][3];
        }
    }
    return result;
}

void Shape::applyMatrix(mat4x4 m)
{
    if (numShapes > 0)
    {
        for (int i = 0; i < numShapes; i++)
        {
            shapes[i]->applyMatrix(m);
        }
    }
    else
    {
        for (int i = 0; i < numPoints(); i++)
        {
            (*vertices[i]) = m * vec4((*vertices[i]), 1);
        }
    }
}

int Shape::numVertices()
{
    int count = 0;
    for (int i = 0; i < numShapes; i++)
    {
        count += shapes[i]->numVertices();
    }
    return count;
}

int Shape::numColors()
{
    int count = 0;
    for (int i = 0; i < numShapes; i++)
    {
        count += shapes[i]->numColors();
    }
    return count;
}

Triangle::Triangle(vec3 point1, vec3 point2, vec3 point3, vec4 color)
{
    numShapes = 0;
    shapes = new Shape *[0];

    int n = numPoints();
    vertices = new vec3 *[n];
    vertices[0] = new vec3(point1);
    vertices[1] = new vec3(point2);
    vertices[2] = new vec3(point3);

    colors = new vec4[n];
    for (int i = 0; i < n; i++)
    {
        colors[i] = color;
    }
}

int Triangle::numVertices()
{
    return 9;
}

int Triangle::numColors()
{
    return 12;
}

int Triangle::numPoints()
{
    return 3;
}

Rectangle::Rectangle(vec3 ul, vec3 ur, vec3 ll, vec3 lr, vec4 color)
{
    numShapes = 2;
    shapes = new Shape *[numShapes];
    shapes[0] = new Triangle(ur, ul, ll, color);
    shapes[1] = new Triangle(ll, lr, ur, color);
    int n = numPoints();
    vertices = new vec3 *[n];
    int count = 0;
    for (int i = 0; i < numShapes; i++)
    {
        for (int j = 0; j < shapes[i]->numPoints(); j++)
        {
            vertices[count++] = shapes[i]->vertices[j];
        }
    }

    colors = new vec4[n];

    for (int i = 0; i < n; i++)
    {
        colors[i] = color;
    }
}

Box::Box(vec3 center, double height, double width, double length, vec4 color)
{
    vec3 topPlane = center + vec3(0, height / 2, 0);
    vec3 bottomPlane = center - vec3(0, height / 2, 0);
    vec3 leftPlane = center - vec3(width / 2, 0, 0);
    vec3 rightPlane = center + vec3(width / 2, 0, 0);
    vec3 nearPlane = center - vec3(0, 0, length / 2);
    vec3 farPlane = center + vec3(0, 0, length / 2);

    vec3 nearLeftTop = nearPlane + leftPlane + topPlane;
    vec3 nearRightTop = nearPlane + rightPlane + topPlane;
    vec3 nearLeftBottom = nearPlane + leftPlane + bottomPlane;
    vec3 nearRightBottom = nearPlane + rightPlane + bottomPlane;

    vec3 farLeftTop = farPlane + leftPlane + topPlane;
    vec3 farRightTop = farPlane + rightPlane + topPlane;
    vec3 farLeftBottom = farPlane + leftPlane + bottomPlane;
    vec3 farRightBottom = farPlane + rightPlane + bottomPlane;

    Rectangle *front = new Rectangle(nearLeftTop, nearRightTop, nearLeftBottom, nearRightBottom, color*vec4(0.9f, 0.9f, 0.9f, 1.0f));
    Rectangle *back = new Rectangle(farRightTop, farLeftTop, farRightBottom, farLeftBottom, color*vec4(0.6f, 0.6f, 0.6f, 1.0f));
    Rectangle *left = new Rectangle(farLeftTop, nearLeftTop, farLeftBottom, nearLeftBottom, color*vec4(0.7f, 0.7f, 0.7f, 1.0f));
    Rectangle *right = new Rectangle(nearRightTop, farRightTop, nearRightBottom, farRightBottom, color*vec4(0.8f, 0.8f, 0.8f, 1.0f));
    Rectangle *bottom = new Rectangle(nearLeftBottom, nearRightBottom, farLeftBottom, farRightBottom, color*vec4(1.0f, 1.0f, 1.0f, 1.0f));
    Rectangle *top = new Rectangle(nearLeftTop, nearRightTop, farLeftTop, farRightTop, color*vec4(1.0f, 1.0f, 1.0, 1.0f));

    numShapes = 6;
    shapes = new Shape *[numShapes]
    {
        front, back, left, right, bottom, top
    };
}

Walls::Walls()
{
    vec3 wallCenters[4] = {
        vec3(0, 0, 5.5),
        vec3(0, 0, -5.5),
        vec3(1.3, 0, 0),
        vec3(-1.3, 0, 0)};
    double wallHeights[4] = {11, 11, 11, 11};
    double wallLengths[4] = {0.01, 0.01, 35, 35};
    double wallWidths[4] = {11, 11, 0.01, 0.01};
    vec4 wallColors[4] = {
        vec4(1, 0, 0, 1), // Red
        vec4(0, 1, 0, 1), // Green
        vec4(0, 0, 1, 1), // Blue
        vec4(1, 1, 0, 1)  // Yellow
    };
    numShapes = 4;
    shapes = new Shape *[numShapes];
    for (int i = 0; i < numShapes; i++)
    {
        shapes[i] = new Box(wallCenters[i], wallHeights[i], wallWidths[i], wallLengths[i], wallColors[i]);
    }
}

WindowPane::WindowPane(vec3 center, double height, double width, double length, bool stained)
{
    numShapes = 5;
    shapes = new Shape *[numShapes];
    if (!stained) {
        shapes[0] = new Box(center, height, width, length, vec4(1, 1, 1, 0.3));
    } else {
        shapes[0] = new Box(center,height, width, length, vec4(1, 1, 0, 0.5));
    }

    vec3 cR = center + vec3(0.8, 0.0, 0.00);
    vec3 cL = center + vec3(-0.8, 0.0, 0.00);
    vec3 cN = center + vec3(0.0, 0.0, 0.25);
    vec3 cF = center + vec3(0.0, 0.0, -0.25);

    //Frame
    shapes[1] = new Box(cR, 0.02, 0.02, length+0.04, vec4(0.3, 0.3, 0.3, 1));
    shapes[2] = new Box(cL, 0.02, 0.02, length+0.04, vec4(0.3, 0.3, 0.3, 1));
    shapes[3] = new Box(cN, 0.02, width, 0.02, vec4(0.3, 0.3, 0.3, 1));
    shapes[4] = new Box(cF, 0.02, width, 0.02, vec4(0.3, 0.3, 0.3, 1));
}

Roof::Roof()
{
    numShapes = 4;
    shapes = new Shape *[numShapes];
    
    double height = 0.2;
    double width = 1.6;
    double length = 0.51;

    vec3 centers[4] = {
        vec3(0, 0, 0),
        vec3(-0.5, 0, 0),
        vec3(-1.0, 0, 0),
        vec3(-1.5, 0, 0)};

    vec4 colors[2] = {
        vec4(0, 0, 1, 1),
        vec4(1, 0, 0, 1)};

    shapes[0] = new WindowPane(centers[0], height, width, length, true);
    shapes[1] = new WindowPane(centers[1], height, width, length, true);
    shapes[2] = new WindowPane(centers[2], height, width, length, false);
    shapes[3] = new WindowPane(centers[3], height, width, length, false);
}

Scene::Scene() 
{
    numShapes = 2;
    shapes = new Shape *[numShapes];
    shapes[0] = new Walls();
    shapes[1] = new Roof();
}