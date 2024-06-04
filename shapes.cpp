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

Triangle::Triangle(vec3 point1, vec3 point2, vec3 point3, vec3 color)
{
    numShapes = 0;
    shapes = new Shape *[0];

    int n = numPoints();
    vertices = new vec3 *[n];
    vertices[0] = new vec3(point1);
    vertices[1] = new vec3(point2);
    vertices[2] = new vec3(point3);

    colors = new vec3[n];
    for (int i = 0; i < 3; i++)
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
    return 9;
}

int Triangle::numPoints()
{
    return 3;
}

Rectangle::Rectangle(vec3 ul, vec3 ur, vec3 ll, vec3 lr, vec3 color)
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

    colors = new vec3[n];

    for (int i = 0; i < n; i++)
    {
        colors[i] = color;
    }
}

House::House()
{
    numShapes = 3;
    shapes = new Shape *[numShapes];
    shapes[0] = new Triangle(
        vec3(0, 0.4, 0),
        vec3(-0.2, 0.2, 0),
        vec3(0.2, 0.2, 0));
    shapes[1] = new Rectangle(
        vec3(-0.2, 0.2, 0.5),
        vec3(0.2, 0.2, 0.5),
        vec3(-0.2, -0.2, 0.5),
        vec3(0.2, -0.2, 0.5));
    shapes[2] = new Rectangle(
        vec3(-0.05, 0, 0),
        vec3(0.05, 0, 0),
        vec3(-0.05, -0.2, 0),
        vec3(0.05, -0.2, 0),
        vec3(0, 0, 1));
}

Box::Box(vec3 center, double height, double width, double length, vec3 color)
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

    Rectangle *front = new Rectangle(nearLeftTop, nearRightTop, nearLeftBottom, nearRightBottom, color);
    Rectangle *back = new Rectangle(farRightTop, farLeftTop, farRightBottom, farLeftBottom, color);
    Rectangle *left = new Rectangle(farLeftTop, nearLeftTop, farLeftBottom, nearLeftBottom, color);
    Rectangle *right = new Rectangle(nearRightTop, farRightTop, nearRightBottom, farRightBottom, color);
    Rectangle *bottom = new Rectangle(nearLeftBottom, nearRightBottom, farLeftBottom, farRightBottom, color);
    Rectangle *top = new Rectangle(nearLeftTop, nearRightTop, farLeftTop, farRightTop, color);

    numShapes = 6;
    shapes = new Shape *[numShapes]
    {
        front, back, left, right, bottom, top
    };
}

Boxes::Boxes(int numBoxes, vec3 *centers, double *heights, double *widths, double *lengths, vec3 *colors)
{
    numShapes = numBoxes;
    shapes = new Shape*[numShapes];
    for(int i=0; i < numShapes; i++){
        shapes[i] = new Box(centers[i],heights[i], widths[i], lengths[i], colors[i]);
    }
}
float outWidth = 1;
NorthWall::NorthWall(vec3 center)
{
    numShapes = 20;
    shapes = new Shape *[numShapes];

    float totalHeight = 11;
    float totalLength = 35;
    float largeHeight = totalHeight / 4;
    float smallHeight = totalHeight / 8;
    float pillarHeight = totalHeight;
    float pillarWidth = 0.4;

    // Middle piece
    shapes[2] = new Box(center, largeHeight, outWidth, totalLength, vec3(0.9, 0.9, 0.9));

    // Top pieces
    shapes[0] = new Box(vec3(center.x, center.y + largeHeight / 2 - smallHeight / 2, center.z), smallHeight, 0.01, totalLength, vec3(0, 0.6, 1));
    shapes[1] = new Box(vec3(center.x, center.y + largeHeight - smallHeight, center.z), largeHeight, outWidth, totalLength, vec3(0.4, 0.4, 0.4));

    // Bottom pieces
    shapes[3] = new Box(vec3(center.x, center.y - largeHeight / 2 + smallHeight / 2, center.z), smallHeight, 0.01, totalLength, vec3(0, 0.6, 1));
    shapes[4] = new Box(vec3(center.x, center.y - largeHeight + smallHeight, center.z), largeHeight, outWidth, totalLength, vec3(0.9, 0.9, 0.9));

    // Pillars
    float pillarSpacing = totalLength / 50;
    for (int i = 0; i < 15; i++) {
        float pillarZ = center.z - totalLength / 6 + pillarSpacing * (i + 1);
        shapes[5 + i] = new Box(vec3(center.x, center.y, pillarZ), pillarHeight, outWidth - 0.5, pillarWidth, vec3(0.8, 0.8, 0.8));
    }
    // shapes[20] = new WoodenDecoration(center);
}

EastWall::EastWall(vec3 center)
{
    numShapes = 5;
    shapes = new Shape *[numShapes];
    float totalWidth = 8;
    float middleWidth = totalWidth / 3;
    float sideWidth = totalWidth / 6;

    // Middle wall
    shapes[2] = new Box(center, 11, middleWidth, outWidth, vec3(0.4, 0.4, 0.4));

    // Left walls
    shapes[0] = new Box(vec3(center.x - middleWidth / 2 + sideWidth / 2, center.y, center.z), 11, sideWidth, 0.01, vec3(0, 0.3, 1));
    shapes[1] = new Box(vec3(center.x - middleWidth / 2 + sideWidth / 6, center.y, center.z), 11, sideWidth, outWidth, vec3(0.4, 0.4, 0.4));

    // Right walls
    shapes[3] = new Box(vec3(center.x + middleWidth / 2 - sideWidth / 2, center.y, center.z), 11, sideWidth, 0.01, vec3(0, 0.3, 1));
    shapes[4] = new Box(vec3(center.x + middleWidth / 2 - sideWidth / 6, center.y, center.z), 11, sideWidth, outWidth, vec3(0.4, 0.4, 0.4));
}

SouthWall::SouthWall(vec3 center)
{
    numShapes = 20;
    shapes = new Shape *[numShapes];

    float totalHeight = 11;
    float totalLength = 35;
    float largeHeight = totalHeight / 4;
    float smallHeight = totalHeight / 8;
    float pillarHeight = totalHeight;
    float pillarWidth = 0.4;

    // Middle piece
    shapes[2] = new Box(center, largeHeight, outWidth, totalLength, vec3(0.9, 0.9, 0.9));

    // Top pieces
    shapes[0] = new Box(vec3(center.x, center.y + largeHeight / 2 - smallHeight / 2, center.z), smallHeight, 0.01, totalLength, vec3(0, 0.6, 1));
    shapes[1] = new Box(vec3(center.x, center.y + largeHeight - smallHeight, center.z), largeHeight, outWidth, totalLength, vec3(0.4, 0.4, 0.4));

    // Bottom pieces
    shapes[3] = new Box(vec3(center.x, center.y - largeHeight / 2 + smallHeight / 2, center.z), smallHeight, 0.01, totalLength, vec3(0, 0.6, 1));
    shapes[4] = new Box(vec3(center.x, center.y - largeHeight + smallHeight, center.z), largeHeight, outWidth, totalLength, vec3(0.9, 0.9, 0.9));

    // Pillars
    float pillarSpacing = totalLength / 50;
    for (int i = 0; i < 15; i++) {
        float pillarZ = center.z - totalLength / 6 + pillarSpacing * (i + 1);
        shapes[5 + i] = new Box(vec3(center.x, center.y, pillarZ), pillarHeight, outWidth - 0.5, pillarWidth, vec3(0.8, 0.8, 0.8));
    }
}

WestWall::WestWall(vec3 center)
{
    numShapes = 3;
    shapes = new Shape *[numShapes];

    float totalWidth = 8;
    float middleWidth = totalWidth / 1.5;
    float sideWidth = totalWidth / 6;

    // Middle wall
    shapes[1] = new Box(center, 11, middleWidth, 0.01, vec3(0, 0.3, 1));

    // Left wall
    shapes[0] = new Box(vec3(center.x - middleWidth / 2 + sideWidth*1.18, center.y, center.z), 11, sideWidth, outWidth, vec3(0.4, 0.4, 0.4));

    // Right wall
    shapes[2] = new Box(vec3(center.x + middleWidth / 2 - sideWidth*1.18, center.y, center.z), 11, sideWidth, outWidth, vec3(0.4, 0.4, 0.4));
}

WoodenDecoration::WoodenDecoration(vec3 center)
{
    numShapes = 15;
    shapes = new Shape *[numShapes];

    float totalLength = 35;
    float baseHeight = 2;
    float prismWidth = 0.2;
    float prismDepth = 0.2;
    vec3 color(0.65, 0.16, 0.16);

    float prismSpacing = 0.01;
    float waveAmplitude = 2;
    float waveFrequency = 2 * M_PI / totalLength;

    for (int i = 0; i < numShapes; i++) {
        float x = center.x - totalLength / 2 + prismSpacing * i;
        float height = baseHeight + waveAmplitude * (1 + std::sin(waveFrequency * x));
        float y = center.y;
        shapes[i] = new Box(vec3(x, y, center.z), height, prismWidth, prismDepth, color);
    }
}
Walls::Walls()
{
    vec3 wallCenters[4] = {
        vec3(1.3, 0, 0),  // North Wall
        vec3(-1.3, 0, 0), // South Wall
        vec3(0, 0, 5.5),  // East Wall
        vec3(0, 0, -5.5)  // West Wall
    };

    numShapes = 4;
    shapes = new Shape *[numShapes];
    shapes[0] = new NorthWall(wallCenters[0]);
    shapes[1] = new SouthWall(wallCenters[1]);
    shapes[2] = new EastWall(wallCenters[2]);
    shapes[3] = new WestWall(wallCenters[3]);
}
