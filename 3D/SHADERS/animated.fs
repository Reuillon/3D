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




in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;




  

uniform Material material;
uniform Material2 material2;
uniform DirLight dirLight;






//FUNCTION PROPERTIES
vec3 CalcDirLight(DirLight light, vec3 normal);

void main()
{
   //NORMAL CALC
   vec3 norm = normalize(Normal);
   vec3 result;
   result = CalcDirLight(dirLight, norm); 
   result = pow(result, vec3(1.0/2.2)); 
   FragColor = vec4(result, 1.0);
} 

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal)
{
    // diffuse shading
    float diff = max(dot(normal, light.direction), 0.0);
    // combine results
    vec3 ambient;
    vec3 diffuse;
    ambient = material2.diffuse * 0.6 * 0.95;
    diffuse = vec3(0.1f) * (diff / 2);
    return (ambient * (diffuse + vec3(0.25)) * ambient);
}

