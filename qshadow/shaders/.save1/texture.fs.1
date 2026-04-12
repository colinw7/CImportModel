#version 330 core

in vec2 TexCoords;

uniform sampler2D textureId;
uniform bool      isDepth;

uniform float near_plane;
uniform float far_plane;

//uniform float shadowScale;

// required when using a perspective projection matrix
float LinearizeDepth(float depth) {
  float z = depth*2.0 - 1.0; // Back to NDC
  return (2.0*near_plane*far_plane)/(far_plane + near_plane - z*(far_plane - near_plane)); 
}

void main() {
  if (isDepth) {
    float depth = texture(textureId, TexCoords).r;

    // gl_FragColor = vec4(vec3(LinearizeDepth(depth)/far_plane), 1.0); // perspective

/*
    if (depth < 0)
      depth = -depth;

    depth = depth/shadowScale;
*/

    if      (depth < 0.0)
      gl_FragColor = vec4(-depth, 0, 0, 1);
    else if (depth <= 1.0)
      gl_FragColor = vec4(0, depth, 0, 1);
    else
      gl_FragColor = vec4(0, 0, 1/depth, 1);

    //gl_FragColor = vec4(vec3(depth), 1.0);
  } else {
    vec4 c = texture(textureId, TexCoords);

    gl_FragColor = c;
  }
}
