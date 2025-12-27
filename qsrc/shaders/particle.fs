#version 330 core

in vec4 Color;
in vec2 TexPos;

uniform bool      useTexture;
uniform sampler2D textureId;

void main() {
  if (useTexture) {
    vec4 tc = texture(textureId, TexPos);
    gl_FragColor = Color*tc;
  } else {
    gl_FragColor = vec4(Color.rgb, 0.5);
  }
}
