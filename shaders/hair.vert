#version 460

in vec3 vPos;
in vec3 vTangent;

uniform mat4 uTModel;
uniform mat4 uTView;
uniform mat4 uTProj;
uniform mat4 uTLight;
uniform mat4 viewInverse;
uniform vec3 light_dir;

out vec3 light_clip_pos;
out vec3 fTangent;
out vec3 viewDir;
out float sinThetaI,sinThetaR,cosPhiD,cosThetaI,cosHalfPhi;

void main()
{
    gl_Position = uTProj * uTView * uTModel * vec4(vPos, 1.0);
    vec4 lcp = uTLight * vec4(vPos, 1.0);
    light_clip_pos = lcp.xyz/lcp.w;

    fTangent = (uTView * vec4(vTangent,0.0)).xyz;
    viewDir = -(uTView * vec4(vPos,1.0)).xyz;

    vec3 lightDirTangentSpace = normalize((viewInverse * vec4(-light_dir,1.0)).xyz-vPos);
    vec3 viewDirTangetSpace = normalize((viewInverse * vec4(0,0,0,1.0)).xyz-vPos);

    sinThetaI = dot(lightDirTangentSpace, fTangent);
    sinThetaR = dot(viewDirTangetSpace, fTangent);

    vec3 lightDirPerp = lightDirTangentSpace - sinThetaI * fTangent;
    vec3 viewDirPerp = viewDirTangetSpace - sinThetaR * fTangent;

    cosPhiD = dot(lightDirPerp, viewDirPerp)*pow(dot(lightDirPerp, lightDirPerp) * dot(viewDirPerp, viewDirPerp),-0.5);

    cosThetaI = 1 - sinThetaI * sinThetaI;
    cosHalfPhi = cos(acos(cosPhiD) / 2.0);
}
