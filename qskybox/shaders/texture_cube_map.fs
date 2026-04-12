#version 330 core

uniform samplerCube textureId;

in vec2 TexCoords;

void main() {
  vec3 dir;
  vec2 localUV;

  // Determine which cell of the 4x3 grid we are in
  if (TexCoords.y > 0.333 && TexCoords.y < 0.666) { // Middle Row (+X, -X, +Z, -Z)
    if (TexCoords.x < 0.25) {        // -X (Left)
      localUV = (TexCoords - vec2(0.0, 0.333))/vec2(0.25, 0.333);
      dir = vec3(1.0, 2.0*localUV.y - 1.0, 2.0*localUV.x - 1.0);
    } else if (TexCoords.x < 0.5)  { // +Z (Front)
      localUV = 1.0 - (TexCoords - vec2(0.25, 0.333))/vec2(0.25, 0.333);
      dir = vec3(2.0*localUV.x - 1.0, 1.0 - 2.0*localUV.y, 1.0);
    } else if (TexCoords.x < 0.75) { // +X (Right)
      localUV = (TexCoords - vec2(0.5, 0.333))/vec2(0.25, 0.333);
      dir = vec3(-1.0, 2.0*localUV.y - 1.0, 1.0 - 2.0*localUV.x);
    } else {                         // -Z (Back) 
      localUV = 1.0 - (TexCoords - vec2(0.75, 0.333))/vec2(0.25, 0.333);
      dir = vec3(1.0 - 2.0*localUV.x, 1.0 - 2.0*localUV.y, -1.0);
    }
  } else if (TexCoords.y >= 0.666 && TexCoords.x >= 0.25 && TexCoords.x < 0.5) { // Top Row (+Y)
    localUV = 1.0 - (TexCoords - vec2(0.25, 0.666))/vec2(0.25, 0.333);
    dir = vec3(2.0*localUV.x - 1.0, 1.0, 2.0*localUV.y - 1.0);
  } else if (TexCoords.y <= 0.333 && TexCoords.x >= 0.25 && TexCoords.x < 0.5) { // Bottom Row (-Y)
    localUV = 1.0 - (TexCoords - vec2(0.25, 0.0))/vec2(0.25, 0.333);
    dir = vec3(2.0*localUV.x - 1.0, -1.0, 1.0 - 2.0*localUV.y);
  } else {
    discard; // Areas of the 4x3 grid without faces
  }

  gl_FragColor = textureCube(textureId, dir);
}
