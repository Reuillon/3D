#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gPBR;



//checks
uniform float hasTexture;
uniform float hasRoughness;
uniform float hasMetallic;

//TEXTURES
uniform sampler2D albedoMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;


//NON TEXTURE VALUES
uniform vec3 baseColor;
uniform float rough;
uniform float metal;
uniform float spec;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;




void main()
{     
    // store the fragment position vector in the first gbuffer texture
    gPosition = WorldPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    
    //this stores the albedo color depending on if there is a texture or solid color
    if(hasTexture < 0.5)
    {
        gAlbedo.rgb = pow(baseColor, vec3(1.0/2.2));
    }
    else
    {
        gAlbedo.rgb = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));
    }
    //this stores the metallicness depending on if there is a texture or solid color
    if(hasMetallic < 0.5)
    {
        gPBR.r = metal;
    }
    else
    {
        gPBR.r = texture(metallicMap , TexCoords).r;
    }

    //this stores the roughness depending on if there is a texture or solid color
    if(hasRoughness < 0.5)
    {
        gPBR.g = rough ;   
    }
    else
    {
        gPBR.g = texture(roughnessMap, TexCoords).r;
    }
    gPBR.b = spec;
}
    