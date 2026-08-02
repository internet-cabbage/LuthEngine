#version 410 core
out vec4 fragColor;

in vec2 mapping;

uniform vec3 starColour;
uniform float glowScale;

void main() {
    // If the distance between the fragment and the centre of the square is greater than 1
    // the fragment is outside the circle, so is discarded
    // This trims the corners of the quad so that the glow only extends outwards in a circle
    float distSquared = dot(mapping,mapping);
    float glowSquared = glowScale * glowScale;
    if (distSquared > glowSquared) {
        discard;
    }
    // Ensure the core is rendered solid
    float glow = 1.0;
    if (distSquared > 1.0) {
        glow = 1.0/((1.0 * distSquared) + 1.0);
        // Now we need a falloff function so that the brightness decreases to exactly zero at the boundary
        float falloff = 1.0 - (distSquared/glowSquared);
        glow *= falloff;
    }

    fragColor = vec4(starColour*glow, 1.0);

}