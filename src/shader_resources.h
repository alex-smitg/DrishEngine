#pragma once
#include <map>
#include <string>
std::map<std::string, std::string> shaders = {
{"emission.fragment",R"(#version 330 core
out vec4 FragColor;

in vec3 TexCoords;


uniform vec3 color;
uniform float strenght;

void main()
{    
    FragColor = vec4(color, 1.0) * strenght;
})"},{"emission.vertex",R"(#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    TexCoords = aPos;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}  )"},{"screen.fragment",R"(#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300.0;  

uniform float Directions;
uniform float Size;
uniform float Quality;


void main()
{
    
    
    vec4 col = texture(screenTexture, TexCoords);

    vec4 BrightColor;

    float brightness = dot(col.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 0.5)
        BrightColor = vec4(col.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);


    float Pi = 6.28318530718;
    

   
    vec2 Radius = Size/TexCoords;
    
    vec4 Color = texture(screenTexture, TexCoords);
    

    for( float d=0.0; d<Pi; d+=Pi/Directions)
    {
		for(float i=1.0/Quality; i<=1.0; i+=1.0/Quality)
        {
			Color += texture(screenTexture, TexCoords+vec2(-cos(d),sin(d))*Radius*i);		
        }
    }
    

    Color /= Quality * Directions - 15.0;

    FragColor = Color + texture(screenTexture, TexCoords);;
}  )"},{"screen.vertex",R"(#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
    TexCoords = aTexCoords;
}  )"},{"sky.fragment",R"(#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

uniform float value;

void main()
{    
    FragColor = texture(skybox, TexCoords) * value;
})"},{"sky.vertex",R"(#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    gl_Position = projection * view * vec4(aPos, 1.0);
}  )"},{"space.fragment",R"(#version 330 core
out vec4 FragColor;
  
in mat4 matrix;

void main()
{
    FragColor = vec4(matrix[3].x, matrix[3].y, matrix[3].z, 1.0);
})"},{"space.vertex",R"(#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in mat4 instanceMatrix;


uniform mat4 projection;
uniform mat4 view;

out mat4 matrix;

void main()
{
    gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0); 

    matrix = instanceMatrix;
})"},{"standart.fragment",R"(#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in mat3 TBN;

uniform bool use_normal;
uniform bool use_specular;
uniform bool use_diffuse;
uniform bool use_blinn;
uniform bool emit;

uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 diffuse_color;

uniform float shine;
uniform float reflection;
uniform float spec_factor;
uniform float ref_ratio;
uniform float normal_bump;
uniform float dif;


uniform vec2 uv_scale;

uniform float sky_val;




uniform sampler2D oText;
uniform sampler2D sText;
uniform sampler2D nText;
uniform samplerCube skybox;

struct PointLight {    
    vec3 position;
    vec3 color;

    float strength;
    float constant;
    float radius;
};

uniform int light_number;

#define NR_POINT_LIGHTS 64
uniform PointLight pointLights[NR_POINT_LIGHTS];


vec4 calculatePointLight(PointLight light, vec3 norm, vec3 FragPos, vec3 viewDir, vec2 uv) 
{
    float distance = length(light.position - FragPos);
    float attenuation = smoothstep(light.radius, 0, distance);

    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 diffuse;

    if (use_diffuse) {
        diffuse = diff * texture(oText, vec2(uv.x, 1.0 - uv.y)).rgb * light.color * dif * diffuse_color;
    } else {
        diffuse = diff * light.color * dif * diffuse_color;
    }
    
    float nDot = dot(norm, lightDir);

    float spec;

    if (use_blinn) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(norm, halfwayDir), 0.0), shine);
    } else {
         vec3 reflectDir = reflect(lightDir, norm);
        spec = pow(max(dot(viewDir, -reflectDir), 0.0), shine);
    }
 
    vec3 specular = vec3(0.0, 0.0, 0.0);

    if (nDot > 0) {
        if(use_specular) {
            specular = spec * light.color * vec3(texture(sText, vec2(uv.x, 1.0 - uv.y))).r * spec_factor * light.color * nDot;
        } else {
            specular = spec * light.color * spec_factor * light.color * nDot;
        }
        
    }
    diffuse *= light.strength * attenuation;
    specular *= light.strength * attenuation;
    vec3 result = (diffuse + specular);
    return vec4(result, 1.0) ;
}






void main()
{
   
    vec2 new_TexCoord = uv_scale * TexCoord;

    vec3 norm = texture(nText, vec2(new_TexCoord.x, 1.0 - new_TexCoord.y)).rgb ;
    norm = norm * 2.0 - 1.0;
    norm.xy *= normal_bump;
    norm = normalize(TBN * norm);
    if(normal_bump < 0.01 || use_normal == false) {
        norm = normalize(Normal);
    }
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0, 0.0, 0.0);

    vec3 ambient = lightColor * sky_val;
    ambient  = ambient * texture(oText, vec2(new_TexCoord.x, 1.0 - new_TexCoord.y)).rgb;

    for(int i = 0; i < light_number; i++)
        result += calculatePointLight(pointLights[i], norm, FragPos, viewDir, new_TexCoord).rgb;    

    vec3 R = refract(-viewDir, normalize(-norm), ref_ratio);
    vec3 refl = texture(skybox, R).rgb;
    result += refl * reflection * sky_val;
    result += ambient;
 

    if (emit) {
        result =  vec3(diffuse_color);
    }
    FragColor = vec4(result, 1.0);
})"},{"standart.vertex",R"(#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

out vec3 ourColor;
out vec2 TexCoord;

out vec3 FragPos;
out vec3 Normal;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{

    mat3 modelVector = mat3(transpose(inverse(model)));

    vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
    TBN = mat3(T, B, N);


    FragPos = vec3(model * vec4(position, 1.0));
    Normal = modelVector * aNormal;

    gl_Position = projection * view * vec4(FragPos, 1.0);

    TexCoord = aTexCoord;
})"},{"standart_test.fragment",R"(#version 330 core

in vec2 TexCoord; // texture coords
in vec3 Normal;   // normal
in mat3 TBN; // binormal (for TBN basis calc)
in vec3 FragPos;      // pixel view space position

out vec4 color;

layout(std140) uniform Transforms
{
    mat4x4 world_matrix;  // object's world position
    mat4x4 view_matrix;   // view (camera) transform
    mat4x4 proj_matrix;   // projection matrix
    mat3x3 normal_matrix; // normal transformation matrix ( transpose(inverse(W * V)) )
};

layout(std140) uniform Material
{
    vec4 material; // x - metallic, y - roughness, w - "rim" lighting
    vec4 albedo;   // constant albedo color, used when textures are off
};

uniform samplerCube envd;  // prefiltered env cubemap
uniform sampler2D tex;     // base texture (albedo)
uniform sampler2D norm;    // normal map
uniform sampler2D spec;    // "factors" texture (G channel used as roughness)
uniform sampler2D iblbrdf; // IBL BRDF normalization precalculated tex

   
#define PI 3.1415926


// constant light position, only one light source for testing (treated as point light)
const vec4 light_pos = vec4(-2, 3, -2, 1);


// handy value clamping to 0 - 1 range
float saturate(in float value)
{
    return clamp(value, 0.0, 1.0);
}


// phong (lambertian) diffuse term
float phong_diffuse()
{
    return (1.0 / PI);
}


// compute fresnel specular factor for given base specular and product
// product could be NdV or VdH depending on used technique
vec3 fresnel_factor(in vec3 f0, in float product)
{
    return mix(f0, vec3(1.0), pow(1.01 - product, 5.0));
}


// following functions are copies of UE4
// for computing cook-torrance specular lighting terms

float D_blinn(in float roughness, in float NdH)
{
    float m = roughness * roughness;
    float m2 = m * m;
    float n = 2.0 / m2 - 2.0;
    return (n + 2.0) / (2.0 * PI) * pow(NdH, n);
}

float D_beckmann(in float roughness, in float NdH)
{
    float m = roughness * roughness;
    float m2 = m * m;
    float NdH2 = NdH * NdH;
    return exp((NdH2 - 1.0) / (m2 * NdH2)) / (PI * m2 * NdH2 * NdH2);
}

float D_GGX(in float roughness, in float NdH)
{
    float m = roughness * roughness;
    float m2 = m * m;
    float d = (NdH * m2 - NdH) * NdH + 1.0;
    return m2 / (PI * d * d);
}

float G_schlick(in float roughness, in float NdV, in float NdL)
{
    float k = roughness * roughness * 0.5;
    float V = NdV * (1.0 - k) + k;
    float L = NdL * (1.0 - k) + k;
    return 0.25 / (V * L);
}


// simple phong specular calculation with normalization
vec3 phong_specular(in vec3 V, in vec3 L, in vec3 N, in vec3 specular, in float roughness)
{
    vec3 R = reflect(-L, N);
    float spec = max(0.0, dot(V, R));

    float k = 1.999 / (roughness * roughness);

    return min(1.0, 3.0 * 0.0398 * k) * pow(spec, min(10000.0, k)) * specular;
}

// simple blinn specular calculation with normalization
vec3 blinn_specular(in float NdH, in vec3 specular, in float roughness)
{
    float k = 1.999 / (roughness * roughness);
    
    return min(1.0, 3.0 * 0.0398 * k) * pow(NdH, min(10000.0, k)) * specular;
}

// cook-torrance specular calculation                      
vec3 cooktorrance_specular(in float NdL, in float NdV, in float NdH, in vec3 specular, in float roughness)
{
#ifdef COOK_BLINN
    float D = D_blinn(roughness, NdH);
#endif

#ifdef COOK_BECKMANN
    float D = D_beckmann(roughness, NdH);
#endif

#ifdef COOK_GGX
    float D = D_GGX(roughness, NdH);
#endif

    float G = G_schlick(roughness, NdV, NdL);

    float rim = mix(1.0 - roughness * material.w * 0.9, 1.0, NdV);

    return (1.0 / rim) * specular * G * D;
}

                      
void main() {
    // point light direction to point in view space
    vec3 local_light_pos = (view_matrix * (/*world_matrix */ light_pos)).xyz;

    // light attenuation
    float A = 20.0 / dot(local_light_pos - FragPos, local_light_pos - FragPos);

    // L, V, H vectors
    vec3 L = normalize(local_light_pos - FragPos);
    vec3 V = normalize(-FragPos);
    vec3 H = normalize(L + V);
    vec3 nn = normalize(Normal);

    //vec3 nb = normalize(v_binormal);
    //mat3x3 tbn = mat3x3(nb, cross(nn, nb), nn);
    mat3x3 tbn = TBN


    vec2 texcoord = TexCoord;


    // normal map
#if USE_NORMAL_MAP
    // tbn basis
    vec3 N = tbn * (texture2D(norm, texcoord).xyz * 2.0 - 1.0);
#else
    vec3 N = nn;
#endif

    // albedo/specular base
#if USE_ALBEDO_MAP
    vec3 base = texture2D(tex, texcoord).xyz;
#else
    vec3 base = albedo.xyz;
#endif

    // roughness
#if USE_ROUGHNESS_MAP
    float roughness = texture2D(spec, texcoord).y * material.y;
#else
    float roughness = material.y;
#endif

    // material params
    float metallic = material.x;

    // mix between metal and non-metal material, for non-metal
    // constant base specular factor of 0.04 grey is used
    vec3 specular = mix(vec3(0.04), base, metallic);

    // diffuse IBL term
    //    I know that my IBL cubemap has diffuse pre-integrated value in 10th MIP level
    //    actually level selection should be tweakable or from separate diffuse cubemap
    mat3x3 tnrm = transpose(normal_matrix);
    vec3 envdiff = textureCubeLod(envd, tnrm * N, 10).xyz;

    // specular IBL term
    //    11 magic number is total MIP levels in cubemap, this is simplest way for picking
    //    MIP level from roughness value (but it's not correct, however it looks fine)
    vec3 refl = tnrm * reflect(-V, N);
    vec3 envspec = textureCubeLod(
        envd, refl, max(roughness * 11.0, textureQueryLod(envd, refl).y)
    ).xyz;

    // compute material reflectance

    float NdL = max(0.0, dot(N, L));
    float NdV = max(0.001, dot(N, V));
    float NdH = max(0.001, dot(N, H));
    float HdV = max(0.001, dot(H, V));
    float LdV = max(0.001, dot(L, V));

    // fresnel term is common for any, except phong
    // so it will be calcuated inside ifdefs


#ifdef PHONG
    // specular reflectance with PHONG
    vec3 specfresnel = fresnel_factor(specular, NdV);
    vec3 specref = phong_specular(V, L, N, specfresnel, roughness);
#endif

#ifdef BLINN
    // specular reflectance with BLINN
    vec3 specfresnel = fresnel_factor(specular, HdV);
    vec3 specref = blinn_specular(NdH, specfresnel, roughness);
#endif

#ifdef COOK
    // specular reflectance with COOK-TORRANCE
    vec3 specfresnel = fresnel_factor(specular, HdV);
    vec3 specref = cooktorrance_specular(NdL, NdV, NdH, specfresnel, roughness);
#endif

    specref *= vec3(NdL);

    // diffuse is common for any model
    vec3 diffref = (vec3(1.0) - specfresnel) * phong_diffuse() * NdL;

    
    // compute lighting
    vec3 reflected_light = vec3(0);
    vec3 diffuse_light = vec3(0); // initial value == constant ambient light

    // point light
    vec3 light_color = vec3(1.0) * A;
    reflected_light += specref * light_color;
    diffuse_light += diffref * light_color;

    // IBL lighting
    vec2 brdf = texture2D(iblbrdf, vec2(roughness, 1.0 - NdV)).xy;
    vec3 iblspec = min(vec3(0.99), fresnel_factor(specular, NdV) * brdf.x + brdf.y);
    reflected_light += iblspec * envspec;
    diffuse_light += envdiff * (1.0 / PI);

    // final result
    vec3 result =
        diffuse_light * mix(base, vec3(0.0), metallic) +
        reflected_light;

    color = vec4(result, 1);
})"},};