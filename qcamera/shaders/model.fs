#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
in vec2 TexCoords;

out vec4 FragColor;

//---- Lights

uniform vec3 viewPos;
uniform vec3 ambient;

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

uniform bool isWireframe;
uniform bool isSelected;

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

//---

void main() {
  // normal
  vec3 norm = calcNormal();

  // diffuse color
  vec3 diffuseColor = calcDiffuseColor();

  vec3 viewDir = normalize(viewPos - FragPos);

  float diffuseStrength = max(dot(norm, viewDir), 0.0);

  vec3 diffuse = diffuseStrength*diffuseColor + ambient;

  if (! isWireframe) {
    if (isSelected) {
      FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    } else {
      FragColor = vec4(diffuse, 1.0);
    }
  } else {
    if (isSelected) {
      FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    } else {
      FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
  }
}
