#version 330 core
#define kernelSize 16

in vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;

uniform sampler2D gPosition;   // world-space positions
uniform sampler2D gNormal;     // world-space normals
uniform sampler2D texNoise;

uniform vec3 samples[kernelSize];
const float radius = 0.5;   // adjust as needed for your scene
out float FragColor;

void main()
{
    
    vec3 fragPosWorld = texture(gPosition, TexCoords).xyz;
    vec3 normalWorld = texture(gNormal, TexCoords).rgb;


    vec3 fragPosView = vec3(view * vec4(fragPosWorld, 1.0));
    vec3 normalView = normalize(mat3(transpose(inverse(view))) * normalWorld);

    vec3 randomVec = texture(texNoise, TexCoords * textureSize(gPosition, 0) / textureSize(texNoise, 0)).xyz;

    vec3 tangent = normalize(randomVec - normalView * dot(randomVec, normalView));
    vec3 bitangent = normalize(cross(normalView, tangent));
    mat3 TBN = mat3(tangent,  bitangent, normalView);

    int adaptiveKernelSize = 16;

    FragColor = 1.0;
    float positionDepth = fragPosView.z;
    for(int i = 0; i < adaptiveKernelSize; ++i)
    {
        // Sample position in view space
        vec3 samplePosView = fragPosView + TBN * samples[i] * radius;

        vec4 offset = projection * vec4(samplePosView, 1.0);
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        vec3 sampleWorld = texture(gPosition, offset.xy).xyz;
        float sampleDepth = (view * vec4(sampleWorld, 1.0)).z;
        float rangeCheck = smoothstep(0.0,1.0,0.1 / max(abs(positionDepth - sampleDepth), 0.001));

        FragColor -= (sampleDepth >= samplePosView.z + 0.05 ? rangeCheck : 0.0) * 3.0 / adaptiveKernelSize;
    }
    FragColor = clamp(FragColor, 0.0, 1.0);
}