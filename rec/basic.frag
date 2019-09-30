#version 330 core

out vec4 col;

in vec2 texCoords;
uniform sampler2D texture_diffuse1;
layout(location = 0) out vec3 color;

void main()
{
        col = texture(texture_diffuse1, texCoords);
 //       col = vec4(0.6, 0.6, 0.6, 1.0);
}
