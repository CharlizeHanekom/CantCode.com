#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;

uniform mat4 view;
uniform mat4 projection;

out vec3 fragmentColor;

void main() {
    // Transform the vertex position to clip space using view and projection matrices
    gl_Position = projection * view * vec4(vertexPosition_modelspace, 1.0);
    fragmentColor = vertexColor;
}
