#version 330 core

uniform samplerCube textureId;
//uniform sampler2D textureId;
uniform bool isWireframe;

in vec3 fragPos;

out vec4 outCol;

void main() {
  if (isWireframe) {
  //outCol = vec4(fragPos, 1.0);
    outCol = vec4(1.0, 1.0, 1.0, 1.0);
  } else {
    outCol = texture(textureId, fragPos);
  //outCol = texture(textureId, 0.5*(fragPos.xy + 1.0));
  //outCol = vec4(fragPos, 1.0);
  }
}
