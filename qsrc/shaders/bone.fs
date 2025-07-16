#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 ambientColor;

uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform float shininess;

uniform bool isWireframe;

void main()
{
  // normal
  vec3 norm = normalize(Normal);

  // diffuse
  vec3 lightDir = normalize(lightPos - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);

  vec3 diffuseColor = Color;
  vec3 diffuse = diffuseStrength*diff*diffuseColor;

  // ambient
  vec3 ambient = ambientStrength*ambientColor;

  // specular
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
  vec3 specColor = lightColor;
  vec3 specular = specularStrength*spec*specColor;

  vec3 emmisive = vec3(0, 0, 0);

  vec3 result = ambient + diffuse + specular;

  if (isWireframe) {
    FragColor = vec4(Color, 1.0);
  } else {
    FragColor = vec4(result, 1.0);
  }
}
