#version 330 core

in vec3 col;
in vec2 uv0;

uniform sampler2D mainTex;

void main() {
  vec4 tc = texture(mainTex, uv0);

  //gl_FragColor = vec4(col.r, col.g, col.b, tc.r);
  gl_FragColor = tc;
}
