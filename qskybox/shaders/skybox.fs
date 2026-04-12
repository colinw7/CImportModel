#version 330 core

uniform samplerCube textureId;
uniform bool isWireframe;

in vec3 TexCoords;

out vec4 FragColor;

void main() {
  if (isWireframe) {
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
  } else {
    FragColor = texture(textureId, TexCoords);
  }
}
