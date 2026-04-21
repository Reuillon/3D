#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>
#include <array>
#include <vector>
#include <utility>
#include <fstream>
#include <string>



struct ResolutionData 
{
    glm::vec3 Normal;
    float PenetrationDepth;
    bool hasCollision;
};

struct MeshCollider
{
    private:
        
    public:
        std::vector<glm::vec3> identity;
        std::vector<glm::vec3> vertices;
        glm::vec3 pos = glm::vec3(0.0f);
        glm::vec3 rot = glm::vec3(0.0f);
        glm::vec3 scale = glm::vec3(1.0f);
        glm::vec3 color = glm::vec3(1.0f);
        
        //INITIALIZES MESH DATA AND MESHIDENTITY DATA
        MeshCollider(float model[], int arraySize);
        MeshCollider();
        void init(float model[], int arraySize);
        void init(std::vector<glm::vec3> model, int arraySize);

        //CHANGES POSITION/ROTATION/SCALE OF MESH COLLIDER
        void setTransform(glm::vec3 newPos, glm::vec3 rotation, glm::vec3 newScale = glm::vec3(1.0f));
        void moveCollider(glm::vec3 newPos);

        //FINDS FURTHEST VERTEX FROM DIRECTION FOR GJK COLLISION DETECTION
        glm::vec3 FindFurthestVertex(glm::vec3 direction);
};


//CREATES SIMPLEX FOR GJK ALGORITHM
struct Simplex
{
    private:
        std::array<glm::vec3, 4> m_points;
        int m_size;

    public:
        Simplex() : m_size(0) {}
        Simplex& operator=(std::initializer_list<glm::vec3> list)
        {
            m_size = 0;
            for (glm::vec3 point : list)
            {
                m_points[m_size++] = point;
            }
            return *this;
        }
        void push_front(glm::vec3 point)
        {
            m_points = { point, m_points[0], m_points[1], m_points[2] };
            m_size = std::min(m_size + 1, 4);
        }
        glm::vec3& operator[](int i) { return m_points[i]; }
        size_t size() const { return m_size; }
        auto begin() const { return m_points.begin(); }
        auto end() const { return m_points.end() - (4 - m_size); }
};

glm::vec3 Support(MeshCollider& collider1, MeshCollider& collider2, glm::vec3 direction);

//GJK COLLISION AND HELPER FUNCTIONS
bool SameDirection(const glm::vec3& direction, const glm::vec3& ao);

bool Line(Simplex& points, glm::vec3& direction);

bool Triangle(Simplex& points, glm::vec3& direction);

bool Tetrahedron(Simplex& points, glm::vec3& direction);

bool NextSimplex(Simplex& points, glm::vec3& direction);


//RUNS COLLSION TEST FOR GJK ALGORITHM OF TWO CONVEX OBJECTS
ResolutionData GJK(MeshCollider& collider1, MeshCollider& collider2, float deltaTime, bool resolve);

//EPA COLLISION AND HELPER FUNCTIONS
void AddIfUniqueEdge(std::vector<std::pair<size_t, size_t>>& edges, const std::vector<size_t>& faces, size_t a, size_t b);

std::pair<std::vector<glm::vec4>, size_t> GetFaceNormals(const std::vector<glm::vec3>& polytope, const std::vector<size_t>& faces);

ResolutionData EPA(Simplex& simplex, MeshCollider& colliderA, MeshCollider& colliderB, float deltaTime);

std::vector<MeshCollider> initCollisionMap(std::string filePath);

