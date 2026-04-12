#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform bool onTop;

void main() {
  TexCoords = aPos;

  vec4 pos = projection*view*model*vec4(aPos, 1.0);

  if (onTop) {
    gl_Position = pos.xyww;
  }
  else {
    gl_Position = pos;
  }
}
