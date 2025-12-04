#version 330 core

in vec3 vColor;
uniform vec3 uGlobalColor;

out vec4 FragColor;

void main() {
    FragColor = vec4(vColor * uGlobalColor, 1.0);
}