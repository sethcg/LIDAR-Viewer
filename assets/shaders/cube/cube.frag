#version 330 core

in float vIntensity;

out vec4 FragColor;

uniform sampler1D uColorLUT;

void main() {
    vec3 color = texture(uColorLUT, clamp(vIntensity, 0.0, 1.0)).rgb;
    FragColor = vec4(color, 1.0);
}