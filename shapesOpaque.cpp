#include "shapesOpaque.h"

ShapeOpaque::~ShapeOpaque()
{
}

int ShapeOpaque::numPoints()
{
    int count = 0;
    for (int i = 0; i < numShapeOpaques; i++)
    {
        count += ShapeOpaques[i]->numPoints();
    }
    return count;
}

GLfloat *ShapeOpaque::toVertexArray()
{
    int n = numVertices();
    GLfloat *result = new GLfloat[numVertices()];
    if (numShapeOpaques > 0)
    {
        int count = 0;
        for (int i = 0; i < numShapeOpaques; i++)
        {
            GLfloat *temp = ShapeOpaques[i]->toVertexArray();
            for (int j = 0; j < ShapeOpaques[i]->numVertices(); j++)
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

GLfloat *ShapeOpaque::toColorArray()
{
    GLfloat *result = new GLfloat[numColors()];
    if (numShapeOpaques > 0)
    {
        int count = 0;
        for (int i = 0; i < numShapeOpaques; i++)
        {
            GLfloat *temp = ShapeOpaques[i]->toColorArray();
            for (int j = 0; j < ShapeOpaques[i]->numColors(); j++)
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

void ShapeOpaque::applyMatrix(mat4x4 m)
{
    if (numShapeOpaques > 0)
    {
        for (int i = 0; i < numShapeOpaques; i++)
        {
            ShapeOpaques[i]->applyMatrix(m);
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

int ShapeOpaque::numVertices()
{
    int count = 0;
    for (int i = 0; i < numShapeOpaques; i++)
    {
        count += ShapeOpaques[i]->numVertices();
    }
    return count;
}

int ShapeOpaque::numColors()
{
    int count = 0;
    for (int i = 0; i < numShapeOpaques; i++)
    {
        count += ShapeOpaques[i]->numColors();
    }
    return count;
}

Triangle::Triangle(vec3 point1, vec3 point2, vec3 point3, vec3 color)
{
    numShapeOpaques = 0;
    ShapeOpaques = new ShapeOpaque *[0];

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
    numShapeOpaques = 2;
    ShapeOpaques = new ShapeOpaque *[numShapeOpaques];
    ShapeOpaques[0] = new Triangle(ur, ul, ll, color);
    ShapeOpaques[1] = new Triangle(ll, lr, ur, color);
    int n = numPoints();
    vertices = new vec3 *[n];
    int count = 0;
    for (int i = 0; i < numShapeOpaques; i++)
    {
        for (int j = 0; j < ShapeOpaques[i]->numPoints(); j++)
        {
            vertices[count++] = ShapeOpaques[i]->vertices[j];
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
    numShapeOpaques = 3;
    ShapeOpaques = new ShapeOpaque *[numShapeOpaques];
    ShapeOpaques[0] = new Triangle(
        vec3(0, 0.4, 0),
        vec3(-0.2, 0.2, 0),
        vec3(0.2, 0.2, 0));
    ShapeOpaques[1] = new Rectangle(
        vec3(-0.2, 0.2, 0.5),
        vec3(0.2, 0.2, 0.5),
        vec3(-0.2, -0.2, 0.5),
        vec3(0.2, -0.2, 0.5));
    ShapeOpaques[2] = new Rectangle(
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

    numShapeOpaques = 6;
    ShapeOpaques = new ShapeOpaque *[numShapeOpaques]
    {
        front, back, left, right, bottom, top
    };
}

Boxes::Boxes(int numBoxes, vec3 *centers, double *heights, double *widths, double *lengths, vec3 *colors)
{
    numShapeOpaques = numBoxes;
    ShapeOpaques = new ShapeOpaque *[numShapeOpaques];
    for (int i = 0; i < numShapeOpaques; i++)
    {
        ShapeOpaques[i] = new Box(centers[i], heights[i], widths[i], lengths[i], colors[i]);
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
    numShapeOpaques = 4;
    ShapeOpaques = new ShapeOpaque *[numShapeOpaques];
    for (int i = 0; i < numShapeOpaques; i++)
    {
        ShapeOpaques[i] = new Box(wallCenters[i], wallHeights[i], wallWidths[i], wallLengths[i], wallColors[i]);
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
    numShapeOpaques = 2 * numSidesOnBase;
    ShapeOpaques = new ShapeOpaque *[numShapeOpaques];
    for (int i = 0; i < numSidesOnBase; i++)
    {
        int nextIndex = (i + 1) % numSidesOnBase;
        ShapeOpaques[i] = new Triangle(vertices[i], vertices[nextIndex], vertices[numSidesOnBase + i], color);
        ShapeOpaques[numSidesOnBase + i] = new Triangle(vertices[nextIndex], vertices[numSidesOnBase + nextIndex], vertices[numSidesOnBase + i], color);
    }
}

void Cylinder::Circles(vec3 center, int numSidesOnBase, float height, float radius, vec3 color)
{
    // -------------------------------------------------- //
    // Drawing the Back face of the cylinder
    // -------------------------------------------------- //

    numShapeOpaques = numSidesOnBase * 2;
    ShapeOpaques = new ShapeOpaque *[numShapeOpaques];

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
        ShapeOpaques[i] = new Triangle(vec3(backWheel[0], backWheel[1], backWheel[2]), vec3(backWheel[(i + 1) * 3], backWheel[((i + 1) * 3) + 1], backWheel[((i + 1) * 3) + 2]), vec3(backWheel[(i + 2) * 3], backWheel[((i + 2) * 3) + 1], backWheel[((i + 2) * 3) + 2]), color);
    }
    for (int i = numSidesOnBase; i < (numSidesOnBase * 2); i++)
    {
        ShapeOpaques[i] = new Triangle(vec3(frontWheel[0], frontWheel[1], frontWheel[2]), vec3(frontWheel[(i - numSidesOnBase + 1) * 3], frontWheel[((i - numSidesOnBase + 1) * 3) + 1], frontWheel[((i - numSidesOnBase + 1) * 3) + 2]), vec3(frontWheel[(i - numSidesOnBase + 2) * 3], frontWheel[((i - numSidesOnBase + 2) * 3) + 1], frontWheel[((i - numSidesOnBase + 2) * 3) + 2]), color);
    }
}

void ShapeOpaque::RotateY(float angle)
{
    mat4x4 rotationMatrix = mat4x4(
        vec4(cos(angle), 0, sin(angle), 0),
        vec4(0, 1, 0, 0),
        vec4(-sin(angle), 0, cos(angle), 0),
        vec4(0, 0, 0, 1));
    applyMatrix(rotationMatrix);
}

void ShapeOpaque::RotateX(float angle)
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
    numShapeOpaques = 114;
    ShapeOpaques = new ShapeOpaque *[numShapeOpaques];

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
        ShapeOpaques[i] = new Box(wallCenters[i], wallHeights[i], wallWidths[i], wallLengths[i], wallColors[i]);
    }

    /**
     * The following objects are the dividers
     */
    vec3 DividerColour = vec3(1, 1, 1);
    // middle 2
    ShapeOpaques[4] = new Box(vec3(0, -1.5, -1), 2, 2.5, 0.05, DividerColour);
    ShapeOpaques[5] = new Box(vec3(0, -1.5, 1), 2, 2.5, 0.05, DividerColour);

    // far left
    ShapeOpaques[6] = new Box(vec3(-0.5, -1.5, -4), 2, 1.75, 0.05, DividerColour);

    // far right
    ShapeOpaques[7] = new Box(vec3(0, -1.5, -4.5), 2, 0.05, 1.75, DividerColour);
    ShapeOpaques[8] = new Box(vec3(0, -1.5, -3.5), 2, 0.05, 1.75, DividerColour);

    // Near left
    ShapeOpaques[9] = new Box(vec3(-0.2, -1.5, 2.5), 2, 0.05, 1.75, DividerColour);

    /**
     * The following objects is the set of 4 cubicles in the near left corner
     */
    vec3 CubicleColour = vec3(0.2, 0.2, 0.2);
    vec3 CubicleSeatColour = vec3(1, 1, 0);
    vec3 CubicleWoodColour = vec3(0.45, 0.25, 0.0);
    float startingPosX = -0.23;
    float startingPosZ = 2.5;
    ShapeOpaques[10] = new Box(vec3(startingPosX, -1.6, startingPosZ), 1.5, 0.05, 1.2, CubicleColour);
    ShapeOpaques[11] = new Box(vec3(startingPosX - 0.3, -1.6, startingPosZ), 1.5, 0.05, 1.2, CubicleColour);
    ShapeOpaques[12] = new Box(vec3(startingPosX - 0.325, -1.6, startingPosZ), 1.5, 0.05, 1.2, CubicleColour);
    ShapeOpaques[13] = new Box(vec3(startingPosX - 0.325 - 0.3, -1.6, startingPosZ), 1.5, 0.05, 1.2, CubicleColour);
    ShapeOpaques[14] = new Box(vec3(startingPosX - 0.325 - 0.15, -1.6, startingPosZ), 1.5, 0.9, 0.05, CubicleColour);
    ShapeOpaques[15] = new Box(vec3(startingPosX - 0.15, -1.6, startingPosZ), 1.5, 0.9, 0.05, CubicleColour);

    ShapeOpaques[16] = new Box(vec3(startingPosX - 0.1, -1.725, startingPosZ), 0.75, 0.6, 0.15, CubicleSeatColour);
    ShapeOpaques[17] = new Box(vec3(startingPosX - 0.1, -1.75, startingPosZ), 0.3, 0.6, 1.19, CubicleSeatColour);
    ShapeOpaques[18] = new Box(vec3(startingPosX - 0.2475, -1.75, startingPosZ), 0.3, 0.29, 1.19, CubicleWoodColour);

    ShapeOpaques[19] = new Box(vec3(startingPosX - 0.1 - 0.325, -1.725, startingPosZ), 0.75, 0.6, 0.15, CubicleSeatColour);
    ShapeOpaques[20] = new Box(vec3(startingPosX - 0.1 - 0.325, -1.75, startingPosZ), 0.3, 0.6, 1.19, CubicleSeatColour);
    ShapeOpaques[21] = new Box(vec3(startingPosX - 0.2475 - 0.325, -1.75, startingPosZ), 0.3, 0.29, 1.19, CubicleWoodColour);

    /**
     * The following objects is the set of 4 cubicles in the near left corner
     */
    startingPosX = 0.65;
    startingPosZ = -4.5;
    ShapeOpaques[22] = new Box(vec3(startingPosX, -1.6, startingPosZ), 1.5, 0.05, 1.2, CubicleColour);
    ShapeOpaques[23] = new Box(vec3(startingPosX - 0.3, -1.6, startingPosZ), 1.5, 0.05, 1.2, CubicleColour);
    ShapeOpaques[24] = new Box(vec3(startingPosX - 0.325, -1.6, startingPosZ), 1.5, 0.05, 1.2, CubicleColour);
    ShapeOpaques[25] = new Box(vec3(startingPosX - 0.325 - 0.3, -1.6, startingPosZ), 1.5, 0.05, 1.2, CubicleColour);
    ShapeOpaques[26] = new Box(vec3(startingPosX - 0.325 - 0.15, -1.6, startingPosZ), 1.5, 0.9, 0.05, CubicleColour);
    ShapeOpaques[27] = new Box(vec3(startingPosX - 0.15, -1.6, startingPosZ), 1.5, 0.9, 0.05, CubicleColour);

    ShapeOpaques[28] = new Box(vec3(startingPosX - 0.1, -1.725, startingPosZ), 0.75, 0.6, 0.15, CubicleSeatColour);
    ShapeOpaques[29] = new Box(vec3(startingPosX - 0.1, -1.75, startingPosZ), 0.3, 0.6, 1.19, CubicleSeatColour);
    ShapeOpaques[30] = new Box(vec3(startingPosX - 0.2475, -1.75, startingPosZ), 0.3, 0.29, 1.19, CubicleWoodColour);

    ShapeOpaques[31] = new Box(vec3(startingPosX - 0.1 - 0.325, -1.725, startingPosZ), 0.75, 0.6, 0.15, CubicleSeatColour);
    ShapeOpaques[32] = new Box(vec3(startingPosX - 0.1 - 0.325, -1.75, startingPosZ), 0.3, 0.6, 1.19, CubicleSeatColour);
    ShapeOpaques[33] = new Box(vec3(startingPosX - 0.2475 - 0.325, -1.75, startingPosZ), 0.3, 0.29, 1.19, CubicleWoodColour);

    /**
     * The following objects is the plant
     */
    vec3 PlantColour = vec3(0, 1, 0);
    vec3 PlantPotColour = vec3(0.5, 0.5, 0.5);
    startingPosX = -0.15;
    startingPosZ = 2.7;
    ShapeOpaques[34] = new Box(vec3(startingPosX, -1.725, startingPosZ), 0.75, 0.25, 0.5, PlantPotColour);

    ShapeOpaques[35] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ - 0.06), 0.75, 0.05, 0.05, PlantColour);
    ShapeOpaques[36] = new Box(vec3(startingPosX - 0.02, -1.6, startingPosZ - 0.05), 0.1, 0.05, 0.05, PlantColour);
    ShapeOpaques[37] = new Box(vec3(startingPosX, -1.6, startingPosZ - 0.04), 0.4, 0.05, 0.05, PlantColour);
    ShapeOpaques[38] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ - 0.03), 0.65, 0.05, 0.05, PlantColour);
    ShapeOpaques[39] = new Box(vec3(startingPosX + 0.01, -1.6, startingPosZ - 0.02), 0.5, 0.05, 0.05, PlantColour);

    ShapeOpaques[40] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ + 0.06), 0.75, 0.05, 0.05, PlantColour);
    ShapeOpaques[41] = new Box(vec3(startingPosX - 0.02, -1.6, startingPosZ + 0.05), 0.12, 0.05, 0.05, PlantColour);
    ShapeOpaques[42] = new Box(vec3(startingPosX, -1.6, startingPosZ + 0.04), 0.5, 0.05, 0.05, PlantColour);
    ShapeOpaques[43] = new Box(vec3(startingPosX + 0.01, -1.6, startingPosZ + 0.03), 0.3, 0.05, 0.05, PlantColour);
    ShapeOpaques[44] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ + 0.02), 0.5, 0.05, 0.05, PlantColour);

    /**
     * Another plant
     */
    startingPosX = -0.15;
    startingPosZ = 2.3;
    ShapeOpaques[45] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ - 0.06), 0.75, 0.05, 0.05, PlantColour);
    ShapeOpaques[46] = new Box(vec3(startingPosX - 0.02, -1.6, startingPosZ - 0.05), 0.1, 0.05, 0.05, PlantColour);
    ShapeOpaques[47] = new Box(vec3(startingPosX, -1.6, startingPosZ - 0.04), 0.4, 0.05, 0.05, PlantColour);
    ShapeOpaques[48] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ - 0.03), 0.65, 0.05, 0.05, PlantColour);
    ShapeOpaques[49] = new Box(vec3(startingPosX + 0.01, -1.6, startingPosZ - 0.02), 0.5, 0.05, 0.05, PlantColour);

    ShapeOpaques[50] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ + 0.06), 0.75, 0.05, 0.05, PlantColour);
    ShapeOpaques[51] = new Box(vec3(startingPosX - 0.02, -1.6, startingPosZ + 0.05), 0.12, 0.05, 0.05, PlantColour);
    ShapeOpaques[52] = new Box(vec3(startingPosX, -1.6, startingPosZ + 0.04), 0.5, 0.05, 0.05, PlantColour);
    ShapeOpaques[53] = new Box(vec3(startingPosX + 0.01, -1.6, startingPosZ + 0.03), 0.3, 0.05, 0.05, PlantColour);
    ShapeOpaques[54] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ + 0.02), 0.5, 0.05, 0.05, PlantColour);

    ShapeOpaques[55] = new Box(vec3(startingPosX, -1.725, startingPosZ), 0.75, 0.25, 0.5, PlantPotColour);

    /**
     * Another plant #2
     */
    startingPosX = 0.05;
    startingPosZ = -3.7;
    ShapeOpaques[56] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ - 0.06), 0.75, 0.05, 0.05, PlantColour);
    ShapeOpaques[57] = new Box(vec3(startingPosX - 0.02, -1.6, startingPosZ - 0.05), 0.1, 0.05, 0.05, PlantColour);
    ShapeOpaques[58] = new Box(vec3(startingPosX, -1.6, startingPosZ - 0.04), 0.4, 0.05, 0.05, PlantColour);
    ShapeOpaques[59] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ - 0.03), 0.65, 0.05, 0.05, PlantColour);
    ShapeOpaques[60] = new Box(vec3(startingPosX + 0.01, -1.6, startingPosZ - 0.02), 0.5, 0.05, 0.05, PlantColour);

    ShapeOpaques[61] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ + 0.06), 0.75, 0.05, 0.05, PlantColour);
    ShapeOpaques[62] = new Box(vec3(startingPosX - 0.02, -1.6, startingPosZ + 0.05), 0.12, 0.05, 0.05, PlantColour);
    ShapeOpaques[63] = new Box(vec3(startingPosX, -1.6, startingPosZ + 0.04), 0.5, 0.05, 0.05, PlantColour);
    ShapeOpaques[64] = new Box(vec3(startingPosX + 0.01, -1.6, startingPosZ + 0.03), 0.3, 0.05, 0.05, PlantColour);
    ShapeOpaques[65] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ + 0.02), 0.5, 0.05, 0.05, PlantColour);

    ShapeOpaques[66] = new Box(vec3(startingPosX, -1.725, startingPosZ), 0.75, 0.25, 0.5, PlantPotColour);

    /**
     * Another plant #3
     */
    startingPosX = 0.05;
    startingPosZ = -3.3;
    ShapeOpaques[67] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ - 0.06), 0.75, 0.05, 0.05, PlantColour);
    ShapeOpaques[68] = new Box(vec3(startingPosX - 0.02, -1.6, startingPosZ - 0.05), 0.1, 0.05, 0.05, PlantColour);
    ShapeOpaques[69] = new Box(vec3(startingPosX, -1.6, startingPosZ - 0.04), 0.4, 0.05, 0.05, PlantColour);
    ShapeOpaques[70] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ - 0.03), 0.65, 0.05, 0.05, PlantColour);
    ShapeOpaques[71] = new Box(vec3(startingPosX + 0.01, -1.6, startingPosZ - 0.02), 0.5, 0.05, 0.05, PlantColour);

    ShapeOpaques[72] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ + 0.06), 0.75, 0.05, 0.05, PlantColour);
    ShapeOpaques[73] = new Box(vec3(startingPosX - 0.02, -1.6, startingPosZ + 0.05), 0.12, 0.05, 0.05, PlantColour);
    ShapeOpaques[74] = new Box(vec3(startingPosX, -1.6, startingPosZ + 0.04), 0.5, 0.05, 0.05, PlantColour);
    ShapeOpaques[75] = new Box(vec3(startingPosX + 0.01, -1.6, startingPosZ + 0.03), 0.3, 0.05, 0.05, PlantColour);
    ShapeOpaques[76] = new Box(vec3(startingPosX - 0.01, -1.6, startingPosZ + 0.02), 0.5, 0.05, 0.05, PlantColour);

    ShapeOpaques[77] = new Box(vec3(startingPosX, -1.725, startingPosZ), 0.75, 0.25, 0.5, PlantPotColour);

    /**
     * The following objects is the set of 2 chairs at the far left
     */
    vec3 ChairColour = vec3(0.5, 0.5, 0.5);
    startingPosX = -0.6;
    startingPosZ = -3.95;
    float gap = 0.21;
    ShapeOpaques[78] = new Box(vec3(startingPosX, -1.725, startingPosZ), 0.75, 0.6, 0.2, ChairColour);
    ShapeOpaques[79] = new Box(vec3(startingPosX, -1.8075, startingPosZ + 0.1), 0.25, 0.6, 0.5, ChairColour);
    ShapeOpaques[80] = new Box(vec3(startingPosX + gap, -1.725, startingPosZ), 0.75, 0.6, 0.2, ChairColour);
    ShapeOpaques[81] = new Box(vec3(startingPosX + gap, -1.8075, startingPosZ + 0.1), 0.25, 0.6, 0.5, ChairColour);

    /**
     * The following objects is the set of 4 chairs at the middle left
     */
    startingPosX = -0.7;
    startingPosZ = -2.8;
    ShapeOpaques[82] = new Box(vec3(startingPosX, -1.725, startingPosZ), 0.75, 0.6, 0.2, ChairColour);
    ShapeOpaques[83] = new Box(vec3(startingPosX, -1.8075, startingPosZ + 0.1), 0.25, 0.6, 0.5, ChairColour);
    ShapeOpaques[84] = new Box(vec3(startingPosX + gap, -1.725, startingPosZ), 0.75, 0.6, 0.2, ChairColour);
    ShapeOpaques[85] = new Box(vec3(startingPosX + gap, -1.8075, startingPosZ + 0.1), 0.25, 0.6, 0.5, ChairColour);
    ShapeOpaques[86] = new Box(vec3(startingPosX + 2 * gap, -1.725, startingPosZ), 0.75, 0.6, 0.2, ChairColour);
    ShapeOpaques[87] = new Box(vec3(startingPosX + 2 * gap, -1.8075, startingPosZ + 0.1), 0.25, 0.6, 0.5, ChairColour);
    ShapeOpaques[88] = new Box(vec3(startingPosX + 3 * gap, -1.725, startingPosZ), 0.75, 0.6, 0.2, ChairColour);
    ShapeOpaques[89] = new Box(vec3(startingPosX + 3 * gap, -1.8075, startingPosZ + 0.1), 0.25, 0.6, 0.5, ChairColour);

    /**
     * The following objects is the cluster of blue sofas
     */
    vec3 SofaColourBlue = vec3(0, 0, 1);
    vec3 SofaColourYellow = vec3(1, 1, 0);
    startingPosX = -0.75;
    startingPosZ = -2.2;
    ShapeOpaques[90] = new Box(vec3(startingPosX, -1.8075, startingPosZ), 0.25, 0.5, 0.5, SofaColourBlue);
    ShapeOpaques[91] = new Box(vec3(startingPosX - (0.5 * gap), -1.8075, startingPosZ + (2 * gap)), 0.25, 0.5, 0.5, SofaColourYellow);
    ShapeOpaques[92] = new Box(vec3(startingPosX + gap, -1.8075, startingPosZ + (2.5 * gap)), 0.25, 0.5, 0.5, SofaColourBlue);
    ShapeOpaques[93] = new Box(vec3(startingPosX + (2 * gap), -1.8075, startingPosZ + (2.5 * gap)), 0.25, 0.5, 0.5, SofaColourYellow);
    ShapeOpaques[94] = new Box(vec3(startingPosX + (3 * gap), -1.8075, startingPosZ + (2.3 * gap)), 0.25, 0.5, 0.5, SofaColourYellow);
    ShapeOpaques[95] = new Box(vec3(startingPosX + (1.75 * gap), -1.8075, startingPosZ - (0.3 * gap)), 0.25, 0.5, 0.5, SofaColourBlue);
    ShapeOpaques[96] = new Box(vec3(startingPosX + (3 * gap), -1.8075, startingPosZ - (0.2 * gap)), 0.25, 0.5, 0.5, SofaColourBlue);

    /**
     * This object will render the table in the middle of the sofas
    */
    vec3 TableColour = vec3(1, 1, 1);
    startingPosX = -0.75;
    startingPosZ = -2.2;
    ShapeOpaques[97] = new Box(vec3(startingPosX + (2* gap), -1.725, startingPosZ + (gap)), 0.1, 2, 0.6, TableColour);

    /**
     * The following objects is the set of 4 chairs at the middle infrornt of the dividerr
     */
    startingPosX = -0.3;
    startingPosZ = -0.95;
    ShapeOpaques[98] = new Box(vec3(startingPosX, -1.725, startingPosZ), 0.75, 0.6, 0.2, ChairColour);
    ShapeOpaques[99] = new Box(vec3(startingPosX, -1.8075, startingPosZ + 0.1), 0.25, 0.6, 0.5, ChairColour);
    ShapeOpaques[100] = new Box(vec3(startingPosX + gap, -1.725, startingPosZ), 0.75, 0.6, 0.2, ChairColour);
    ShapeOpaques[101] = new Box(vec3(startingPosX + gap, -1.8075, startingPosZ + 0.1), 0.25, 0.6, 0.5, ChairColour);
    ShapeOpaques[102] = new Box(vec3(startingPosX + 2 * gap, -1.725, startingPosZ), 0.75, 0.6, 0.2, ChairColour);
    ShapeOpaques[103] = new Box(vec3(startingPosX + 2 * gap, -1.8075, startingPosZ + 0.1), 0.25, 0.6, 0.5, ChairColour);
    ShapeOpaques[104] = new Box(vec3(startingPosX + 3 * gap, -1.725, startingPosZ), 0.75, 0.6, 0.2, ChairColour);
    ShapeOpaques[105] = new Box(vec3(startingPosX + 3 * gap, -1.8075, startingPosZ + 0.1), 0.25, 0.6, 0.5, ChairColour);

    /**
     * The following objects is the set of 2 csofas and a small table at the middle left
    */
    vec3 SofaColourRed = vec3(1, 0, 0);
    startingPosX = -1.1;
    startingPosZ = 0.0;
    ShapeOpaques[106] = new Box(vec3(startingPosX, -1.8075, startingPosZ), 0.25, 0.5, 0.5, SofaColourBlue);
    ShapeOpaques[107] = new Box(vec3(startingPosX, -1.8075, startingPosZ + (2.2 * gap)), 0.25, 0.5, 0.5, SofaColourYellow);
    ShapeOpaques[108] = new Box(vec3(startingPosX, -1.725, startingPosZ + (gap)), 0.1, 0.8, 0.8, TableColour);
    ShapeOpaques[109] = new Box(vec3(startingPosX + gap, -1.8075, startingPosZ + (1.8 * gap)), 0.25, 0.5, 0.5, SofaColourBlue);

    /**
     * The following object is the table near left to the coffee shop
    */
    startingPosX = -0.5;
    startingPosZ = 4;
    ShapeOpaques[110] = new Box(vec3(startingPosX, -1.6, startingPosZ), 0.1, 0.75, 0.75, TableColour);
    ShapeOpaques[111] = new Box(vec3(startingPosX, -1.725, startingPosZ), 0.75, 0.1, 0.1, TableColour);

    /**
     * The following object is the table near middle to the coffee shop
    */
    startingPosX = 0;
    startingPosZ = 4.2;
    ShapeOpaques[112] = new Box(vec3(startingPosX, -1.6, startingPosZ), 0.1, 0.75, 0.75, TableColour);
    ShapeOpaques[113] = new Box(vec3(startingPosX, -1.725, startingPosZ), 0.75, 0.1, 0.1, TableColour);



}
