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
vec4 glassColor(0, 0.7, 1, 0.2);
vec4 greyWallColor(0.4, 0.4, 0.4, 1);
vec4 whiteWallColor(0.9, 0.9, 0.9, 1);
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
    shapes[2] = new Box(center, largeHeight, outWidth, totalLength, vec4(0.9, 0.9, 0.9, 1));

    // Top pieces
    shapes[0] = new Box(vec3(center.x, center.y + largeHeight / 2 - smallHeight / 2, center.z), smallHeight, 0.01, totalLength, glassColor);
    shapes[1] = new Box(vec3(center.x, center.y + largeHeight - smallHeight, center.z), largeHeight, outWidth, totalLength, greyWallColor);

    // Bottom pieces
    shapes[3] = new Box(vec3(center.x, center.y - largeHeight / 2 + smallHeight / 2, center.z), smallHeight, 0.01, totalLength, glassColor);
    shapes[4] = new Box(vec3(center.x, center.y - largeHeight + smallHeight, center.z), largeHeight, outWidth, totalLength, whiteWallColor);

    // Pillars
    float pillarSpacing = totalLength / 50;
    for (int i = 0; i < 15; i++)
    {
        float pillarZ = center.z - totalLength / 6 + pillarSpacing * (i + 1);
        shapes[5 + i] = new Box(vec3(center.x, center.y, pillarZ), pillarHeight, outWidth - 0.5, pillarWidth, vec4(0.8, 0.8, 0.8, 1));
    }
    // shapes[20] = new WoodenDecoration(center);
}

EastWall::EastWall(vec3 center)
{
    numShapes = 7;
    shapes = new Shape *[numShapes];
    float totalWidth = 8;
    float middleWidth = totalWidth / 3;
    float sideWidth = totalWidth / 6;

    // Middle wall
    shapes[2] = new Box(center, 11, middleWidth, outWidth, vec4(0.4, 0.4, 0.4, 1));

    // Left walls
    shapes[0] = new Box(vec3(center.x - middleWidth / 2 + sideWidth / 2, center.y + 0.3, center.z), 10, sideWidth, 0.01, glassColor);
    shapes[1] = new Box(vec3(center.x - middleWidth / 2 + sideWidth / 6, center.y, center.z), 11, sideWidth, outWidth, greyWallColor);

    // Right walls
    shapes[3] = new Box(vec3(center.x + middleWidth / 2 - sideWidth / 2, center.y + 0.3, center.z), 10, sideWidth, 0.01, glassColor);
    shapes[4] = new Box(vec3(center.x + middleWidth / 2 - sideWidth / 6, center.y, center.z), 11, sideWidth, outWidth, greyWallColor);

        //Doors
    shapes[5] = new Door(vec3(center.x - middleWidth / 2 + sideWidth / 2, center.y - 1.6, center.z));
    shapes[6] = new Door(vec3(center.x + middleWidth / 2 - sideWidth / 2, center.y - 1.6, center.z));
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
    shapes[2] = new Box(center, largeHeight, outWidth, totalLength, whiteWallColor);

    // Top pieces
    shapes[0] = new Box(vec3(center.x, center.y + largeHeight / 2 - smallHeight / 2, center.z), smallHeight, 0.01, totalLength, glassColor);
    shapes[1] = new Box(vec3(center.x, center.y + largeHeight - smallHeight, center.z), largeHeight, outWidth, totalLength, greyWallColor);

    // Bottom pieces
    shapes[3] = new Box(vec3(center.x, center.y - largeHeight / 2 + smallHeight / 2, center.z), smallHeight, 0.01, totalLength, glassColor);
    shapes[4] = new Box(vec3(center.x, center.y - largeHeight + smallHeight, center.z), largeHeight, outWidth, totalLength, whiteWallColor);

    // Pillars
    float pillarSpacing = totalLength / 50;
    for (int i = 0; i < 15; i++)
    {
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
    shapes[1] = new Box(center, 11, middleWidth, 0.01, glassColor);

    // Left wall
    shapes[0] = new Box(vec3(center.x - middleWidth / 2 + sideWidth*1.18, center.y, center.z), 11, sideWidth, outWidth, greyWallColor);

    // Right wall
    shapes[2] = new Box(vec3(center.x + middleWidth / 2 - sideWidth*1.18, center.y, center.z), 11, sideWidth, outWidth, greyWallColor);
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

    for (int i = 0; i < numShapes; i++)
    {
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
Door::Door(vec3 center)
{
    numShapes = 4; // Increase the number of shapes to accommodate the frame
    shapes = new Shape *[numShapes];

    // Door
    shapes[0] = new Box(center, 1.5, 1.5, 0.2, vec4(0.6, 0.8, 0.8, 0.8));

    // Frame
    float frameThickness = 0.1;
    float frameDepth = 0.3;
    vec4 frameColor(0.3, 0.3, 0.3,1);

    // Left side of the frame
    shapes[1] = new Box(vec3(center.x - 0.21, center.y, center.z), 1.6, frameThickness, frameDepth, frameColor);

    // Right side of the frame
    shapes[2] = new Box(vec3(center.x + 0.21, center.y, center.z), 1.6, frameThickness, frameDepth, frameColor);

    // Top of the frame
    shapes[3] = new Box(vec3(center.x, center.y + 0.27, center.z), frameThickness, 1.5 + 2 * frameThickness, frameDepth, frameColor);
}

Scene::Scene()
{
    numShapes = 4;
    shapes = new Shape *[numShapes];
    shapes[0] = new Walls();
    shapes[1] = new Objects();
    shapes[2] = new Roof();
    shapes[3] = new Floor();
}

Objects::Objects()
{
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

    numShapes = 110;
    shapes = new Shape *[numShapes];

    /**
     * The following object is the table near left to the coffee shop
     */
    float startingPosX = -0.5;
    float startingPosZ = 4;
    shapes[0] = new Box(vec3(startingPosX, -1.6, startingPosZ), 0.1, 0.75, 0.75, TableColour);
    shapes[1] = new Box(vec3(startingPosX, -1.725, startingPosZ), 0.75, 0.1, 0.1, TableColour);

    /**
     * The following object is the table near middle to the coffee shop
     */
    startingPosX = 0;
    startingPosZ = 4.2;
    shapes[2] = new Box(vec3(startingPosX, -1.6, startingPosZ), 0.1, 0.75, 0.75, TableColour);
    shapes[3] = new Box(vec3(startingPosX, -1.725, startingPosZ), 0.75, 0.1, 0.1, TableColour);

    /**
     * The following objects are the dividers
     */

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

    startingPosX = -0.23;
    startingPosZ = 2.5;
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
     * The following objects is the set of 2 chairs at the far left
     */

    startingPosX = -0.6;
    startingPosZ = -3.95;
    float gap = 0.21;
    shapes[78] = new Box(vec3(startingPosX, -1.725, startingPosZ), 0.75, 0.6, 0.2, ChairColour);
    shapes[79] = new Box(vec3(startingPosX, -1.8075, startingPosZ + 0.1), 0.25, 0.6, 0.5, ChairColour);
    shapes[80] = new Box(vec3(startingPosX + gap, -1.725, startingPosZ), 0.75, 0.6, 0.2, ChairColour);
    shapes[81] = new Box(vec3(startingPosX + gap, -1.8075, startingPosZ + 0.1), 0.25, 0.6, 0.5, ChairColour);

    /**
     * The following objects is the set of 4 chairs at the middle left
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

    startingPosX = -0.75;
    startingPosZ = -2.2;
    shapes[97] = new Box(vec3(startingPosX + (2 * gap), -1.725, startingPosZ + (gap)), 0.1, 2, 0.6, TableColour);

    /**
     * The following objects is the set of 4 chairs at the middle infrornt of the dividerr
     */
    startingPosX = -0.3;
    startingPosZ = -0.95;
    shapes[98] = new Box(vec3(startingPosX, -1.725, startingPosZ), 0.75, 0.6, 0.2, ChairColour);
    shapes[99] = new Box(vec3(startingPosX, -1.8075, startingPosZ + 0.1), 0.25, 0.6, 0.5, ChairColour);
    shapes[100] = new Box(vec3(startingPosX + gap, -1.725, startingPosZ), 0.75, 0.6, 0.2, ChairColour);
    shapes[101] = new Box(vec3(startingPosX + gap, -1.8075, startingPosZ + 0.1), 0.25, 0.6, 0.5, ChairColour);
    shapes[102] = new Box(vec3(startingPosX + 2 * gap, -1.725, startingPosZ), 0.75, 0.6, 0.2, ChairColour);
    shapes[103] = new Box(vec3(startingPosX + 2 * gap, -1.8075, startingPosZ + 0.1), 0.25, 0.6, 0.5, ChairColour);
    shapes[104] = new Box(vec3(startingPosX + 3 * gap, -1.725, startingPosZ), 0.75, 0.6, 0.2, ChairColour);
    shapes[105] = new Box(vec3(startingPosX + 3 * gap, -1.8075, startingPosZ + 0.1), 0.25, 0.6, 0.5, ChairColour);

    /**
     * The following objects is the set of 2 csofas and a small table at the middle left
     */

    startingPosX = -1.1;
    startingPosZ = 0.0;
    shapes[106] = new Box(vec3(startingPosX, -1.8075, startingPosZ), 0.25, 0.5, 0.5, SofaColourBlue);
    shapes[107] = new Box(vec3(startingPosX, -1.8075, startingPosZ + (2.2 * gap)), 0.25, 0.5, 0.5, SofaColourYellow);
    shapes[108] = new Box(vec3(startingPosX, -1.725, startingPosZ + (gap)), 0.1, 0.8, 0.8, TableColour);
    shapes[109] = new Box(vec3(startingPosX + gap, -1.8075, startingPosZ + (1.8 * gap)), 0.25, 0.5, 0.5, SofaColourBlue);
}

Roof::Roof()
{
    numShapes = 3*10 + 5;
    shapes = new Shape *[numShapes];

    //panes
    double x = 0;
    double y = 0.0;
    double z = 15;
    int index = 0;
    for (int i = 0; i < 10; i++) {
        shapes[index++] = new Cylinder(vec3(x,y,z), 50, 2, 6.8, vec4(0.5,0.5,0.0,0.7));
        shapes[index++] = new Cylinder(vec3(x,y,z-=1.5), 50, 1, 6.8, vec4(0.4,0.4,0.4,0.4));
        shapes[index++] = new Cylinder(vec3(x,y,z-=1.5), 50, 2, 6.8, vec4(0.5,0.5,0.0,0.7));
    }

    //frames
    x = -1;
    y = 2;
    z = 0;
    shapes[index++] = new Box(vec3(x,y,z), 0.05, 0.05, 32, vec4(0.3,0.3,0.3,1.0));
    shapes[index++] = new Box(vec3(x+=0.5,y+=0.2,z), 0.05, 0.05, 32, vec4(0.3,0.3,0.3,1.0));
    shapes[index++] = new Box(vec3(x+=0.5,y+=0.05,z), 0.05, 0.05, 32, vec4(0.3,0.3,0.3,1.0));
    shapes[index++] = new Box(vec3(x+=0.5,y-=0.05,z), 0.05, 0.05, 32, vec4(0.3,0.3,0.3,1.0));
    shapes[index++] = new Box(vec3(x+=0.5,y-=0.2,z), 0.05, 0.05, 32, vec4(0.3,0.3,0.3,1.0));
}

Cylinder::Cylinder(vec3 center, int numSidesOnBase, float height, float radius, vec4 color)
{
    float angle = 2 * M_PI / numSidesOnBase;
    vec3 *vertices = new vec3[2*numSidesOnBase];

    int startIndex = numSidesOnBase * 0.575 / 4;
    int endIndex = numSidesOnBase * 1.575 /4;

    for (int i = startIndex; i < endIndex; i++)
    {
        float x = center.x + radius * cos(i * angle);
        float y = center.y + radius * sin(i * angle);
        float z = center.z + height / 2;
        vertices[i] = vec3(x, y, z);
    }

    for (int i = startIndex; i < endIndex; i++)
    {
        float x = center.x + radius * cos(i * angle);
        float y = center.y + radius * sin(i * angle);
        float z = center.z - height / 2;
        vertices[numSidesOnBase + i] = vec3(x, y, z);
    }

    numShapes = 2 * (endIndex - startIndex - 1);
    shapes = new Shape *[numShapes];
    for (int i = startIndex; i < endIndex - 1; i++)
    {
        int nextIndex = i + 1;
        shapes[2 * (i - startIndex)] = new Triangle(vertices[i], vertices[nextIndex], vertices[numSidesOnBase + i], color);
        shapes[2 * (i - startIndex) + 1] = new Triangle(vertices[nextIndex], vertices[numSidesOnBase + nextIndex], vertices[numSidesOnBase + i], color);
    }
}

Floor::Floor() {
    numShapes = 2;
    shapes = new Shape *[numShapes];
    shapes[0] = new Rectangle(vec3(-4, -5.55, -16.5), vec3(4, -5.55, -16.5), vec3(-4, -5.55, 16.5), vec3(4, -5.55, 16.5), vec4(0.3,0.3,0.3,1.0));
    shapes[1] = new Cutout();
}

Cutout::Cutout() {
    numShapes = 12;
    shapes = new Shape *[numShapes];

    vec3 ul[6] = {
        vec3(2, -5.54, -10),
        vec3(2, -5.54, -6),
        vec3(2, -5.54, -2),
        vec3(2, -5.54, 2),
        vec3(2, -5.54, 6),
        vec3(2, -5.54, 10)
    };
    vec3 ur[6] = {
        vec3(4, -5.54, -10),
        vec3(4, -5.54, -6),
        vec3(4, -5.54, -2),
        vec3(4, -5.54, 2),
        vec3(4, -5.54, 6),
        vec3(4, -5.54, 10)
    };
    vec3 ll[6] = {
        vec3(2, -5.54, -8),
        vec3(2, -5.54, -4),
        vec3(2, -5.54, 0),
        vec3(2, -5.54, 4),
        vec3(2, -5.54, 8),
        vec3(2, -5.54, 12)
    };
    vec3 lr[6] =  {
        vec3(4, -5.54, -8),
        vec3(4, -5.54, -4),
        vec3(4, -5.54, 0),
        vec3(4, -5.54, 4),
        vec3(4, -5.54, 8),
        vec3(4, -5.54, 12)
    };

    vec4 black = vec4(0.0,0.0,0.0,1.0);

    for (int i = 0; i < 6; i++) {
       shapes[i] = new Rectangle(ul[i], ur[i], ll[i], lr[i], black);
    }

    vec3 centers[6] = {
        vec3(1, -1.7, -9)*vec3(1,1,0.33333),
        vec3(1, -1.7, -5)*vec3(1,1,0.33333),
        vec3(1, -1.7, -1)*vec3(1,1,0.33333),
        vec3(1, -1.7, 3)*vec3(1,1,0.33333),
        vec3(1, -1.7, 7)*vec3(1,1,0.33333),
        vec3(1, -1.7, 11)*vec3(1,1,0.33333)
    };

    double dim = 2;
    double height = 1;

    for (int i = 0; i < 6; i++) {
        shapes[6 + i] = new Box(centers[i], height, dim, dim, vec4(0.5,0.5,0.5,0.5));
    }
}