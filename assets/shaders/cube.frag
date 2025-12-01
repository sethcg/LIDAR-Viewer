#version 330 core

out vec4 FragColor;

uniform vec3 cubeColor;
uniform vec3 globalColor;

void main()
{
    FragColor = vec4(cubeColor * globalColor, 1.0);
}