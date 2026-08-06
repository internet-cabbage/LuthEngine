#version 410 core
out vec4 fragColor;

in vec2 HDRTexCoords;

uniform sampler2D hdrBuffer;
uniform float exposure;

void main() {
    const float gamma = 2.2;

    // Obtains the HDR colour of the pixel
    vec3 hdrColour = texture(hdrBuffer, HDRTexCoords).rgb * exposure;

    // Performs exposure tone mapping
    vec3 LDRColour = vec3(1.0) - exp(-hdrColour * exposure);
    /*
    // Performs Reinhard tone mapping
    float luminance = max(max(hdrColour.r, hdrColour.g), hdrColour.b);
    vec3 LDRColour = hdrColour / (1.0 + luminance);
    */

    // Performs gamma correction
    LDRColour = pow(LDRColour, vec3(1.0/gamma));

    // Output colour
    fragColor = vec4(LDRColour,1.0);
    //fragColor = vec4(1.0,0.0,0.0,1.0);
}