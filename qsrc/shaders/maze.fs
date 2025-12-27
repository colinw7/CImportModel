#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
in vec2 TexCoords;

uniform vec3 lightPos;

uniform sampler2D textureId;
uniform bool      useTexture;
uniform sampler2D normalTextureId;
uniform bool      useNormalTexture;

void main()
{
  // normal
  vec3 norm = normalize(Normal);

  if (useNormalTexture) {
    norm = texture(normalTextureId, TexCoords).rgb;
    norm = normalize(norm*2.0 - 1.0); // this normal is in tangent space
  }

  // diffuse
  vec3 lightDir = normalize(lightPos - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);

  vec3 diffuseColor = Color;

  if (useTexture)
    diffuseColor = texture(textureId, TexCoords).rgb;

  gl_FragColor = vec4(diff*diffuseColor, 1.0);
}
