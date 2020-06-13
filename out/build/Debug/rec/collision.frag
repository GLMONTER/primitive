#version 330 core

out vec4 col;

in vec2 texCoords;
uniform sampler2D texture_diffuse1;
layout(location = 0) out vec3 color;

void main()
{
    col = vec4(0, 255, 0, 255);
}
