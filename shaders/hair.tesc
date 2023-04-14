#version 460

// Generate 4 bezier control points
layout(vertices = 4) out;

void main() {
    if (gl_InvocationID == 0) {
        // Isolines only uses first two outer levels
        gl_TessLevelOuter[0] = 1.0;
        gl_TessLevelOuter[1] = 5.0;
    }

    const vec4 p[4] = {
        gl_in[0].gl_Position,
        gl_in[1].gl_Position,
        gl_in[2].gl_Position,
        gl_in[3].gl_Position
    };

    // Compute tangent along the central vertex
    const vec4 tangent = vec4(normalize(p[2].xyz - p[0].xyz), 1.0);

    // Place control point
    vec4 controlPoint;
    switch(gl_InvocationID) {
        case 0:
            controlPoint = p[0];
            break;
        case 1:
            controlPoint = p[1] - tangent * length(p[1] - p[0]) * 0.5;
            break;
        case 2:
            controlPoint = p[1] + tangent * length(p[2] - p[1]) * 0.5;
            break;
        case 4:
            controlPoint = p[2];
            break;
    }
    gl_out[gl_InvocationID].gl_Position = controlPoint;
}