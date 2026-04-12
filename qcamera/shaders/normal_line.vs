#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoords;
layout (location = 4) in vec4 BoneIds;
layout (location = 5) in vec4 BoneWeights;

out VS_OUT {
  vec3 normal;
} vs_out;

uniform mat4 meshMatrix;
uniform mat4 model;
uniform mat4 view;

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
  /*
  gl_Position = view*model*vec4(aPos, 1.0);
  */
  vec3 position = aPos;
  vec3 norm     = normalize(aNormal);

  if (useBonePoints) {
    position = applyBonePointTransform(vec4(position, 1.0));
//  norm     = normalize(applyBonePointTransform(vec4(norm, 0.0)));
  }

  position = vec3(meshMatrix*vec4(position, 1.0));

  vec3 FragPos = vec3(model*vec4(position, 1.0));

  gl_Position = view*vec4(FragPos, 1.0);

//vs_out.normal = mat3(transpose(inverse(model)))*norm;

  mat3 normalMatrix = mat3(transpose(inverse(view*model)));
  vs_out.normal = vec3(vec4(normalMatrix*norm, 0.0));
//vs_out.normal = vec3(vec4(normalMatrix*aNormal, 0.0));

//Color     = aColor;
//TexCoords = aTexCoords;
}
