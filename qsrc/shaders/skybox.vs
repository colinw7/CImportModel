#version 330 core

layout (location = 0) in vec3 pos;

out vec3 fragPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
  fragPos = pos;

//gl_Position = projection * view * model * vec4(pos, 1.0);
  gl_Position = (projection * view * model * vec4(pos, 1.0)).xyww;
//gl_Position = view * model * vec4(pos, 1.0);
}
