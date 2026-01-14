#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 center;
uniform vec3 cameraUp;
uniform vec3 cameraRight;

void main() {
  vec3 position  = aPos;
  vec3 position1 = center + (cameraRight*position.x) + (cameraUp*position.y);

  vec3 norm = normalize(aNormal);

  FragPos = vec3(model*vec4(position1, 1.0));
  Normal  = mat3(transpose(inverse(model)))*norm;

  Color = aColor;

  gl_Position = projection*view*vec4(FragPos, 1.0);
}
