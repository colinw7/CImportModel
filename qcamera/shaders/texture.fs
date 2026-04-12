#version 330 core

in vec2 TexCoords;

uniform sampler2D textureId;
//uniform sampler2DShadow textureId;
uniform bool      isDepth;

void main() {
  if (isDepth) {
    //float depth = texture(textureId, TexCoords).r;
    //float depth = texture(textureId, vec3(TexCoords, 0.0));

    vec4 c = texture(textureId, TexCoords);
    //float depth = c.r;

    /*
    if      (depth >= 1.0)
      gl_FragColor = vec4(1, 0, 0, 1);
    else if (depth >= 0.0)
      gl_FragColor = vec4(0, 1, 0, 1);
    else
      gl_FragColor = vec4(0, 0, 0, 1);
    */
    //gl_FragColor = vec4(vec3(depth), 1.0);
    gl_FragColor = c;
  } else {
    vec4 c = texture(textureId, TexCoords);

    gl_FragColor = c;
  }
}
