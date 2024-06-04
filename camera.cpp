#include "camera.h"

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
    position = startPosition;
    up = startUp;
    yaw = startYaw;
    pitch = startPitch;
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    moveSpeed = startMoveSpeed;
    turnSpeed = startTurnSpeed;
    zoom = 45.0f;
    roll = 0.0f;
    update();
}

glm::mat4 Camera::calculateViewMatrix()
{
    return glm::lookAt(position, position + front, up);
}

void Camera::update()
{
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(roll), front);
    // Rotate the up vector
    up = glm::normalize(glm::vec3(rotation * glm::vec4(up, 0.0f)));
}
void Camera::keyControl(bool* keys, GLfloat deltaTime)
{
    GLfloat velocity = moveSpeed * deltaTime;
    if(keys[GLFW_KEY_LEFT_SHIFT])
    {
        velocity *= 2.0f;
    }
    if (keys[GLFW_KEY_W])
    {
        position += front * velocity;
    }
    if (keys[GLFW_KEY_S])
    {
        position -= front * velocity;
    }
    if (keys[GLFW_KEY_A])
    {
        position -= glm::normalize(glm::cross(front, up)) * velocity;
    }
    if (keys[GLFW_KEY_D])
    {
        position += glm::normalize(glm::cross(front, up)) * velocity;
    }
    if (keys[GLFW_KEY_SPACE])
    {
        position += up * velocity;
    }
    if (keys[GLFW_KEY_LEFT_CONTROL])
    {
        position -= up * velocity;
    }
    GLfloat yawSpeed = 50.0f;
    if (keys[GLFW_KEY_J])
    {
        yaw -= yawSpeed * deltaTime;
    }
    if (keys[GLFW_KEY_L])
    {
        yaw += yawSpeed * deltaTime;
    }
    if (keys[GLFW_KEY_I])
    {
        pitch += yawSpeed * deltaTime;
    }
    if (keys[GLFW_KEY_K])
    {
        pitch -= yawSpeed * deltaTime;
    }
    if (pitch > 89.0f)
    {
        pitch = 89.0f;
    }
    if (pitch < -89.0f)
    {
        pitch = -89.0f;
    }
    update();
}
void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
    xChange *= turnSpeed;
    yChange *= turnSpeed;

    yaw += xChange;
    pitch += yChange;

    if (pitch > 89.0f)
    {
        pitch = 89.0f;
    }

    if (pitch < -89.0f)
    {
        pitch = -89.0f;
    }

    update();
}
void Camera::scrollControl(GLfloat yOffset) 
{
    zoom -= yOffset;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f;
}