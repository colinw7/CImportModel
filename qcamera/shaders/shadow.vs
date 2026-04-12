#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 4) in vec4 BoneIds;
layout (location = 5) in vec4 BoneWeights;

out vec4 FragPos;

uniform mat4 meshMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform bool useBonePoints;
uniform mat4 globalBoneTransform[128];
  
vec3 applyBonePointTransform(vec4 p) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < 4; ++i) {
    mat4 boneTransform = globalBoneTransform[int(BoneIds[i])];
    result += BoneWeights[i]*vec3(boneTransform*p);
  }
  return result;
}

void main() {
  vec4 position = vec4(aPos, 1.0);

  if (useBonePoints) {
    position = vec4(applyBonePointTransform(position), 1.0);
  }

  position = meshMatrix*position;

  FragPos = model*position;

  gl_Position = projection*view*FragPos;
}
