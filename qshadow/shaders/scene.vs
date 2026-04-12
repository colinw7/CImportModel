#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;
out vec2 TexCoords;
out vec4 FragPosLightSpace;

uniform mat4 meshMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 lightSpaceMatrix;

void main() {
  vec4 position = vec4(aPos, 1.0);
  vec3 norm     = normalize(aNormal);

  position = meshMatrix*position;

  FragPos = vec3(model*position);

  // a slight hack to make sure the outer large cube displays lighting
  // from the 'inside' instead of the default 'outside'.
  Normal = transpose(inverse(mat3(model)))*norm;
  
  Color     = aColor;
  TexCoords = aTexCoords;

  FragPosLightSpace = lightSpaceMatrix*vec4(FragPos, 1.0);

  gl_Position = projection*view*vec4(FragPos, 1.0);
}
