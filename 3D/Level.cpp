#include "Level.h"


void Level::iniShader(const char* vertexPath, const char* fragmentPath)
{
	Shader aShader(vertexPath, fragmentPath);
	shaders.push_back(aShader);
	
}
Model Level::iniModel(std::string mName,bool isStatic)
{
    Model map("Models/TESTMAP/testmap.fbx");
    Animation mapAnim("Models/TESTMAP/testmap.fbx", &map, 0);
    Animator mapAnmtr(&mapAnim);
    glm::mat3 normal;
    auto transforms = mapAnmtr.GetFinalBoneMatrices();
    for (int i = 0; i < transforms.size(); ++i)
    {
        shaders.at(0).setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

    }
    glm::mat4 model = glm::mat4(1.0f);


    model = glm::rotate(model, 90 * 0.0174533f, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::rotate(model, 90 * 0.0174533f, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
    model = glm::translate(model, glm::vec3(0.0f, -15.0f, -7.5f));
    shaders.at(0).setMat4("model", model);
    normal = glm::mat3(glm::transpose(glm::inverse(model)));
    shaders.at(0).setMat3("inverse", normal);
    models.push_back(map);
}
void Level::iniLight()
{}
void Level::run()
{
    models.at(0).draw(shaders.at(0));
}