#version 330 core

layout (location = 0) in vec3 aPos;

out vec4 FragPos;

uniform mat4 meshMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  FragPos = model*meshMatrix*vec4(aPos, 1.0);

  //gl_Position = view*FragPos;
  //gl_Position = FragPos;
  gl_Position = projection*view*FragPos;

  FragPos = view*FragPos;
  //FragPos = projection*view*FragPos;
}
