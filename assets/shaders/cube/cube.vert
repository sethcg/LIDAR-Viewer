#version 330 core

layout(location = 0) in vec3 aPos;              // CUBE VERTEX POSITIONS
layout(location = 1) in vec4 aModelRow0;        // Instance model matrix row 0
layout(location = 2) in vec4 aModelRow1;        // Instance model matrix row 1
layout(location = 3) in vec4 aModelRow2;        // Instance model matrix row 2
layout(location = 4) in vec4 aModelRow3;        // Instance model matrix row 3
layout(location = 5) in vec3 aInstanceColor;    // PER-INSTANCE COLOR

uniform mat4 uViewProjection;
uniform float uGlobalScale;

out vec3 vColor;

void main() {
    // APPLY GLOBAL SCALE (INLINE FOR PERFORMANCE)
    mat4 model = mat4(
        aModelRow0 * vec4(uGlobalScale, uGlobalScale, uGlobalScale, 1.0),
        aModelRow1 * vec4(uGlobalScale, uGlobalScale, uGlobalScale, 1.0),
        aModelRow2 * vec4(uGlobalScale, uGlobalScale, uGlobalScale, 1.0),
        aModelRow3
    );

    gl_Position = uViewProjection * model * vec4(aPos, 1.0);
    vColor = aInstanceColor;
}
