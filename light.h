#ifndef LIGHT_H
#define LIGHT_H
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cmath>

using namespace glm;
using namespace std;

struct PointLight {
    glm::vec3 position;
    glm::vec3 color;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    PointLight(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
               glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f), 
               glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f), 
               glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f),
               float constant = 1.0f,
               float linear = 0.09f,
               float quadratic = 0.032f)
    {
        this->position = position;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
        this->constant = constant;
        this->linear = linear;
        this->quadratic = quadratic;
        this->color = glm::vec3(1.0f, 1.0f, 1.0f);
    }
};

#endif