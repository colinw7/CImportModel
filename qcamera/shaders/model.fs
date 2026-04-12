#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

out vec4 FragColor;

//--- Lights

// type:
//  0 : Directional (direction)
//  1 : Point (position)
//  2 : Spot (position, direction, cutoff, outer cutoff, exponent)
//  3 : FlashLight (cutoff, outer cutoff, exponent)
struct Light {
  int   type;
  bool  enabled;
  vec3  position;
  vec3  direction;
  vec3  color;
  float radius;
  float cutoff;
  float outerCutoff;
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
uniform float emissiveStrength;
uniform vec3  specularColor;
uniform float specularStrength;
uniform float shininess;
uniform bool  fixedDiffuse;

//--- Shadows
uniform sampler2D shadowMap;
uniform bool      useShadowMap;

//--- Textures

struct TextureData {
  bool      enabled;
  sampler2D texture;
};

uniform TextureData diffuseTexture;
uniform TextureData normalTexture;
uniform TextureData specularTexture;
uniform TextureData emissiveTexture;

//--- State

uniform bool isSelected;
uniform bool isWireframe;

uniform float transparency;

uniform vec3  selectColor;
uniform vec3  wireframeColor;
uniform float wireframeTransparency;

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

float calcSpecularFactor(vec3 lightDir, vec3 viewDir, vec3 normal, float shininess) {
  vec3 reflectDir = reflect(-lightDir, normal);
  float specAmt = max(dot(viewDir, reflectDir), 0.0);
  return pow(specAmt, shininess);
}

float calcBlinnSpecularFactor(vec3 lightDir, vec3 viewDir, vec3 normal, float shininess) {
  vec3 halfwayDir = normalize(lightDir + viewDir);
  float specAmt = max(dot(normal, halfwayDir), 0.0);
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

  float bias = 0.00001;

  // check whether current frag pos is in shadow
  float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
  //float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

  return shadow;
}

//---

vec3 calcDirectionalLight(int i, vec3 norm, vec3 diffuseColor, vec3 specColor, vec3 viewDir) {
  float shadow = 0.0;
  if (useShadowMap) {
    shadow = shadowCalculation(FragPosLightSpace);
  }

  float diffFactor = 1.0;

  vec3 lightDir = normalize(-lights[i].direction);

  // diffuse light color
  if (! fixedDiffuse)
    diffFactor = calcDiffuseFactor(lightDir, norm);

  vec3 result = (1 - shadow)*diffFactor*lights[i].color*diffuseColor;

  // specular light color
  float specFactor = calcSpecularFactor(lightDir, viewDir, norm, shininess);

  result += (1 - shadow)*specFactor*lights[i].color*specColor;

  return result;
}

vec3 calcPointLight(int i, vec3 norm, vec3 diffuseColor, vec3 specColor, vec3 viewDir) {
  vec3 toLight    = lights[i].position - FragPos;
  vec3 toLightDir = normalize(toLight);

  // diffuse light color
  float falloff = 1.0;

  if (lights[i].radius > 0.0) {
    float dist = length(toLight);

    falloff = 1.0/(lights[i].attenuation0 +
                   dist*(lights[i].attenuation1 +
                         dist*lights[i].attenuation2));
    //falloff = max(0.0, 1.0 - (dist/lights[i].radius));
  }

  float diffFactor = 1.0;

  if (! fixedDiffuse)
    diffFactor = calcDiffuseFactor(toLightDir, norm)*falloff;

  vec3 result = diffFactor*lights[i].color*diffuseColor;

  // specular light color
  //float specFactor = calcSpecularFactor(toLightDir, viewDir, norm, shininess);
  float specFactor = calcBlinnSpecularFactor(toLightDir, viewDir, norm, shininess);

  result += specFactor*lights[i].color*specColor;

  return result;
}

vec3 calcSpotLight(int i, vec3 norm, vec3 diffuseColor, vec3 specColor, vec3 viewDir) {
  vec3 toLight    = lights[i].position - FragPos;
  vec3 toLightDir = normalize(toLight);

  float diffFactor = 1.0;

  if (! fixedDiffuse) {
    vec3 lightDir = normalize(-lights[i].direction);

    // diffuse light color
    float angle = max(dot(toLightDir, lightDir), 0.0);

    // cos(0) = 1 (parallel), cos(90) = 0 (perp)
    // cutoff is cos(angle) so inside if greater

    float falloff = 0.0;
    if (lights[i].outerCutoff < lights[i].cutoff) {
      float epsilon = lights[i].cutoff - lights[i].outerCutoff;
      falloff = clamp((angle - lights[i].outerCutoff)/epsilon, 0.0, 1.0);
    } else {
      falloff = clamp(angle/lights[i].cutoff, 0.0, 1.0);
    }
    falloff = pow(falloff, lights[i].exponent);

    //forceColor = vec4(falloff, falloff, falloff, 1.0);

    //if (angle > lights[i].cutoff) {
    //  falloff = pow(angle, lights[i].exponent);
    //}

    diffFactor = calcDiffuseFactor(toLightDir, norm)*falloff;
  }

  vec3 result = diffFactor*lights[i].color*diffuseColor;

  // specular light color
  //float specFactor = calcSpecularFactor(toLightDir, viewDir, norm, shininess);
  float specFactor = calcBlinnSpecularFactor(toLightDir, viewDir, norm, shininess);

  result += specFactor*lights[i].color*specColor;

  return result;
}

vec3 calcFlashLight(int i, vec3 norm, vec3 diffuseColor, vec3 specColor, vec3 viewDir) {
  vec3 toLight    = viewPos - FragPos;
  vec3 toLightDir = normalize(toLight);

  float diffFactor = 1.0;

  if (! fixedDiffuse) {
    // diffuse light color
    float angle = max(dot(toLightDir, norm), 0.0);

    // cos(0) = 1 (parallel), cos(90) = 0 (perp)
    // cutoff is cos(angle) so inside if greater

    float falloff = 0.0;
    if (lights[i].outerCutoff < lights[i].cutoff) {
      float epsilon = lights[i].cutoff - lights[i].outerCutoff;
      falloff = clamp((angle - lights[i].outerCutoff)/epsilon, 0.0, 1.0);
    } else {
      falloff = clamp(angle/lights[i].cutoff, 0.0, 1.0);
    }
    falloff = pow(falloff, lights[i].exponent);

    //if (angle > lights[i].cutoff) {
    //  falloff = pow(angle, lights[i].exponent);
    //}

    diffFactor = calcDiffuseFactor(toLightDir, norm)*falloff;
  }

  vec3 result = diffFactor*lights[i].color*diffuseColor;

  // specular light color
  //float specFactor = calcSpecularFactor(toLightDir, viewDir, norm, shininess);
  float specFactor = calcBlinnSpecularFactor(toLightDir, viewDir, norm, shininess);

  result += specFactor*lights[i].color*specColor;

  return result;
}

//---

void main() {
  vec4 forceColor = vec4(0, 0, 0, 0);

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

  bool lighted = false;

  for (int i = 0; i < NUM_LIGHTS; ++i) {
    if (! lights[i].enabled) {
      continue;
    }

    lighted = true;

    if      (lights[i].type == 0) { // directional
      result += calcDirectionalLight(i, norm, diffuseColor, specColor, viewDir);
    }
    else if (lights[i].type == 1) { // point
      result += calcPointLight(i, norm, diffuseColor, specColor, viewDir);
    }
    else if (lights[i].type == 2) { // spot
      result += calcSpotLight(i, norm, diffuseColor, specColor, viewDir);
    }
    else if (lights[i].type == 3) { // flashlight
      result += calcFlashLight(i, norm, diffuseColor, specColor, viewDir);
    }
  }

  // baked diffuse lighting if none
  if (! lighted) {
    float diffFactor = 1.0;

    if (! fixedDiffuse)
      diffFactor = max(dot(norm, viewDir), 0.0);

    result += diffFactor*diffuseColor;
  }

  // add emission
  vec3 emissionColor = calcEmissionColor();

  result += emissionColor;

  // adjust color by state

  if (! isWireframe) {
    if (! isSelected) {
      FragColor = vec4(result, transparency);
    } else {
      vec3 selectColor1 = mix(result, selectColor, 0.4);

      FragColor = vec4(selectColor1, 1.0);
    }
  } else {
    if (! isSelected) {
      FragColor = vec4(wireframeColor, wireframeTransparency);
    } else {
      FragColor = vec4(selectColor, 1.0);
    }
  }

  if (forceColor.a > 0)
    FragColor = forceColor;
}
