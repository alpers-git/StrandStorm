#version 460

layout(isolines, equal_spacing, ccw) in;

// Evaluate bezier curve at t
vec4 bezier(float t) {
    float t2 = t * t;
    float t3 = t2 * t;
    float mt = 1.0 - t;
    float mt2 = mt * mt;
    float mt3 = mt2 * mt;
    return mt3 * gl_in[0].gl_Position +
        3.0 * mt2 * t * gl_in[1].gl_Position +
        3.0 * mt * t2 * gl_in[2].gl_Position +
        t3 * gl_in[3].gl_Position;
}

uniform mat4 uTProj;
uniform mat4 uTView;
uniform mat4 uTModel;

void main() {
    // gl_TessCoord.x is distance along each isoline, [0,1]
    gl_Position = uTProj * uTView * uTModel * bezier(gl_TessCoord.x);
}