#version 410 core
out vec4 fragColor;

in vec2 mapping;
in vec3 starUniqueColourOut;
uniform float glowScale;

void main() {
    float glow;
    float x = dot(mapping,mapping);
    float glowSquared = glowScale * glowScale;
    if (x > glowSquared) {
        discard;
    }
    /*
    if (x > 1.0) {
        // The primary function determining the shape of the intensity profile
        glow = 10.0/(2.0*x + 5.0);
        // The function which causes the glow to tend towards zero as it approaches the end of the quad
        float smoothBorder = 1.0 - (x/(glowSquared));
        glow *= smoothBorder; 
    }
    */
    if (x > 1.0) {
        glow = 20.0/(2.0*x + 10.0);
        // The function which causes the glow to tend towards zero as it approaches the end of the quad
        float smoothBorder = 1.0 - (x/(glowSquared));
        glow *= smoothBorder;         
    }
    else {
        // The fragment is inside the star core so we discard it
        discard;
    }

    fragColor = vec4(starUniqueColourOut * glow, 1.0);
}