#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform bool useBones;
uniform int  boneId;
uniform int  parentBoneId;
uniform mat4 globalBoneTransform[128];
uniform bool isLine;

vec3 applyBoneTransform(vec4 p) {
  mat4 boneTransform;
  if (parentBoneId >= 0) {
    if (aTexCoords.x > 0) {
      boneTransform = globalBoneTransform[parentBoneId];
    } else {
      boneTransform = globalBoneTransform[boneId];
    }
  } else {
    boneTransform = globalBoneTransform[boneId];
  }
  return vec3(boneTransform*p);
}

vec3 boneColor() {
  if (isLine) {
    if (parentBoneId >= 0) {
      if (aTexCoords.x > 0) {
        return vec3(1.0, 0.0, 0.0);
      } else {
        return vec3(0.0, 0.0, 1.0);
      }
    } else {
      return aColor;
    }
  } else { 
    return aColor;
  }
}

void main()
{
  vec3 position;
  vec3 norm;
  if (useBones) {
    position = applyBoneTransform(vec4(aPos, 1.0));
    norm     = normalize(applyBoneTransform(vec4(aNormal, 1.0)));
  } else {
    position = aPos;
    norm     = normalize(aNormal);
  }

  FragPos = vec3(model*vec4(position, 1.0));
  Normal  = mat3(transpose(inverse(model)))*norm;
  Color   = boneColor();

  gl_Position = projection*view*vec4(FragPos, 1.0);
}
