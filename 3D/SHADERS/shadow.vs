#version 420 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;

out vec3 WorldPos; 
out vec3 Normal;


uniform mat4 model;
uniform mat3 normalMatrix;
uniform mat4 view;
uniform mat4 projection;

uniform bool isStatic;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];




void main()
{


    mat4 BoneTransform = mat4(1.0);
    if(!isStatic)
    {
        BoneTransform =
            finalBonesMatrices[boneIds[0]] * weights[0] +
            finalBonesMatrices[boneIds[1]] * weights[1] +
            finalBonesMatrices[boneIds[2]] * weights[2] +
            finalBonesMatrices[boneIds[3]] * weights[3];
    }

    // Transform position
    vec4 totalPosition = BoneTransform * vec4(aPos, 1.0);

    WorldPos = vec3(model * totalPosition);



    // Transform normal correctly
    vec3 skinnedNormal = (BoneTransform * vec4(aNormal, 0.0)).xyz;
    Normal = normalize(normalMatrix * skinnedNormal);

    gl_Position = projection * view * model * totalPosition;
}