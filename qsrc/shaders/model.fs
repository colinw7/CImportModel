#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
//uniform vec3 objectColor;

uniform bool useDiffuseTexture;
uniform bool useSpecularTexture;
uniform bool useNormalTexture;

uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform float shininess;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;

uniform bool isWireframe;

void main()
{
  // normal
  vec3 norm;
  if (useNormalTexture) {
    norm = texture(normalTexture, TexCoords).rgb;
    norm = normalize(norm*2.0 - 1.0); // this normal is in tangent space
    //norm = normalize(norm);
  }
  else
    norm = normalize(Normal);

  // diffuse
  vec3 lightDir = normalize(lightPos - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);

  vec3 diffuseColor;
  if (useDiffuseTexture)
    diffuseColor = texture(diffuseTexture, TexCoords).rgb;
  else
    diffuseColor = Color;
  vec3 diffuse = diffuseStrength*diff*diffuseColor;

  // ambient
  vec3 ambient = ambientStrength*diffuseColor;

  // specular
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
  vec3 specColor;
  if (useSpecularTexture)
    specColor = texture(specularTexture, TexCoords).rgb;
  else
    specColor = lightColor;
  vec3 specular = specularStrength*spec*specColor;

  vec3 result = ambient + diffuse + specular;

  FragColor = (isWireframe ? vec4(1.0, 1.0, 1.0, 1.0) : vec4(result, 1.0));
}
