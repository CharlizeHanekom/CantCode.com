#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace std;
class Camera {
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    GLfloat zoom;

    Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);

    glm::mat4 calculateViewMatrix();

    void keyControl(bool* keys, GLfloat deltaTime);
    void mouseControl(GLfloat xChange, GLfloat yChange);
    void scrollControl(GLfloat yChange);

private:
    GLfloat yaw, pitch;
    GLfloat moveSpeed, turnSpeed;

    void update();
};
#endif /*CAMERA_H*/