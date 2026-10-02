#version 330 core

in vec2 TexCoords;

uniform sampler2D textureId;
uniform bool      isDepth;

uniform float near_plane;
uniform float far_plane;

uniform float min_value;
uniform float max_value;

//uniform sampler2DShadow textureId;

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

    //gl_FragColor = c;

    float g = (c.r - min_value)/(max_value - min_value);

    gl_FragColor = vec4(g, g, g, 1);
  } else {
    vec4 c = texture(textureId, TexCoords);

    gl_FragColor = c;
  }
}
