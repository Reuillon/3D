#ifndef MESH_H
#define MESH_H


#include "shader.h"

#include <string>
#include <vector>

#define MAX_BONE_INFLUENCE 4

struct Vertex 
{
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
   
    float useDiffuseTexture;

};

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};

struct Material
{
    bool hasTex;
    glm::vec3 Diffuse;
    glm::vec3 Specular;
    float Shininess;
};

class Mesh
{
    public:
        
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
        Material material;
        unsigned int VAO;
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, Material material);
        void draw(Shader& shader);
    private:
        unsigned int VBO, EBO;
        void setupMesh();
        
};
#endif