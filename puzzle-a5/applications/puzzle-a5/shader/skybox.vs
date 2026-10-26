#version 430
layout (location = 0) in vec3 aPos;

out vec3 pos;

uniform mat4 proj;
uniform mat4 view;

void main()
{
    pos = -aPos;
    gl_Position = proj * view * vec4(aPos, 1.0);
} 