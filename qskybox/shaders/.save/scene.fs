#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

out vec4 FragColor;

//--- Lights

uniform vec3 viewPos;

uniform vec3  ambientColor;
uniform float ambientStrength;
uniform float diffuseStrength;
uniform vec3  emissionColor;
uniform float emissiveStrength;
uniform vec3  specularColor;
uniform float specularStrength;
uniform float shininess;
uniform bool  fixedDiffuse;

//--- Textures

struct TextureData {
  bool      enabled;
  sampler2D texture;
};

uniform TextureData diffuseTexture;
uniform TextureData normalTexture;
uniform TextureData specularTexture;
uniform TextureData emissiveTexture;

//--- Lights
uniform vec3 lightPos;
uniform vec3 lightDirection;
uniform vec3 lightColor;

//--- State

uniform float transparency;

//--- Shadows
uniform sampler2D shadowMap;
uniform bool      useShadowMap;
uniform float     shadowBias;

//---

vec3 calcNormal() {
  if (normalTexture.enabled) {
    vec3 norm = texture(normalTexture.texture, TexCoords).rgb;
    norm = normalize(norm*2.0 - 1.0); // this normal is in tangent space
    return norm;
  }
  else
    return normalize(Normal);
}

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
  if (emissiveTexture.enabled)
    return texture(emissiveTexture.texture, TexCoords).rgb;
  else
    return emissiveStrength*emissionColor;
}

//---

float shadowCalculation(vec4 fragPosLightSpace) {
  // perform perspective divide
  vec3 projCoords = fragPosLightSpace.xyz/fragPosLightSpace.w;

  // transform to [0,1] range
  projCoords = projCoords*0.5 + 0.5;

  // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
  float closestDepth = texture(shadowMap, projCoords.xy).r;

  // get depth of current fragment from light's perspective
  float currentDepth = projCoords.z;

  // calculate bias (based on depth map resolution and slope)
/*
  vec3 normal = normalize(Normal);
  vec3 lightDir = normalize(lightPos - FragPos);
  float bias = max(0.05*(1.0 - dot(normal, lightDir)), 0.005);
*/
  float bias = shadowBias;
 
  // check whether current frag pos is in shadow
  //float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
  //float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

  // PCF
  float shadow = 0.0;
  vec2 texelSize = 1.0/textureSize(shadowMap, 0);
  for (int x = -1; x <= 1; ++x) {
    for (int y = -1; y <= 1; ++y) {
      float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y)*texelSize).r; 
      shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
    }    
  }
  shadow /= 9.0;

  //float shadow = currentDepth - shadowBias > closestDepth ? 1.0 : 0.0;
  //float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

  return shadow;
}

//---

vec3 calcDirectionalLight(vec3 norm, vec3 diffuseColor, vec3 specColor, vec3 viewDir) {
  float shadow = 0.0;
  if (useShadowMap) {
    shadow = shadowCalculation(FragPosLightSpace);
  }

  float diffFactor = 1.0;

  vec3 lightDir = normalize(-lightDirection);

  // diffuse light color
  if (! fixedDiffuse)
    diffFactor = calcDiffuseFactor(lightDir, norm);

  vec3 result = (1 - shadow)*diffFactor*lightColor*diffuseColor;

  // specular light color
  float specFactor = calcSpecularFactor(lightDir, viewDir, norm, shininess);

  result += (1 - shadow)*specFactor*lightColor*specColor;

  return result;
}

//---

void main() {
  // normal
  vec3 norm = calcNormal();

  // ambient
  vec3 ambient = ambientStrength*ambientColor;

  vec3 result = ambient;

  // diffuse color
  vec3 diffuseColor = calcDiffuseColor();

  // specular color
  vec3 specColor = calcSpecularColor();

  vec3 viewDir = normalize(viewPos - FragPos);

  result += calcDirectionalLight(norm, diffuseColor, specColor, viewDir);

/*
  // baked diffuse lighting
  float diffFactor = 1.0;

  if (! fixedDiffuse)
    diffFactor = max(dot(norm, viewDir), 0.0);

  result += diffFactor*diffuseColor;
*/

  // add emission
  vec3 emissionColor = calcEmissionColor();

  result += emissionColor;

  // adjust color by state

  FragColor = vec4(result, transparency);
}
