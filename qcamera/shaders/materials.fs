#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
in vec2 TexCoords;

out vec4 FragColor;

//--- Lights

uniform vec3 viewPos;

//--- Textures

struct TextureData {
  bool      enabled;
  sampler2D texture;
};

uniform TextureData diffuseTexture;
uniform TextureData normalTexture;

//---

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

vec3 calcNormal() {
  if (normalTexture.enabled) {
    vec3 norm = texture(normalTexture.texture, TexCoords).rgb;
    norm = normalize(norm*2.0 - 1.0); // this normal is in tangent space
    return norm;
  }
  else
    return normalize(Normal);
}

//---

void main() {
  // normal
  vec3 norm = calcNormal();

  // ambient
  vec3 ambient = vec3(0.2, 0.2, 0.2);

  vec3 result = ambient;

  // diffuse color
  vec3 viewDir = normalize(viewPos - FragPos);

  float diff = max(dot(norm, viewDir), 0.0);

  vec3 diffuseColor = calcDiffuseColor();

  vec3 diffuse = diff*diffuseColor;

  result += diffuse;

  //---

  FragColor = vec4(result, 1.0);
}
