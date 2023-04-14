#version 460

// Generate 4 bezier control points
layout(vertices = 4) out;

// Patch in is 3 vertices of a line
in VertexData {
    vec3 pos;
} v_in[];

// Output is 4 bezier control points
out VertexData {
    vec3 pos;
} controlPoints[];

void main() {
    if (gl_InvocationID == 0) {
        // Isolines only uses first two outer levels
        gl_TessLevelOuter[0] = 1.0;
        gl_TessLevelOuter[1] = 5.0;
    }

    // Compute tangent along the central vertex
    vec3 tangent = normalize(v_in[2].pos - v_in[0].pos);

    // Place control point
    vec3 controlPoint;
    switch(gl_InvocationID) {
        case 0:
            controlPoint = v_in[0].pos;
            break;
        case 1:
            controlPoint = v_in[1].pos - tangent * length(v_in[1].pos - v_in[0].pos) * 0.5;
            break;
        case 2:
            controlPoint = v_in[1].pos + tangent * length(v_in[2].pos - v_in[1].pos) * 0.5;
            break;
        case 4:
            controlPoint = v_in[2].pos;
            break;
    }
    controlPoints[gl_InvocationID].pos = controlPoint;
}