#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoords;
layout (location = 4) in vec4 BoneIds;
layout (location = 5) in vec4 BoneWeights;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform bool useBones;
uniform bool useBonePoints;
uniform int  boneId;
uniform mat4 globalBoneTransform[128];

vec3 applyBonePointTransform(vec4 p) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < 4; ++i) {
    mat4 boneTransform = globalBoneTransform[int(BoneIds[i])];
    result += BoneWeights[i]*vec3(boneTransform*p);
  }
  return result;
}

vec3 applyBoneTransform(vec4 p) {
  return vec3(globalBoneTransform[boneId]*p);
}

void main() {
  vec3 position = aPos;
  vec3 norm     = normalize(aNormal);

  if (useBonePoints) {
    position = applyBonePointTransform(vec4(position, 1.0));
    norm     = normalize(applyBonePointTransform(vec4(norm, 0.0)));
  } else if (useBones) {
    position = applyBoneTransform(vec4(position, 1.0));
    norm     = normalize(applyBoneTransform(vec4(norm, 0.0)));
  }

  FragPos = vec3(model*vec4(position, 1.0));
  Normal  = mat3(transpose(inverse(model)))*norm;

  Color     = aColor;
  TexCoords = aTexCoords;

  gl_Position = projection*view*vec4(FragPos, 1.0);
}
