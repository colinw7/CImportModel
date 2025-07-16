#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoords;

//attribute vec4 position;
//attribute vec2 texCoord0;
//attribute vec3 color;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 uv0;
out vec3 col;

void main() {
  gl_Position = projection*view*model*vec4(aPos, 1);

  uv0 = aTexCoords;
  col = aColor;
}
