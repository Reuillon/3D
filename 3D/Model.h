#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 


#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"
#include "assimp_glm_helpers.h"


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

struct BoneInfo
{
    int id;
    glm::mat4 offset;
};

class Model
{
    
public:
    // model data 
    std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh> meshes;
    string directory;
    bool gammaCorrection;

    // constructor, expects a filepath to a 3D model.
    Model(string const& path, bool gamma = false);

    Model();

    void initialize(std::string const& path);

    // draws the model, and thus all its meshes
    void draw(Shader& shader);
  
    
    auto& GetBoneInfoMap() {return m_BoneInfoMap;}
    int& GetBoneCount() { return m_BoneCounter;}

private:
    //ANIMATION DATA
    std::map<string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes std::vector.
    void loadModel(string const& path);

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
  
    //INITIALIZE BONE DATA
    void SetVertexBoneDataToDefault(Vertex& vertex);

    //GATHER BONE WEIGHTS
    void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

    //SET BONE DATA
    void SetVertexBoneData(Vertex& vertex, int boneID, float weight);

    Material loadMaterial(aiMaterial* mat) 
    {
        Material material;
        aiColor3D color(0.f, 0.f, 0.f);
        float shininess;

        mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        material.Diffuse = glm::vec3(color.r, color.b, color.g);

        mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
        material.Specular = glm::vec3(color.r, color.b, color.g);

        mat->Get(AI_MATKEY_SHININESS, shininess);
        material.Shininess = shininess;

        return material;
    }
    
    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};



#endif