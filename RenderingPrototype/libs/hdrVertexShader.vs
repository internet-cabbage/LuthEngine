#version 410 core
layout (location = 0) in vec3 aPos;

out vec2 HDRTexCoords;

void main() {
    // HDR coordinates is a texture so it has coordinates in the range [0,1] in both the x and y axis
    // Therefore I have to map the aPos vals to tex coords
    HDRTexCoords = 0.5 * aPos.xy + 0.5;
    gl_Position = vec4(aPos,1.0);
}