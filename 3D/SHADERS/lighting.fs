#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;
uniform sampler2D shadowMap;

uniform vec3 lightDir;
uniform vec3 viewPos;




void main()
{       
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedo, TexCoords).rgb;
    float AmbientOcclusion = texture(ssao, TexCoords).r;
    
    vec3 color = Diffuse;
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 lightColor = vec3(1.0);
    // ambient
    vec3 ambient = 0.5 * color * AmbientOcclusion ;
    // diffuse
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor * texture(shadowMap, TexCoords).rgb;
    // specular
    vec3 viewDir = normalize(viewPos - texture(gPosition, TexCoords).rgb);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 0.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
                     
    vec3 lighting = (ambient * (diffuse + specular )) * color;    
    
    FragColor = vec4(lighting, 1.0);
}