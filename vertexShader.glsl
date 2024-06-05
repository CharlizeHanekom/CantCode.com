#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace; // Normal vector input
layout(location = 2) in vec4 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;  // Position of the fragment (for lighting calculation)
out vec3 Normal;   // Normal vector (for lighting calculation)
out vec4 fragmentColor;

void main() {
    // Transform the vertex position to world space
    FragPos = vec3(model * vec4(vertexPosition_modelspace, 1.0));
    // Transform the normal vector to world space
    Normal = mat3(transpose(inverse(model))) * vertexNormal_modelspace;

    // Transform the vertex position to clip space using view and projection matrices
    gl_Position = projection * view * vec4(FragPos, 1.0);

    // Pass the vertex color to the fragment shader
    fragmentColor = vertexColor;
}
