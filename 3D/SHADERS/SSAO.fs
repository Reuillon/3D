#version 330 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;   // world-space positions
uniform sampler2D gNormal;     // world-space normals
uniform sampler2D texNoise;

uniform vec3 samples[64];

int kernelSize = 64;
float radius = 0.3;   // adjust as needed for your scene
float bias = 0.1;

const vec2 noiseScale = vec2(2560.0/16.0, 1440.0/16.0); 

uniform mat4 projection;
uniform mat4 view;

void main()
{
    // ----------------------------
    // Convert world-space gPosition to view space for SSAO
    // ----------------------------
    vec3 fragPosWorld = texture(gPosition, TexCoords).xyz;
    vec3 fragPosView = (view * vec4(fragPosWorld, 1.0)).xyz;

    // Transform normal to view space
    vec3 normalWorld = normalize(texture(gNormal, TexCoords).rgb);
    vec3 normalView = normalize(mat3(view) * normalWorld);

    // ----------------------------
    // Build TBN matrix in view space
    // ----------------------------
    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);
    vec3 tangent = normalize(randomVec - normalView * dot(randomVec, normalView));
    vec3 bitangent = cross(normalView, tangent);
    mat3 TBN = mat3(tangent, bitangent, normalView);

    float occlusion = 0.0;

    for(int i = 0; i < kernelSize; ++i)
    {
        // Sample position in view space
        vec3 samplePos = fragPosView + TBN * samples[i] * radius;

        // Project to clip space to get UVs for sampling neighboring pixels
        vec4 offset = projection * vec4(samplePos, 1.0);
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        // Sample depth (convert gPosition to view space for SSAO)
        vec3 sampleWorld = texture(gPosition, offset.xy).xyz;
        float sampleDepth = (view * vec4(sampleWorld, 1.0)).z;

        // Range check & accumulate occlusion
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPosView.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / kernelSize);

    FragColor = occlusion;
}