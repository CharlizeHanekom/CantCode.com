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

float outWidth = 1;
NorthWall::NorthWall(vec3 center)
{
    numShapes = 21;
    shapes = new Shape *[numShapes];

    float totalHeight = 11;
    float totalLength = 35;
    float largeHeight = totalHeight / 4;
    float smallHeight = totalHeight / 8;
    float pillarHeight = totalHeight;
    float pillarWidth = 0.4;

    // Middle piece
    shapes[2] = new Box(center, largeHeight, outWidth, totalLength, vec4(0.9, 0.9, 0.9,1));

    // Top pieces
    shapes[0] = new Box(vec3(center.x, center.y + largeHeight / 2 - smallHeight / 2, center.z), smallHeight, 0.01, totalLength, vec4(0, 0.6, 1, 0.1));
    shapes[1] = new Box(vec3(center.x, center.y + largeHeight - smallHeight, center.z), largeHeight, outWidth, totalLength, vec4(0.4, 0.4, 0.4, 1));

    // Bottom pieces
    shapes[3] = new Box(vec3(center.x, center.y - largeHeight / 2 + smallHeight / 2, center.z), smallHeight, 0.01, totalLength, vec4(0, 0.6, 1, 0.1));
    shapes[4] = new Box(vec3(center.x, center.y - largeHeight + smallHeight, center.z), largeHeight, outWidth, totalLength, vec4(0.9, 0.9, 0.9, 1));

    // Pillars
    float pillarSpacing = totalLength / 50;
    for (int i = 0; i < 15; i++) {
        float pillarZ = center.z - totalLength / 6 + pillarSpacing * (i + 1);
        shapes[5 + i] = new Box(vec3(center.x, center.y, pillarZ), pillarHeight, outWidth - 0.5, pillarWidth, vec4(0.8, 0.8, 0.8, 1));
    }
    shapes[20] = new WoodenDecoration(center);
}

EastWall::EastWall(vec3 center)
{
    numShapes = 5;
    shapes = new Shape *[numShapes];
    float totalWidth = 8;
    float middleWidth = totalWidth / 3;
    float sideWidth = totalWidth / 6;

    // Middle wall
    shapes[2] = new Box(center, 11, middleWidth, outWidth, vec4(0.4, 0.4, 0.4,1));

    // Left walls
    shapes[0] = new Box(vec3(center.x - middleWidth / 2 + sideWidth / 2, center.y, center.z), 11, sideWidth, 0.01, vec4(0, 0.3, 1, 0.1));
    shapes[1] = new Box(vec3(center.x - middleWidth / 2 + sideWidth / 6, center.y, center.z), 11, sideWidth, outWidth, vec4(0.4, 0.4, 0.4, 1));

    // Right walls
    shapes[3] = new Box(vec3(center.x + middleWidth / 2 - sideWidth / 2, center.y, center.z), 11, sideWidth, 0.01, vec4(0, 0.3, 1, 0.1));
    shapes[4] = new Box(vec3(center.x + middleWidth / 2 - sideWidth / 6, center.y, center.z), 11, sideWidth, outWidth, vec4(0.4, 0.4, 0.4, 1));
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
    shapes[2] = new Box(center, largeHeight, outWidth, totalLength, vec4(0.9, 0.9, 0.9, 1));

    // Top pieces
    shapes[0] = new Box(vec3(center.x, center.y + largeHeight / 2 - smallHeight / 2, center.z), smallHeight, 0.01, totalLength, vec4(0, 0.6, 1, 0.1));
    shapes[1] = new Box(vec3(center.x, center.y + largeHeight - smallHeight, center.z), largeHeight, outWidth, totalLength, vec4(0.4, 0.4, 0.4, 1));

    // Bottom pieces
    shapes[3] = new Box(vec3(center.x, center.y - largeHeight / 2 + smallHeight / 2, center.z), smallHeight, 0.01, totalLength, vec4(0, 0.6, 1, 0.1));
    shapes[4] = new Box(vec3(center.x, center.y - largeHeight + smallHeight, center.z), largeHeight, outWidth, totalLength, vec4(0.9, 0.9, 0.9, 1));

    // Pillars
    float pillarSpacing = totalLength / 50;
    for (int i = 0; i < 15; i++) {
        float pillarZ = center.z - totalLength / 6 + pillarSpacing * (i + 1);
        shapes[5 + i] = new Box(vec3(center.x, center.y, pillarZ), pillarHeight, outWidth - 0.5, pillarWidth, vec4(0.8, 0.8, 0.8, 1));
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
    shapes[1] = new Box(center, 11, middleWidth, 0.01, vec4(0, 0.3, 1, 0.1));

    // Left wall
    shapes[0] = new Box(vec3(center.x - middleWidth / 2 + sideWidth*1.18, center.y, center.z), 11, sideWidth, outWidth, vec4(0.4, 0.4, 0.4, 1));

    // Right wall
    shapes[2] = new Box(vec3(center.x + middleWidth / 2 - sideWidth*1.18, center.y, center.z), 11, sideWidth, outWidth, vec4(0.4, 0.4, 0.4, 1));
}

WoodenDecoration::WoodenDecoration(vec3 center)
{
    numShapes = 1;
    shapes = new Shape *[numShapes];

    float totalLength = 35;
    float baseHeight = 0.1;
    float prismWidth = 0.2;
    float prismDepth = 0.2;
    vec4 color(0.65, 0.16, 0.16, 1);

    float prismSpacing = 1;
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
    numShapes = 1;
    shapes = new Shape *[numShapes];
    shapes[0] = new Walls();
    // shapes[1] = new Roof();
}