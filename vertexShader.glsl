#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;
uniform mat4 view;
uniform mat4 projection;
out vec3 fragmentColor;

void main(){
    gl_Position.xyz = vertexPosition_modelspace;
    gl_Position.w = 1.0;
//    gl_Position = projection * view * vec4(position, 1.0);
    fragmentColor = vertexColor;
}

