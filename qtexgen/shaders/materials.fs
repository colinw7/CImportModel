#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
in vec2 TexCoords;

out vec4 FragColor;

//--- Constants
const float PI = 3.14159265359;

//--- Lights

// type: 
//  0 : Directional (direction)
//  1 : Point (position)
//  2 : Spot (position, direction, cutoff, exponent)
struct Light {
  int   type;
  bool  enabled;
  vec3  position;
  vec3  direction;
  vec3  color;
  float radius;
  float cutoff;
  float exponent;
  float attenuation0;
  float attenuation1;
  float attenuation2;
};  
      
#define NUM_LIGHTS 4

uniform Light lights[NUM_LIGHTS];

uniform vec3 viewPos;

uniform vec3  ambientColor;
uniform float ambientStrength;
uniform float diffuseStrength;
uniform vec3  emissionColor;
uniform float emissionStrength;
uniform vec3  specularColor;
uniform float specularStrength;
uniform float shininess;

//--- Textures

struct TextureData {
  bool      enabled;
  sampler2D texture;
};

uniform TextureData diffuseTexture;
uniform TextureData normalTexture;
uniform TextureData specularTexture;
uniform TextureData emissionTexture;
uniform TextureData metallicTexture;
uniform TextureData ORMTexture;

//--- Material

uniform float metallic;
uniform float roughness;
uniform float occlusion;

//---

// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal
// mapping the usual way for performance anyways; I do plan make a note of this
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap() {
  vec3 tangentNormal = texture(normalTexture.texture, TexCoords).xyz*2.0 - 1.0;

  vec3 Q1  = dFdx(FragPos);
  vec3 Q2  = dFdy(FragPos);
  vec2 st1 = dFdx(TexCoords);
  vec2 st2 = dFdy(TexCoords);
  
  vec3 N   = normalize(Normal);
  vec3 T   = normalize(Q1*st2.t - Q2*st1.t);
  vec3 B   = -normalize(cross(N, T));
  mat3 TBN = mat3(T, B, N);
    
  return normalize(TBN*tangentNormal);
}   

vec3 calcNormal() {
  if (normalTexture.enabled) {
    //vec3 norm = texture(normalTexture.texture, TexCoords).rgb;
    //norm = normalize(norm*2.0 - 1.0); // this normal is in tangent space
    //return norm;

    return getNormalFromMap();
  }
  else
    return normalize(Normal);
}

//---

float calcDiffuseFactor(vec3 lightDir, vec3 nrm) {
  float diffAmt = max(dot(nrm, lightDir), 0.0);
  return diffAmt;
}

vec3 calcDiffuseColor() {
  if (diffuseTexture.enabled) {
    vec4 textureColor = texture(diffuseTexture.texture, TexCoords);

    if (textureColor.a < 0.1)
      discard;

    return textureColor.rgb;
  }
  else
    return diffuseStrength*Color;
}

float calcSpecularFactor(vec3 lightDir, vec3 viewDir, vec3 nrm, float shininess) {
  vec3 reflectDir = reflect(-lightDir, nrm);
  float specAmt = max(dot(viewDir, reflectDir), 0.0);
  return pow(specAmt, shininess);
}

vec3 calcSpecularColor() {
  if (specularTexture.enabled)
    return texture(specularTexture.texture, TexCoords).rgb;
  else
    return specularColor*specularStrength;
}

vec3 calcEmissionColor() {
  if (emissionTexture.enabled)
    return texture(emissionTexture.texture, TexCoords).rgb;
  else
    return emissionStrength*emissionColor;
}

float calcMetallic() {
  if (metallicTexture.enabled)
    return texture(metallicTexture.texture, TexCoords).r;
       
  if (ORMTexture.enabled)
    return texture(ORMTexture.texture, TexCoords).b;

  return metallic;
}

float calcRoughness() {
  if (ORMTexture.enabled)
    return texture(ORMTexture.texture, TexCoords).g;

  return roughness;
}

float calcOcclusion() {
  if (ORMTexture.enabled)
    return texture(ORMTexture.texture, TexCoords).r;

  return occlusion;
}

//---

float DistributionGGX(vec3 N, vec3 H, float roughness) {
  float a = roughness*roughness;
  float a2 = a*a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH*NdotH;

  float nom   = a2;
  float denom = (NdotH2*(a2 - 1.0) + 1.0);
  denom = PI*denom*denom;

  return nom/denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
  float r = (roughness + 1.0);
  float k = (r*r)/8.0;

  float nom   = NdotV;
  float denom = NdotV*(1.0 - k) + k;

  return nom/denom;
} 

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx2 = GeometrySchlickGGX(NdotV, roughness);
  float ggx1 = GeometrySchlickGGX(NdotL, roughness);

  return ggx1*ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
  return F0 + (1.0 - F0)*pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
  return F0 + (max(vec3(1.0 - roughness), F0) - F0)*pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

//---

void main() {
  // diffuse color (albedo)
  vec3 albedo = calcDiffuseColor();

  // specular color
  vec3 specColor = calcSpecularColor();

  // emission color
  vec3 emissionColor = calcEmissionColor();

  // metallic
  float metallic = calcMetallic();

  // roughness
  float roughness = calcRoughness();

  // occlusion
  float occlusion = calcOcclusion();

  //---

  // normal
  vec3 norm = calcNormal();

  // view direction
  vec3 viewDir = normalize(viewPos - FragPos);

  // reflect vector
  //vec3 reflectDir = reflect(-viewDir, norm);

  // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
  // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
  vec3 F0 = vec3(0.04);
  F0 = mix(F0, albedo, metallic);

  //---

  vec3 result = vec3(0.0);

  for (int i = 0; i < NUM_LIGHTS; ++i) {
    if (! lights[i].enabled) {
      continue;
    }

    float diffFactor = 1.0;

    if      (lights[i].type == 0) { // directional
      vec3 lightDir = normalize(lights[i].direction);

      // diffuse light color
      diffFactor = calcDiffuseFactor(lightDir, norm);

      result += diffFactor*lights[i].color*albedo;

      // specular light color
      float specFactor = calcSpecularFactor(lightDir, viewDir, norm, shininess);

      result += specColor*lights[i].color*specFactor;
    }
    else if (lights[i].type == 1) { // point
      vec3 toLight    = lights[i].position - FragPos;
      vec3 toLightDir = normalize(toLight);

      // calc falloff
      float falloff = 1.0;

      if (lights[i].radius > 0.0) {
        float lightDist = length(toLight);

        falloff = 1.0/(lights[i].attenuation0 +
                       lightDist*(lights[i].attenuation1 +
                                  lightDist*lights[i].attenuation2));
      //falloff = max(0.0, 1.0 - (lightDist/lights[i].radius));
      //falloff = 1.0/(lightDist*lightDist);
      }

      // diffuse light color
      vec3 radiance = lights[i].color*falloff;

      // Cook-Torrance BRDF
      vec3 H = normalize(viewDir + toLightDir);

      float NDF = DistributionGGX(norm, H, roughness);
      float G   = GeometrySmith(norm, viewDir, toLightDir, roughness);
      vec3 F    = fresnelSchlick(clamp(dot(H, viewDir), 0.0, 1.0), F0);
    //vec3 F    = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);

      vec3 numerator = NDF*G*F;

      // add 0.0001 to prevent divide by zero
      float denominator = 4.0*max(dot(norm, viewDir), 0.0)*
                              max(dot(norm, toLightDir), 0.0) + 0.0001;

      vec3 specular = numerator/denominator;

      // kS is equal to Fresnel
      vec3 kS = F;

      // for energy conservation, the diffuse and specular light can't
      // be above 1.0 (unless the surface emits light); to preserve this
      // relationship the diffuse component (kD) should equal 1.0 - kS.
      vec3 kD = vec3(1.0) - kS;

      // multiply kD by the inverse metalness such that only non-metals
      // have diffuse lighting, or a linear blend if partly metal (pure metals
      // have no diffuse light).
      kD *= 1.0 - metallic;

      // scale light by NdotL
      float NdotL = max(dot(norm, toLightDir), 0.0);

      // add to outgoing radiance result
      // note that we already multiplied the BRDF by the Fresnel (kS)
      // so we won't multiply by kS again
      result += (kD*albedo/PI + specular)*radiance*NdotL;
    }
    else if (lights[i].type == 2) { // spot
      vec3 lightDir = normalize(lights[i].direction);

      vec3 toLight    = lights[i].position - FragPos;
      vec3 toLightDir = normalize(toLight);

      // diffuse light color
      float angle = dot(lightDir, toLightDir);

      if (angle > lights[i].cutoff) {
        float falloff = pow(angle, lights[i].exponent);

        diffFactor = calcDiffuseFactor(toLightDir, norm)*falloff;

        result += diffFactor*lights[i].color*albedo;
      }

      // specular light color
      float specFactor = calcSpecularFactor(toLightDir, viewDir, norm, shininess);

      result += specFactor*lights[i].color*specColor;
    }
  }

  //---

  // add emission 
  result += emissionColor;

  //---

  // add ambient
  //vec3 ambient = ambientStrength*ambientColor;
  vec3 ambient = vec3(ambientStrength)*albedo*occlusion;

  result += ambient;

  //---

  // HDR tonemapping
  result = result/(result + vec3(1.0));
  // gamma correct
  result = pow(result, vec3(1.0/2.2));

  FragColor = vec4(result, 1.0);
}
