#version 410 core
out vec4 fragColor;

in vec2 mapping;
in vec3 starUniqueColourOut;

/*
What does this shader do, and why does it exist?

Well basically when I originally wrote my shaders I had a single shader which would render both the star core and apply the glow
effect in the same pass, which was not the best idea in retrospect. Basically in order to apply glow I need to get rid of depth testing and
make all pixels blend with one another, so i.e. the glow of several stars close together would add up. This worked well for the glow
but it would result in the core of the star being see through and hazy, so I have separated the drawing into several steps.

1.) The core of the star is rendered in HDR, using depth testing and a depth buffer
2.) The glow effect is added to the star after this, using the depth buffer of the original pass.
3.) (TODO) Post processing
4.) Convert HDR to SDR
*/

void main() {

    // If the distance between the fragment and the core of the star is greater than the radius, then discard it.
    // This ensures it is rendered as a circle, instead of just a funny looking quad.
    if (dot(mapping,mapping) > 1.0) {
        discard;
    }
    fragColor = vec4(starUniqueColourOut*30.0,1.0);
}