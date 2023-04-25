#version 460

#define SQRT_TWO_PI_INV 0.3989422804
#define ROOT_THREE_BY_TWO 0.86602540378

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
uniform int shadingModel;

uniform sampler2D lut0,lut1;

// Marschner LUT parameters
uniform float diffuseFalloff;
uniform float diffuseAzimuthFalloff;
uniform float scaleDiffuse;
uniform float scaleR;
uniform float scaleTT;
uniform float scaleTRT;

// Marschner Procedural parameters
uniform float roughness;
uniform float shift;
uniform float refractiveIndex;
uniform float procScaleR;
uniform float procScaleTT;
uniform float procScaleTRT;

in float sinThetaI,sinThetaR,cosThetaI,cosThetaR;
in float cosPhiD,cosThetaD;
in float cosHalfPhi,lightViewDot;

// ============== UTILITY FUNCTIONS ==============

const vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);


// Return gausssian lobe. beta is based on roughness, alpha is based on the cuticle angle
float gaussian(float alpha,float beta) 
{
    return exp(-0.5 * pow(sinThetaI+sinThetaR-alpha,2) / pow(beta,2)) * SQRT_TWO_PI_INV / beta;
}

float schlickFresnel(float angle)
{
    float reflectionCoeff = pow((1-refractiveIndex)/(1+refractiveIndex),2);
    return reflectionCoeff + (1-reflectionCoeff) * pow(1-angle,5);
}
// ================= SHADOWS ===================

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

// ============== LIGHTING MODELS ==============

void CalculateKajiyaKay(out vec3 shadedColor,float shadowFraction)
{
    vec3 viewSpaceLightDir = normalize((uTView*vec4(light_dir, 0.0)).xyz);
    vec3 viewSpaceTangent = normalize(fTangent);
    float cosL = dot(viewSpaceTangent, viewSpaceLightDir);
    float sinL =  clamp(sqrt(1.0 - cosL * cosL), 0.0, 1.0);
    shadedColor = hair_color.rgb * sinL;
    vec3 viewDirNorm = normalize(viewDir);
    float cosV = dot(viewSpaceTangent, viewDirNorm);
    float sinV = clamp(sqrt(1.0 - cosV * cosV), 0.0, 1.0);
    shadedColor += specular * vec3(pow(max(clamp(cosL,0,1)*clamp(cosV,0,1) + sinL*sinV,0),shininess));
    shadedColor = shadedColor * shadowFraction + hair_color.rgb * sinL * ambient;
}

void CalculateMarschnerLUT(out vec3 shadedColor,float shadowFraction)
{
    vec2 uv0 = vec2( sinThetaI * 0.5 + 0.5, 1.0 - ( sinThetaR * 0.5 + 0.5 ));
	vec4 N = texture( lut0, uv0 );
    vec2 uv1 = vec2( cosPhiD * 0.5 + 0.5, 1.0 - N.a);
    vec4 M = texture(lut1, uv1);

    vec3 viewSpaceLightDir = normalize((uTView*vec4(light_dir, 0.0)).xyz);
    vec3 viewSpaceTangent = normalize(fTangent);
    float cosL = dot(viewSpaceTangent, viewSpaceLightDir);
    float sinL =  clamp(sqrt(1.0 - cosL * cosL), 0.0, 1.0);

    vec3 marschner = vec3( N.r * M.a * scaleR ) + ( N.g * M.rgb * scaleTT ) + (N.b * M.rgb * scaleTRT);
	vec3 diffuse = hair_color.rgb * mix( 1.0, cosThetaI, diffuseFalloff ) * mix( 1.0, cosHalfPhi, diffuseAzimuthFalloff ) * scaleDiffuse * sinL; 
    shadedColor = diffuse*hair_color.rgb + marschner * specular;
    shadedColor = shadedColor * shadowFraction + diffuse * ambient;      
}

// Based on "Physcially Based Hair Shading in Unreal" by Brian Karis
void CalculateMarschnerProcedural(out vec3 shadedColor,float shadowFraction)
{
    shadedColor = vec3(0.0);

    // === Longitudinal scattering lobes ===
    float Mr = gaussian(shift,roughness);
    float Mtt = gaussian(-shift*0.5,roughness*0.5);
    float Mtrt = gaussian(-3 *shift * 0.5,roughness*2);

    // === Azimuthal scattering lobes ===
    float a,h,D,f;
    vec3 T,A;

    // Nr
    f = schlickFresnel(lightViewDot);
    float Nr = 0.25 * cosHalfPhi * f; // Check if lightViewDot should be remapped to 01

    // Ntt
    a = 1.55/ (refractiveIndex * (1.19/cosThetaD + 0.36 * cosThetaD));
    h = (1+a*(0.6-0.8 * cosPhiD)) * cosHalfPhi;
    T = pow(hair_color.rgb,vec3(sqrt(1-h*h*a*a)/2*cosThetaD));
    D = exp(-3.65*cosPhiD-3.98);
    f = schlickFresnel(cosThetaD * sqrt(1-h*h));
    A = pow(1-f,2) * T;
    vec3 Ntt = 0.5 * A * D;

    // Ntrt
    T = pow(hair_color.rgb,vec3(0.8/cosThetaD));
    D = exp(17*cosPhiD-16.78);
    f = schlickFresnel(cosThetaD*0.5);
    A = pow(1-f,2) * f * T * T;
    vec3 Ntrt = 0.5 * A * D;

    // === Final shading ===
    vec3 viewSpaceLightDir = normalize((uTView*vec4(light_dir, 0.0)).xyz);
    vec3 viewSpaceTangent = normalize(fTangent);
    float cosL = dot(viewSpaceTangent, viewSpaceLightDir);
    float sinL =  clamp(sqrt(1.0 - cosL * cosL), 0.0, 1.0);

    vec3 marschner = vec3(Mr * Nr * procScaleR) + Mtt * Ntt * procScaleTT + Mtrt * Ntrt * procScaleTRT;
    vec3 diffuse = hair_color.rgb * mix( 1.0, cosThetaI, diffuseFalloff ) * mix( 1.0, cosHalfPhi, diffuseAzimuthFalloff ) * scaleDiffuse * sinL; 
    shadedColor = diffuse*hair_color.rgb + marschner * specular;
    shadedColor = shadedColor * shadowFraction + diffuse * ambient;     
}

out vec4 fragColor;

void main() 
{
    // Compute opacity value and blend
    float shadowFraction = shadows_enabled ? getOpacity() : 1.0;
    vec3 shadedColor;
    
    if(shadingModel == 0)
        CalculateKajiyaKay(shadedColor,shadowFraction);
    else if(shadingModel == 1)
        CalculateMarschnerLUT(shadedColor,shadowFraction);
    else
        CalculateMarschnerProcedural(shadedColor,shadowFraction);
    
    fragColor = vec4(shadedColor, hair_color.a);
}
