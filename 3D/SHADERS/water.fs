#version 330 core
#extension GL_NV_shadow_samplers_cube : enable
out vec4 FragColor;
uniform vec4 outColor;

in vec3 FragPos;  
in vec3 Normal;
in vec3 newColor;
in float oldHeight;

vec3 troughColor = vec3(0x18/255.0, 0x66/255.0, 0x91/255.0) / 2.01;
vec3 surfaceColor = vec3(0x98/255.0,0xd8/255.0,0xc0/255.0) / 2.01;
vec3 peakColor = vec3(0xbb/255.0,0xd8/255.0,0xe0/255.0) / 2.01;

float peakThreshold = 0.08;
float peakTransition = 0.05;
float troughThreshold = -0.01;
float troughTransition = 0.15;

float fresnelScale = 0.5;
float fresnelPower = 0.1;

uniform vec3 viewPos;

uniform samplerCube cubeMap;




void main()
{
    
    
  
  vec3 viewDir = normalize(FragPos - viewPos);
  vec3 reflectDir = reflect(viewDir, Normal);
  
  vec4 reflectionColor = textureCube(cubeMap, reflectDir) /4.0;

  float fresnel = fresnelScale * pow(1.0 - clamp(dot(viewDir, Normal), 0.0, 1.0), fresnelPower) / 2.01;

  float height = FragPos.y - oldHeight;

  float peakFactor = smoothstep(peakThreshold - peakTransition, peakThreshold + peakTransition, height);
  float troughFactor = smoothstep(troughThreshold - troughTransition, troughThreshold + troughTransition, height);

  vec3 mix1 = mix(troughColor, surfaceColor, troughFactor);

  vec3 mix2 = mix(mix1, peakColor, peakFactor);

  vec3 finalColor = mix(mix2, reflectionColor.rgb, fresnel);

  FragColor = vec4(finalColor, 0.5);
    
}