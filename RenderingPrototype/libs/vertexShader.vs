#version 410 core

layout(location = 0) in vec3 starPos; // World space position of the star

uniform mat4 view; // View matrix to convert camera coords to camera coords
uniform mat4 projection; // Converts view coordinates to screen coordinates, by projecting them onto the screen.
uniform float radius;
uniform float glowScale; // How far the glow should extend outwards from the star as a multiple of the star's radius

out vec2 mapping;

/*  The mapping variable is a way of linking the index of the vertex being processed, to the
    position it will be at on screen. For our quad shape, it has the values (0,1,2,3), giving us coordinates of:
    (-1,-1), (-1,1), (1,-1), (1,1)

*/
void main() {
    // The centre of the star quad is obtained by changing frame from the world space to the view space.
    vec3 centre = (view * vec4(starPos,1.0)).xyz;


    // Which corner of the quad this vertice is on
    vec2 quadCorner = vec2(float(gl_VertexID / 2), float(gl_VertexID % 2)) * 2.0 - 1.0;

    mapping = quadCorner * glowScale;
    vec3 corner = centre;
    corner.xy += mapping * radius; // I have finally been able to make use of swizzling 😍

    gl_Position = projection * vec4(corner, 1.0f);
}