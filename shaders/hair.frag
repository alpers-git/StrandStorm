#version 460

in vec3 light_clip_pos;
in vec3 fTangent;
in vec3 viewDir;

uniform sampler2D depthMap;
uniform sampler2D opacityMaps;
uniform float dk;
uniform bool shadows_enabled;

uniform mat4 uTView;
uniform vec3 light_dir;
uniform vec3 ambient;

uniform vec4 hair_color;
uniform vec3 specular;
uniform float shininess;

uniform sampler2D lut0,lut1;


uniform float diffuseFalloff;
uniform float diffuseAzimuthFalloff;

uniform float scaleDiffuse; //Strength of the 'fake' diffuse shading
uniform float scaleR;
uniform float scaleTT;
uniform float scaleTRT;

uniform vec2 resolution;

in float sinThetaI,sinThetaR,cosPhiD,cosThetaI,cosHalfPhi;

const vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

float getOpacity() {
    float absorption = 0.0;
    for(int disk = 0; disk < 4; disk++)
    {
        // Depth value at hair surface
        float surfDepth = texture(depthMap, light_clip_pos.xy + vec2(poissonDisk[disk]/700)).r;
        // Depth relative to surface (value between 0.0 and 3.0)
        float relDepth = clamp((light_clip_pos.z - surfDepth) / dk, 0.0, 3.0);
        // Opacity values
        vec4 op = texture(opacityMaps, light_clip_pos.xy  + vec2(poissonDisk[disk]/700));
        float O[5] = {0.0, op.r, op.g, op.b, op.a};
        for (int i = 1; i < 5; i++)
            O[i] += O[i - 1];
        float t = clamp(relDepth - floor(relDepth), 0.0, 1.0);
         // Opacity map layer
        int curLayers[2] = {int(floor(relDepth)), int(ceil(relDepth))};
        absorption += clamp(mix(O[curLayers[0]], O[curLayers[1]], t), 0.0, 1.0);
    }
    return 1.0 - (absorption / 4.0);
}

void CalculateKajiyaKay(out vec3 shadedColor,float shadowFraction)
{
    vec3 viewSpaceLightDir = normalize((uTView*vec4(light_dir, 0.0)).xyz);
    vec3 viewSpaceTangent = normalize(fTangent);
    float cosL = dot(viewSpaceTangent, viewSpaceLightDir);
    float sinL = clamp(sqrt(1.0 - cosL * cosL), 0.0, 1.0);
    shadedColor = hair_color.rgb * sinL;
    vec3 viewDirNorm = normalize(viewDir);
    float cosV = dot(viewSpaceTangent, viewDirNorm);
    float sinV = clamp(sqrt(1.0 - cosV * cosV), 0.0, 1.0);
    shadedColor += specular * vec3(pow(max(clamp(cosL,0,1)*clamp(cosV,0,1) + sinL*sinV,0),shininess));
    shadedColor = shadedColor * shadowFraction + hair_color.rgb * sinL * ambient;
}

void CalculateMarschner(out vec3 shadedColor,float shadowFraction)
{
    vec2 uv0 = vec2( sinThetaI * 0.5 + 0.5, 1.0 - ( sinThetaR * 0.5 + 0.5 ));\
    vec4 scales = vec4( 30.0, 30.0, 30.0, 1.0 );
	vec4 comps1 = texture( lut0, uv0 ) * scales;
    vec2 uv1 = vec2( cosPhiD * 0.5 + 0.5, 1.0 - comps1.a);
    vec4 comps2 = texture(lut1, uv1);
    vec3 marschner = vec3( comps1.x * comps2.a * scaleR ) + ( comps1.y * comps2.xyz * scaleTT ) + (comps1.z * comps2.xyz * scaleTRT);
	vec3 diffuse = hair_color.rgb * mix( 1.0, cosThetaI, diffuseFalloff ) * mix( 1.0, cosHalfPhi, diffuseAzimuthFalloff ) * scaleDiffuse; 
    shadedColor = diffuse*hair_color.rgb + marschner * specular;
    shadedColor = shadedColor * shadowFraction + diffuse*hair_color.rgb * ambient;  
    
}

out vec4 fragColor;

void main() 
{
    // Compute opacity value and blend
    float shadowFraction = shadows_enabled ? getOpacity() : 1.0;
    vec3 shadedColor;
  
    // CalculateKajiyaKay(shadedColor,shadowFraction);
    CalculateMarschner(shadedColor,shadowFraction);
    
    fragColor = vec4(shadedColor, hair_color.a);
}
