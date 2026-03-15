#version 330 core
#extension GL_NV_shadow_samplers_cube : enable
out vec4 FragColor;


in vec3 FragPos;  
in vec3 Normal;
in float oldHeight;
in vec2 TexCoords;
vec3 troughColor = vec3(43.0/255.0, 19.0/255.0, 15.0/255.0);
vec3 surfaceColor = vec3(151/255.0,90/255.0,36.0/255.0);
vec3 peakColor = vec3(254.0/255.0,169.0/255.0,115.0/255.0);

float peakThreshold = 0.08;
float peakTransition = 0.15;
float troughThreshold = 0.0;
float troughTransition = 0.15;
uniform sampler2D noise;


uniform vec3 viewPos;

uniform samplerCube cubeMap;




void main()
{
    
    
  




  float height = FragPos.y - oldHeight;

  float peakFactor = smoothstep(peakThreshold - peakTransition, peakThreshold + peakTransition, height);
  float troughFactor = smoothstep(troughThreshold - troughTransition, troughThreshold + troughTransition, height);

  vec3 mix1 = mix(troughColor, surfaceColor, troughFactor);

  vec3 mix2 = mix(mix1, peakColor *2, peakFactor) * ((texture(noise, TexCoords).rgb) );

  mix2 = pow(mix2, vec3(1.0/2.2));
  FragColor = vec4(mix2, 1.0);
    
}