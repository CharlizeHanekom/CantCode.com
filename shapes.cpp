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

    Rectangle *front = new Rectangle(nearLeftTop, nearRightTop, nearLeftBottom, nearRightBottom, color * 0.1f);
    Rectangle *back = new Rectangle(farRightTop, farLeftTop, farRightBottom, farLeftBottom, color * 0.2f);
    Rectangle *left = new Rectangle(farLeftTop, nearLeftTop, farLeftBottom, nearLeftBottom, color * 0.3f);
    Rectangle *right = new Rectangle(nearRightTop, farRightTop, nearRightBottom, farRightBottom, color * 0.4f);
    Rectangle *bottom = new Rectangle(nearLeftBottom, nearRightBottom, farLeftBottom, farRightBottom, color * 0.5f);
    Rectangle *top = new Rectangle(nearLeftTop, nearRightTop, farLeftTop, farRightTop, color * 0.6f);

    numShapes = 6;
    shapes = new Shape *[numShapes]
    {
        front, back, left, right, bottom, top
    };
}

Boxes::Boxes(int numBoxes, vec3 *centers, double *heights, double *widths, double *lengths, vec3 *colors)
{
    numShapes = numBoxes;
    shapes = new Shape *[numShapes];
    for (int i = 0; i < numShapes; i++)
    {
        shapes[i] = new Box(centers[i], heights[i], widths[i], lengths[i], colors[i]);
    }
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
    vec3 wallColors[4] = {
        vec3(1, 0, 0), // Red
        vec3(0, 1, 0), // Green
        vec3(0, 0, 1), // Blue
        vec3(1, 1, 0)  // Yellow
    };
    numShapes = 4;
    shapes = new Shape *[numShapes];
    for (int i = 0; i < numShapes; i++)
    {
        shapes[i] = new Box(wallCenters[i], wallHeights[i], wallWidths[i], wallLengths[i], wallColors[i]);
    }
}

Cylinder::Cylinder()
{
}

Cylinder::Cylinder(vec3 center, int numSidesOnBase, float height, float radius, vec3 color)
{
    // Calculate the angle between each side of the base
    float angle = 2 * M_PI / numSidesOnBase;

    // Create an array to store the vertices of the cylinder
    vec3 *vertices = new vec3[2 * numSidesOnBase];

    // Calculate the vertices for the top base
    for (int i = 0; i < numSidesOnBase; i++)
    {
        float x = center.x + radius * cos(i * angle);
        float y = center.y + radius * sin(i * angle);
        float z = center.z + height / 2;
        vertices[i] = vec3(x, y, z);
    }

    // Calculate the vertices for the bottom base
    for (int i = 0; i < numSidesOnBase; i++)
    {
        float x = center.x + radius * cos(i * angle);
        float y = center.y + radius * sin(i * angle);
        float z = center.z - height / 2;
        vertices[numSidesOnBase + i] = vec3(x, y, z);
    }

    // Create the triangles for the sides of the cylinder
    numShapes = 2 * numSidesOnBase;
    shapes = new Shape *[numShapes];
    for (int i = 0; i < numSidesOnBase; i++)
    {
        int nextIndex = (i + 1) % numSidesOnBase;
        shapes[i] = new Triangle(vertices[i], vertices[nextIndex], vertices[numSidesOnBase + i], color);
        shapes[numSidesOnBase + i] = new Triangle(vertices[nextIndex], vertices[numSidesOnBase + nextIndex], vertices[numSidesOnBase + i], color);
    }
}

void Cylinder::Circles(vec3 center, int numSidesOnBase, float height, float radius, vec3 color)
{
    // -------------------------------------------------- //
    // Drawing the Back face of the cylinder
    // -------------------------------------------------- //

    numShapes = numSidesOnBase * 2;
    shapes = new Shape *[numShapes];

    int numberOfVertices = numSidesOnBase + 2;

    GLfloat twicePi = 2.0f * M_PI;

    GLfloat backWheel[(numberOfVertices) * 3];

    backWheel[0] = center.x;
    backWheel[1] = center.y;
    backWheel[2] = (center.z - (height / 2));

    GLfloat frontWheel[(numberOfVertices) * 3];

    frontWheel[0] = center.x;
    frontWheel[1] = center.y;
    frontWheel[2] = (center.z + (height / 2));

    for (int i = 1; i < numberOfVertices; i++)
    {
        float angle = (i * twicePi / numSidesOnBase);
        float cosAngle = cos(angle);
        float sinAngle = sin(angle);

        backWheel[i * 3] = center.x + (radius * cosAngle);
        backWheel[(i * 3) + 1] = center.y + (radius * sinAngle);
        backWheel[(i * 3) + 2] = backWheel[2];

        frontWheel[i * 3] = center.x + (radius * cosAngle);
        frontWheel[(i * 3) + 1] = center.y + (radius * sinAngle);
        frontWheel[(i * 3) + 2] = frontWheel[2];
    }

    for (int i = 0; i < numSidesOnBase; i++)
    {
        shapes[i] = new Triangle(vec3(backWheel[0], backWheel[1], backWheel[2]), vec3(backWheel[(i + 1) * 3], backWheel[((i + 1) * 3) + 1], backWheel[((i + 1) * 3) + 2]), vec3(backWheel[(i + 2) * 3], backWheel[((i + 2) * 3) + 1], backWheel[((i + 2) * 3) + 2]), color);
    }
    for (int i = numSidesOnBase; i < (numSidesOnBase * 2); i++)
    {
        shapes[i] = new Triangle(vec3(frontWheel[0], frontWheel[1], frontWheel[2]), vec3(frontWheel[(i - numSidesOnBase + 1) * 3], frontWheel[((i - numSidesOnBase + 1) * 3) + 1], frontWheel[((i - numSidesOnBase + 1) * 3) + 2]), vec3(frontWheel[(i - numSidesOnBase + 2) * 3], frontWheel[((i - numSidesOnBase + 2) * 3) + 1], frontWheel[((i - numSidesOnBase + 2) * 3) + 2]), color);
    }
}

void Shape::RotateY(float angle)
{
    mat4x4 rotationMatrix = mat4x4(
        vec4(cos(angle), 0, sin(angle), 0),
        vec4(0, 1, 0, 0),
        vec4(-sin(angle), 0, cos(angle), 0),
        vec4(0, 0, 0, 1));
    applyMatrix(rotationMatrix);
}

void Shape::RotateX(float angle)
{
    mat4x4 rotationMatrix = mat4x4(
        vec4(1, 0, 0, 0),
        vec4(0, cos(angle), -sin(angle), 0),
        vec4(0, sin(angle), cos(angle), 0),
        vec4(0, 0, 0, 1));
    applyMatrix(rotationMatrix);
}

Objects::Objects()
{
    numShapes = 98;
    shapes = new Shape *[numShapes];

    vec3 wallCenters[4] = {
        vec3(0, 0, 5.5),
        vec3(0, 0, -5.5),
        vec3(1.3, 0, 0),
        vec3(-1.3, 0, 0)};
    double wallHeights[4] = {11, 11, 11, 11};
    double wallLengths[4] = {0.01, 0.01, 35, 35};
    double wallWidths[4] = {11, 11, 0.01, 0.01};
    vec3 wallColors[4] = {
        vec3(1, 0, 0), // Red
        vec3(0, 1, 0), // Green
        vec3(0, 0, 1), // Blue
        vec3(1, 1, 0)  // Yellow
    };

    for (int i = 0; i < 4; i++)
    {
        shapes[i] = new Box(wallCenters[i], wallHeights[i], wallWidths[i], wallLengths[i], wallColors[i]);
    }

    /**
     * The following objects are the dividers
     */
    vec3 DividerColour = vec3(1, 1, 1);
    // middle 2
    shapes[4] = new Box(vec3(0, -1.5, -1), 2, 2.5, 0.05, DividerColour);
    shapes[5] = new Box(vec3(0, -1.5, 1), 2, 2.5, 0.05, DividerColour);

    // far left
    shapes[6] = new Box(vec3(-0.5, -1.5, -4), 2, 1.75, 0.05, DividerColour);

    // far right
    shapes[7] = new Box(vec3(0, -1.5, -4.5), 2, 0.05, 1.75, DividerColour);
    shapes[8] = new Box(vec3(0, -1.5, -3.5), 2, 0.05, 1.75, DividerColour);

    // Near left
    shapes[9] = new Box(vec3(-0.2, -1.5, 2.5), 2, 0.05, 1.75, DividerColour);

    /**
     * The following objects is the set of 4 cubicles in the near left corner
     */
    vec3 CubicleColour = vec3(0.2, 0.2, 0.2);
    vec3 CubicleSeatColour = vec3(1, 1, 0);
    vec CubicleWoodColour = vec3(0.45, 0.25, 0.0);
    float startingPosX = -0.23;
    float startingPosZ = 2.5;
    shapes[10] = new Box(vec3(startingPosX, -1.6, startingPosZ), 1.5, 0.05, 1.2, CubicleColour);
    shapes[11] = new Box(vec3(startingPosX - 0.3, -1.6, startingPosZ), 1.5, 0.05, 1.2, CubicleColour);
    shapes[12] = new Box(vec3(startingPosX - 0.325, -1.6, startingPosZ), 1.5, 0.05, 1.2, CubicleColour);
    shapes[13] = new Box(vec3(startingPosX - 0.325 - 0.3, -1.6, startingPosZ), 1.5, 0.05, 1.2, CubicleColour);
    shapes[14] = new Box(vec3(startingPosX - 0.325 - 0.15, -1.6, startingPosZ), 1.5, 0.9, 0.05, CubicleColour);
    shapes[15] = new Box(vec3(startingPosX - 0.15, -1.6, startingPosZ), 1.5, 0.9, 0.05, CubicleColour);

    shapes[16] = new Box(vec3(startingPosX - 0.1, -1.725, startingPosZ), 0.75, 0.6, 0.15, CubicleSeatColour);
    shapes[17] = new Box(vec3(startingPosX - 0.1, -1.75, startingPosZ), 0.3, 0.6, 1.19, CubicleSeatColour);
    shapes[18] = new Box(vec3(startingPosX - 0.2475, -1.75, startingPosZ), 0.3, 0.29, 1.19, CubicleWoodColour);

    shapes[19] = new Box(vec3(startingPosX - 0.1 - 0.325, -1.725, startingPosZ), 0.75, 0.6, 0.15, CubicleSeatColour);
    shapes[20] = new Box(vec3(startingPosX - 0.1 - 0.325, -1.75, startingPosZ), 0.3, 0.6, 1.19, CubicleSeatColour);
    shapes[21] = new Box(vec3(startingPosX - 0.2475 - 0.325, -1.75, startingPosZ), 0.3, 0.29, 1.19, CubicleWoodColour);

    /**
     * The following objects is the set of 4 cubicles in the near left corner
     */
    startingPosX = 0.65;
    startingPosZ = -4.5;
    shapes[22] = new Box(vec3(startingPosX, -1.6, startingPosZ), 1.5, 0.05, 1.2, CubicleColour);
    shapes[23] = new Box(vec3(startingPosX - 0.3, -1.6, startingPosZ), 1.5, 0.05, 1.2, CubicleColour);
    shapes[24] = new Box(vec3(startingPosX - 0.325, -1.6, startingPosZ), 1.5, 0.05, 1.2, CubicleColour);
    shapes[25] = new Box(vec3(startingPosX - 0.325 - 0.3, -1.6, startingPosZ), 1.5, 0.05, 1.2, CubicleColour);
    shapes[26] = new Box(vec3(startingPosX - 0.325 - 0.15, -1.6, startingPosZ), 1.5, 0.9, 0.05, CubicleColour);
    shapes[27] = new Box(vec3(startingPosX - 0.15, -1.6, startingPosZ), 1.5, 0.9, 0.05, CubicleColour);

    shapes[28] = new Box(vec3(startingPosX - 0.1, -1.725, startingPosZ), 0.75, 0.6, 0.15, CubicleSeatColour);
    shapes[29] = new Box(vec3(startingPosX - 0.1, -1.75, startingPosZ), 0.3, 0.6, 1.19, CubicleSeatColour);
    shapes[30] = new Box(vec3(startingPosX - 0.2475, -1.75, startingPosZ), 0.3, 0.29, 1.19, CubicleWoodColour);

    shapes[31] = new Box(vec3(startingPosX - 0.1 - 0.325, -1.725, startingPosZ), 0.75, 0.6, 0.15, CubicleSeatColour);
    shapes[32] = new Box(vec3(startingPosX - 0.1 - 0.325, -1.75, startingPosZ), 0.3, 0.6, 1.19, CubicleSeatColour);
    shapes[33] = new Box(vec3(startingPosX - 0.2475 - 0.325, -1.75, startingPosZ), 0.3, 0.29, 1.19, CubicleWoodColour);

    /**
     * The following objects is the plant
     */
    vec3 PlantColour = vec3(0, 1, 0);
    vec3 PlantPotColour = vec3(0.5, 0.5, 0.5);
    startingPosX = -0.15;
    startingPosZ = 2.7;
    shapes[34] = new Box(vec3(startingPosX, -1.725, startingPosZ), 0.75, 0.25, 0.5, PlantPotColour);

    shapes[35] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ - 0.06), 0.75, 0.05, 0.05, PlantColour);
    shapes[36] = new Box(vec3(startingPosX - 0.02, -1.6, startingPosZ - 0.05), 0.1, 0.05, 0.05, PlantColour);
    shapes[37] = new Box(vec3(startingPosX, -1.6, startingPosZ - 0.04), 0.4, 0.05, 0.05, PlantColour);
    shapes[38] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ - 0.03), 0.65, 0.05, 0.05, PlantColour);
    shapes[39] = new Box(vec3(startingPosX + 0.01, -1.6, startingPosZ - 0.02), 0.5, 0.05, 0.05, PlantColour);

    shapes[40] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ + 0.06), 0.75, 0.05, 0.05, PlantColour);
    shapes[41] = new Box(vec3(startingPosX - 0.02, -1.6, startingPosZ + 0.05), 0.12, 0.05, 0.05, PlantColour);
    shapes[42] = new Box(vec3(startingPosX, -1.6, startingPosZ + 0.04), 0.5, 0.05, 0.05, PlantColour);
    shapes[43] = new Box(vec3(startingPosX + 0.01, -1.6, startingPosZ + 0.03), 0.3, 0.05, 0.05, PlantColour);
    shapes[44] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ + 0.02), 0.5, 0.05, 0.05, PlantColour);

    /**
     * Another plant
     */
    startingPosX = -0.15;
    startingPosZ = 2.3;
    shapes[45] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ - 0.06), 0.75, 0.05, 0.05, PlantColour);
    shapes[46] = new Box(vec3(startingPosX - 0.02, -1.6, startingPosZ - 0.05), 0.1, 0.05, 0.05, PlantColour);
    shapes[47] = new Box(vec3(startingPosX, -1.6, startingPosZ - 0.04), 0.4, 0.05, 0.05, PlantColour);
    shapes[48] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ - 0.03), 0.65, 0.05, 0.05, PlantColour);
    shapes[49] = new Box(vec3(startingPosX + 0.01, -1.6, startingPosZ - 0.02), 0.5, 0.05, 0.05, PlantColour);

    shapes[50] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ + 0.06), 0.75, 0.05, 0.05, PlantColour);
    shapes[51] = new Box(vec3(startingPosX - 0.02, -1.6, startingPosZ + 0.05), 0.12, 0.05, 0.05, PlantColour);
    shapes[52] = new Box(vec3(startingPosX, -1.6, startingPosZ + 0.04), 0.5, 0.05, 0.05, PlantColour);
    shapes[53] = new Box(vec3(startingPosX + 0.01, -1.6, startingPosZ + 0.03), 0.3, 0.05, 0.05, PlantColour);
    shapes[54] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ + 0.02), 0.5, 0.05, 0.05, PlantColour);

    shapes[55] = new Box(vec3(startingPosX, -1.725, startingPosZ), 0.75, 0.25, 0.5, PlantPotColour);

    /**
     * Another plant #2
     */
    startingPosX = 0.05;
    startingPosZ = -3.7;
    shapes[56] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ - 0.06), 0.75, 0.05, 0.05, PlantColour);
    shapes[57] = new Box(vec3(startingPosX - 0.02, -1.6, startingPosZ - 0.05), 0.1, 0.05, 0.05, PlantColour);
    shapes[58] = new Box(vec3(startingPosX, -1.6, startingPosZ - 0.04), 0.4, 0.05, 0.05, PlantColour);
    shapes[59] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ - 0.03), 0.65, 0.05, 0.05, PlantColour);
    shapes[60] = new Box(vec3(startingPosX + 0.01, -1.6, startingPosZ - 0.02), 0.5, 0.05, 0.05, PlantColour);

    shapes[61] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ + 0.06), 0.75, 0.05, 0.05, PlantColour);
    shapes[62] = new Box(vec3(startingPosX - 0.02, -1.6, startingPosZ + 0.05), 0.12, 0.05, 0.05, PlantColour);
    shapes[63] = new Box(vec3(startingPosX, -1.6, startingPosZ + 0.04), 0.5, 0.05, 0.05, PlantColour);
    shapes[64] = new Box(vec3(startingPosX + 0.01, -1.6, startingPosZ + 0.03), 0.3, 0.05, 0.05, PlantColour);
    shapes[65] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ + 0.02), 0.5, 0.05, 0.05, PlantColour);

    shapes[66] = new Box(vec3(startingPosX, -1.725, startingPosZ), 0.75, 0.25, 0.5, PlantPotColour);

    /**
     * Another plant #3
     */
    startingPosX = 0.05;
    startingPosZ = -3.3;
    shapes[67] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ - 0.06), 0.75, 0.05, 0.05, PlantColour);
    shapes[68] = new Box(vec3(startingPosX - 0.02, -1.6, startingPosZ - 0.05), 0.1, 0.05, 0.05, PlantColour);
    shapes[69] = new Box(vec3(startingPosX, -1.6, startingPosZ - 0.04), 0.4, 0.05, 0.05, PlantColour);
    shapes[70] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ - 0.03), 0.65, 0.05, 0.05, PlantColour);
    shapes[71] = new Box(vec3(startingPosX + 0.01, -1.6, startingPosZ - 0.02), 0.5, 0.05, 0.05, PlantColour);

    shapes[72] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ + 0.06), 0.75, 0.05, 0.05, PlantColour);
    shapes[73] = new Box(vec3(startingPosX - 0.02, -1.6, startingPosZ + 0.05), 0.12, 0.05, 0.05, PlantColour);
    shapes[74] = new Box(vec3(startingPosX, -1.6, startingPosZ + 0.04), 0.5, 0.05, 0.05, PlantColour);
    shapes[75] = new Box(vec3(startingPosX + 0.01, -1.6, startingPosZ + 0.03), 0.3, 0.05, 0.05, PlantColour);
    shapes[76] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ + 0.02), 0.5, 0.05, 0.05, PlantColour);

    shapes[77] = new Box(vec3(startingPosX, -1.725, startingPosZ), 0.75, 0.25, 0.5, PlantPotColour);

    /**
     * The following objects is the set of 4 chairs at the far left
     */
    vec3 ChairColour = vec3(0.5, 0.5, 0.5);
    startingPosX = -0.6;
    startingPosZ = -3.95;
    float gap = 0.21;
    shapes[78] = new Box(vec3(startingPosX, -1.725, startingPosZ), 0.75, 0.6, 0.2, ChairColour);
    shapes[79] = new Box(vec3(startingPosX, -1.8075, startingPosZ + 0.1), 0.25, 0.6, 0.5, ChairColour);
    shapes[80] = new Box(vec3(startingPosX + gap, -1.725, startingPosZ), 0.75, 0.6, 0.2, ChairColour);
    shapes[81] = new Box(vec3(startingPosX + gap, -1.8075, startingPosZ + 0.1), 0.25, 0.6, 0.5, ChairColour);

    /**
     * The following objects is the set of 4 chairs at the far left
     */
    startingPosX = -0.7;
    startingPosZ = -2.8;
    shapes[82] = new Box(vec3(startingPosX, -1.725, startingPosZ), 0.75, 0.6, 0.2, ChairColour);
    shapes[83] = new Box(vec3(startingPosX, -1.8075, startingPosZ + 0.1), 0.25, 0.6, 0.5, ChairColour);
    shapes[84] = new Box(vec3(startingPosX + gap, -1.725, startingPosZ), 0.75, 0.6, 0.2, ChairColour);
    shapes[85] = new Box(vec3(startingPosX + gap, -1.8075, startingPosZ + 0.1), 0.25, 0.6, 0.5, ChairColour);
    shapes[86] = new Box(vec3(startingPosX + 2 * gap, -1.725, startingPosZ), 0.75, 0.6, 0.2, ChairColour);
    shapes[87] = new Box(vec3(startingPosX + 2 * gap, -1.8075, startingPosZ + 0.1), 0.25, 0.6, 0.5, ChairColour);
    shapes[88] = new Box(vec3(startingPosX + 3 * gap, -1.725, startingPosZ), 0.75, 0.6, 0.2, ChairColour);
    shapes[89] = new Box(vec3(startingPosX + 3 * gap, -1.8075, startingPosZ + 0.1), 0.25, 0.6, 0.5, ChairColour);

    /**
     * The following objects is the cluster of blue sofas
     */
    vec3 SofaColourBlue = vec3(0, 0, 1);
    vec3 SofaColourYellow = vec3(1, 1, 0);
    startingPosX = -0.75;
    startingPosZ = -2.2;
    shapes[90] = new Box(vec3(startingPosX, -1.8075, startingPosZ), 0.25, 0.5, 0.5, SofaColourBlue);
    shapes[91] = new Box(vec3(startingPosX - (0.5 * gap), -1.8075, startingPosZ + (2 * gap)), 0.25, 0.5, 0.5, SofaColourYellow);
    shapes[92] = new Box(vec3(startingPosX + gap, -1.8075, startingPosZ + (2.5 * gap)), 0.25, 0.5, 0.5, SofaColourBlue);
    shapes[93] = new Box(vec3(startingPosX + (2 * gap), -1.8075, startingPosZ + (2.5 * gap)), 0.25, 0.5, 0.5, SofaColourYellow);
    shapes[94] = new Box(vec3(startingPosX + (3 * gap), -1.8075, startingPosZ + (2.3 * gap)), 0.25, 0.5, 0.5, SofaColourYellow);
    shapes[95] = new Box(vec3(startingPosX + (1.75 * gap), -1.8075, startingPosZ - (0.3 * gap)), 0.25, 0.5, 0.5, SofaColourBlue);
    shapes[96] = new Box(vec3(startingPosX + (3 * gap), -1.8075, startingPosZ - (0.2 * gap)), 0.25, 0.5, 0.5, SofaColourBlue);

    /**
     * This object will render the table in the middle of the sofas
    */
    vec3 TableColour = vec3(1, 1, 1);
    startingPosX = -0.75;
    startingPosZ = -2.2;
    shapes[97] = new Box(vec3(startingPosX + (2* gap), -1.725, startingPosZ + (gap)), 0.1, 2, 0.6, TableColour);
}
