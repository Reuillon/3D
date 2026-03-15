#version 330 core
#extension GL_NV_shadow_samplers_cube : enable
out vec4 FragColor;


in vec3 FragPos;  
in vec3 Normal;
in float oldHeight;

vec3 troughColor = vec3(0.0/255.0, 7.0/255.0, 102.0/255.0);
vec3 surfaceColor = vec3(0.0/255.0,0.0/255.0,0.0/255.0);
vec3 peakColor = vec3(0.0/255.0,129.0/255.0,255.0/255.0);

float peakThreshold = 0.08;
float peakTransition = 0.05;
float troughThreshold = -0.01;
float troughTransition = 0.15;

float fresnelScale = 0.5;
float fresnelPower = 1.0;

uniform vec3 viewPos;

uniform samplerCube cubeMap;




void main()
{
    
    
  
  vec3 viewDir = normalize(FragPos - viewPos);
  vec3 reflectDir = reflect(viewDir, Normal);

  //vec4 reflectionColor = pow(textureCube(cubeMap, reflectDir), vec4(1.0/2.2));
  vec4 reflectionColor = textureCube(cubeMap, reflectDir);

  float fresnel = fresnelScale * pow(1.0 - clamp(dot(viewDir, Normal), 0.0, 1.0), fresnelPower);

  float height = FragPos.y - oldHeight;

  float peakFactor = smoothstep(peakThreshold - peakTransition, peakThreshold + peakTransition, height);
  float troughFactor = smoothstep(troughThreshold - troughTransition, troughThreshold + troughTransition, height);

  vec3 mix1 = mix(troughColor, surfaceColor, troughFactor);

  vec3 mix2 = mix(mix1, peakColor, peakFactor);

  vec3 finalColor = mix(mix2, reflectionColor.rgb, fresnel);
  //HDR TONEMAPPING
  finalColor = finalColor / (finalColor + vec3(1.0));
  //gamma correct
  finalColor = pow(finalColor, vec3(1.0/2.2)); 
  FragColor = vec4(finalColor, 0.45);
    
}