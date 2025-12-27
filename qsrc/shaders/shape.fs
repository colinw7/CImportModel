#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
in vec2 TexCoords;

out vec4 FragColor;

//--- Lights

// type:
//  0 : Directional (direction)
//  1 : Point (position)
//  2 : Spot (position, direction, cutoff)
struct Light {
  int   type;
  bool  enabled;
  vec3  position;
  vec3  direction;
  vec3  color;
  float radius;
  float cutoff;
};

#define NUM_LIGHTS 4

uniform Light lights[NUM_LIGHTS];

uniform vec3 viewPos;

//--- Globals

uniform vec3 ambientColor;
uniform vec3 emissionColor;

uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform float emissiveStrength;
uniform float shininess;

//-- State

uniform bool isSelected;
uniform bool isWireframe;

uniform sampler2D textureId;
uniform bool      useTexture;

//---

float calcDiffuse(vec3 lightDir, vec3 nrm) {
  float diffAmt = max(dot(nrm, lightDir), 0.0);
  return diffAmt;
}

float calcSpecular(vec3 lightDir, vec3 viewDir, vec3 nrm, float shininess) {
  vec3 reflectDir = reflect(-lightDir, nrm);
  float specAmt = max(dot(viewDir, reflectDir), 0.0);
  return pow(specAmt, shininess);
}

vec3 calcNormal() {
  return normalize(Normal);
}

vec3 calcDiffuseColor() {
  if (useTexture)
    return texture(textureId, TexCoords).rgb;
  else
    return Color;
}

vec3 calcSpecularColor() {
  return vec3(1, 1, 1);
}

vec3 calcEmission() {
  return emissiveStrength*emissionColor;
}

//---

void main() {
  // normal
  vec3 norm = calcNormal();

  // diffuse color
  vec3 diffuseColor = calcDiffuseColor();

  vec3 diffuse = diffuseStrength*diffuseColor;

  // specular color
  vec3 specColor = calcSpecularColor();

  vec3 specular = specularStrength*specColor;

  // ambient
  vec3 ambient = ambientStrength*ambientColor;
  
  vec3 viewDir = normalize(viewPos - FragPos);

  vec3 result = ambient;

  bool lighted = false;

  for (int i = 0; i < NUM_LIGHTS; ++i) {
    if (! lights[i].enabled) {
      continue;
    }

    lighted = true;

    vec3 toLight  = lights[i].position - FragPos;
    vec3 lightDir = normalize(toLight);

    if      (lights[i].type == 0) { // directional
      // diffuse light color
      float diff = calcDiffuse(lights[i].direction, norm);
      result += diff*lights[i].color*diffuse;

      // specular light color
      float spec = calcSpecular(lights[i].direction, viewDir, norm, shininess);
      result += spec*spec*lights[i].color*specular;
    }
    else if (lights[i].type == 1) { // point
      // diffuse light color
      float distToLight = length(toLight);
      float falloff = max(0.0, 1.0 - (distToLight/lights[i].radius));
      float diff = calcDiffuse(lightDir, norm)*falloff;
      result += diff*lights[i].color*diffuse;

      // specular light color
      float spec = calcSpecular(lightDir, viewDir, norm, shininess);
      result += spec*spec*lights[i].color*specular;
    }
    else if (lights[i].type == 2) { // spot
      // diffuse light color
      float angle = dot(lights[i].direction, lightDir);
      float falloff = (angle > lights[i].cutoff ? 1.0 : 0.0);

      float diff = calcDiffuse(lightDir, norm)*falloff;
      result += diff*lights[i].color*diffuse;

      // specular light color
      float spec = calcSpecular(lightDir, viewDir, norm, shininess);
      result += spec*spec*lights[i].color*specular;
    }
  }

  // baked diffuse lighting if none
  if (! lighted) {
    float diff = max(dot(norm, viewDir), 0.0);

    vec3 diffuse = diffuseStrength*diff*diffuseColor;

    result += diffuse;
  }

  // add emission
  vec3 emissive = calcEmission();

  result += emissive;

  FragColor = (! isSelected ? (! isWireframe ?
    vec4(result, 1.0) : vec4(1.0, 1.0, 1.0, 1.0)) : vec4(1.0, 0.0, 0.0, 1.0));
}
