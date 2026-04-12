#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
in vec2 TexCoords;

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

//--- State

uniform float transparency;

//--- Shadows
in  vec4 FragPosLightSpace; // for directional shadow
out vec4 ShadowColor;       // for cube map shadow debug

uniform float shadowBias;
uniform float far_plane;

// Directional Shadow
uniform sampler2D shadowMap;
uniform bool      useShadowMap;

// Point Cube Map Shadow
uniform samplerCube shadowCubeMap;
uniform bool        useShadowCubeMap;
uniform bool        shadowMapDebug;

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

float shadowCubeMapCalculation(vec3 fragPos, vec3 lightPos) {
  // get vector between fragment position and light position
  vec3 fragToLight = fragPos - lightPos;

  // use the fragment to light vector to sample from the depth map
  float closestDepth = texture(shadowCubeMap, fragToLight).r;
  //float closestDepth = 0.0;

  // it is currently in linear range between [0,1],
  // let's re-transform it back to original depth value
  closestDepth *= far_plane;

  // now get current linear depth as the length between the fragment and light position
  float currentDepth = length(fragToLight);

  // test for shadows
  // we use a much larger bias since depth is now in [near_plane, far_plane] range
//float bias = 0.05;
  float bias = shadowBias;

  float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

  // display closestDepth as debug (to visualize depth cubemap)
  ShadowColor = vec4(vec3(closestDepth/far_plane), 1.0);

  return shadow;
}

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
  float shadow = 0.0;
  if (useShadowCubeMap) {
    shadow = shadowCubeMapCalculation(FragPos, lights[i].position);
    //return vec3(shadow, shadow, shadow);
    if (shadow < 0) return vec3(1, 0, 0);
    if (shadow > 1) return vec3(0, 1, 0);
  }

  vec3 toLight    = lights[i].position - FragPos;
  vec3 toLightDir = normalize(toLight); 

  // diffuse light color
  float falloff = 1.0;

  if (lights[i].radius > 0.0) {
    float dist = length(toLight);

    /*
    falloff = 1.0/(lights[i].attenuation0 +
                   dist*(lights[i].attenuation1 +
                         dist*lights[i].attenuation2));
    */
    falloff = max(0.0, 1.0 - (dist/lights[i].radius));
  }

  float diffFactor = 1.0;

  if (! fixedDiffuse)
    diffFactor = calcDiffuseFactor(toLightDir, norm)*falloff;

  vec3 result = diffFactor*lights[i].color*diffuseColor;

  // specular light color
  float specFactor = calcSpecularFactor(toLightDir, viewDir, norm, shininess);

  result += specFactor*lights[i].color*specColor;

  result = (1 - shadow)*result;

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
  float specFactor = calcSpecularFactor(toLightDir, viewDir, norm, shininess);

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
  float specFactor = calcSpecularFactor(toLightDir, viewDir, norm, shininess);

  result += specFactor*lights[i].color*specColor;

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

  vec3 diffuse;

  if      (lights[0].type == 0) { // directional
    diffuse = calcDirectionalLight(0, norm, diffuseColor, specColor, viewDir);

    result += diffuse;
  }
  else if (lights[0].type == 1) { // point
    diffuse = calcPointLight(0, norm, diffuseColor, specColor, viewDir);

    result += diffuse;
  }
  else if (lights[0].type == 2) { // spot
    diffuse = calcSpotLight(0, norm, diffuseColor, specColor, viewDir);

    result += diffuse;
  }
  else if (lights[0].type == 3) { // flashlight
    diffuse = calcFlashLight(0, norm, diffuseColor, specColor, viewDir);

    result += diffuse;
  }
  else {
    float diffFactor = max(dot(norm, viewDir), 0.0);

    diffuse = diffFactor*diffuseColor;

    result += diffuse;
  }

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

  if (shadowMapDebug) {
    FragColor = ShadowColor;
  }
}
