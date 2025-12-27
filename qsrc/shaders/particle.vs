#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoords;

out vec4 Color;
out vec2 TexPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 cameraUp;
uniform vec3 cameraRight;

uniform vec3  position;
uniform float size;
uniform int   age;
uniform int   maxAge;

void main() {
  float a = (maxAge - age)/(1.0*maxAge);
  Color = vec4(aColor, a);

  TexPos = aPos.xy + 0.5;

//vec3 position1 = (cameraRight*aPos.x*size) + (cameraUp*aPos.y*size);
  vec3 position1 = vec3(aPos.x*size, aPos.y*size, 0.0);

  gl_Position = projection*view*model*vec4(position, 1.0) + vec4(position1, 1.0);
}
