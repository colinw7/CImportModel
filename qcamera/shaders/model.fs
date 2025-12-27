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
uniform float emissiveStrength;
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

//--- State

uniform bool isSelected;
uniform bool isWireframe;

uniform float transparency;

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
  if (normalTexture.enabled) {
    vec3 norm = texture(normalTexture.texture, TexCoords).rgb;
    norm = normalize(norm*2.0 - 1.0); // this normal is in tangent space
    return norm;
  }
  else
    return normalize(Normal);
}

vec3 calcDiffuseColor() {
  if (diffuseTexture.enabled) {
    vec4 textureColor = texture(diffuseTexture.texture, TexCoords);

    if (textureColor.a < 0.1)
      discard;

    return textureColor.rgb;
  }
  else
    return Color;
}

vec3 calcSpecularColor() {
  if (specularTexture.enabled)
    return texture(specularTexture.texture, TexCoords).rgb;
  else
    return vec3(1, 1, 1);
}

vec3 calcEmission() {
  if (emissiveTexture.enabled)
    return texture(emissiveTexture.texture, TexCoords).rgb;
  else
    return emissiveStrength*emissionColor;
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

  vec3 diffuse = diffuseStrength*diffuseColor;

  // specular color
  vec3 specColor = calcSpecularColor();

  vec3 specular = specularStrength*specColor;

  vec3 viewDir = normalize(viewPos - FragPos);

  bool lighted = false;

  for (int i = 0; i < NUM_LIGHTS; ++i) {
    if (! lights[i].enabled) {
      continue;
    }

    lighted = true;

    vec3 toLight  = lights[i].position - FragPos;
    vec3 lightDir = normalize(toLight);

    float diff = 1.0;

    if      (lights[i].type == 0) { // directional
      // diffuse light color
      if (! fixedDiffuse)
        diff = calcDiffuse(lights[i].direction, norm);

      result += diff*lights[i].color*diffuse;

      // specular light color
      float spec = calcSpecular(lights[i].direction, viewDir, norm, shininess);

      result += spec*lights[i].color*specular;
    }
    else if (lights[i].type == 1) { // point
      // diffuse light color
      float falloff = 1.0;

      if (lights[i].radius > 0.0) {
        float dist = length(toLight);

        falloff = 1.0/(lights[i].attenuation0 +
                       dist*(lights[i].attenuation1 +
                             dist*lights[i].attenuation2));
        //falloff = max(0.0, 1.0 - (dist/lights[i].radius));
      }

      if (! fixedDiffuse)
        diff = calcDiffuse(lightDir, norm)*falloff;

      result += diff*lights[i].color*diffuse;

      // specular light color
      float spec = calcSpecular(lightDir, viewDir, norm, shininess);

      result += spec*lights[i].color*specular;
    }
    else if (lights[i].type == 2) { // spot
      // diffuse light color
      float angle = dot(lights[i].direction, lightDir);

      if (angle > lights[i].cutoff) {
        float falloff = pow(angle, lights[i].exponent);

        if (! fixedDiffuse)
          diff = calcDiffuse(lightDir, norm)*falloff;

        result += diff*lights[i].color*diffuse;
      }

      // specular light color
      float spec = calcSpecular(lightDir, viewDir, norm, shininess);

      result += spec*lights[i].color*specular;
    }
  }

  // baked diffuse lighting if none
  if (! lighted) {
    float diff = 1.0;

    if (! fixedDiffuse)
      diff = max(dot(norm, viewDir), 0.0);

    result += diff*diffuse;
  }

  // add emission
  vec3 emissive = calcEmission();

  result += emissive;

  // adjust color by state

  if (! isWireframe) {
    if (! isSelected) {
      FragColor = vec4(result, transparency);
    } else {
      FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
  } else {
    if (! isSelected) {
      FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    } else {
      FragColor = vec4(0.1, 0.1, 0.1, 1.0);
    }
  }
}
