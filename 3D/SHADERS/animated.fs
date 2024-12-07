#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
    float shininess;
}; 

struct Material2 
{
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;



#define NR_POINT_LIGHTS 4
  
uniform vec3 viewPos;
uniform Material material;
uniform Material2 material2;
uniform DirLight dirLight;
uniform float useTexture;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;



//FUNCTION PROPERTIES
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


void main()
{
    
    vec3 result = vec3(0.0f);
    //NORMAL CALC
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    result += CalcDirLight(dirLight, norm, viewDir);
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }         
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
   
   
   
     result = pow(result, vec3(1.0/2.2)); 
      
   
   FragColor = vec4(result, 1.0);
} 

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);

    float spec;
    // combine results
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    if (useTexture < 0.5)
    {
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material2.shininess);
        ambient = light.ambient * material2.diffuse;
        diffuse = light.diffuse * diff * material2.diffuse;
        specular = light.specular * spec * material2.specular;
    }
    else
    {
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) ;
        ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
        diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
        specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    }
    
    
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    
    // attenuation
    float distance = length(light.position - fragPos);
    float dividend = (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    if (dividend == 0)
    {
        dividend = 1;
    }
    float attenuation = 1.0 / (true ? distance * distance : distance);    
   
    
    // combine results
    float spec;
    vec3 ambient;
    vec3 specular;
    vec3 diffuse;

    if (useTexture < 0.5)
    {
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material2.shininess);
        ambient = light.ambient * material2.diffuse * attenuation;
        diffuse = light.diffuse * diff * material2.diffuse * attenuation;
        specular = light.specular * spec * material2.specular * attenuation;
    }
    else
    {
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        ambient = light.ambient * vec3(texture(material.diffuse, TexCoords)) * attenuation;
        diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords)) * attenuation;
        specular = light.specular * spec * vec3(texture(material.specular, TexCoords)) * attenuation;
    }
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    
    // attenuation
    float distance = length(light.position - fragPos);
    float dividend = (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    if (dividend == 0)
    {
        dividend = 1;
    }
    float attenuation = 1.0 / (true ? distance * distance : distance);        
    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // combine results
    float spec;
    vec3 ambient;
    vec3 specular;
    vec3 diffuse;
    if (useTexture < 0.5)
    {
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material2.shininess);
        ambient = light.ambient * material2.diffuse;
        diffuse = light.diffuse * diff * material2.diffuse;
        specular = light.specular * spec * material2.specular;
    }
    else
    {
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
        diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
        specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    
    }
    
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}