#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;
out vec2 TexPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform bool isWireframe;

void main() {
  vec3 position = aPos;
  vec3 norm     = normalize(aNormal);

  FragPos = vec3(model*vec4(position, 1.0));
  Normal  = mat3(transpose(inverse(model)))*norm;

  if (! isWireframe) {
    Color = aColor;
  } else {
    Color = vec3(255, 255, 255);
  }

  TexPos = aTexCoords;

  gl_Position = projection*view*model*vec4(FragPos, 1.0);
}
