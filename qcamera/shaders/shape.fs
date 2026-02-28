#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;

uniform vec3 viewPos;

uniform float transparency;

uniform bool isWireframe;

void main() {
  vec3 norm = normalize(Normal);

  vec3 viewDir = normalize(viewPos - FragPos);

  vec3 ambient = vec3(0.2, 0.2, 0.2);

  float diff = max(dot(norm, viewDir), 0.0);

  vec3 diffuse = diff*Color + ambient;

  if (! isWireframe) {
    gl_FragColor = vec4(diffuse, transparency);
  } else {
    gl_FragColor = vec4(1.0, 1.0, 1.0, transparency);
  }
}
