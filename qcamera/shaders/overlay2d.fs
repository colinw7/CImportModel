#version 330 core

in vec3 FragPos;
in vec3 Color;

uniform bool isWireframe;

void main() {
  if (! isWireframe) {
    gl_FragColor = vec4(Color, 1.0);
  } else {
    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
  }
}
