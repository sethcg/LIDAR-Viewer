#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uTex;

void main() {
    FragColor = texture(uTex, vUV);
    if (FragColor.a < 0.1)
        discard;
}