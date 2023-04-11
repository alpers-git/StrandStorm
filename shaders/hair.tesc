#version 460

// Generate 4 bezier control points
layout(vertices = 4) out;

// Patch in is 3 vertices of a line
in VertexData {
    vec3 pos;
} v_in[];

out VertexData {
    vec3 pos;
} v_out[];

void main() {
    if (gl_InvocationID == 0) {
        // Isolines only uses first two outer levels
        gl_TessLevelOuter[0] = 1.0;
        gl_TessLevelOuter[1] = 5.0;
    }

    v_out[gl_InvocationID].pos = v_in[gl_InvocationID].pos;

    // Compute tangent of the central vertex
    
}