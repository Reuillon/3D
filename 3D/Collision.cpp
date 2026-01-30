#include "Collision.h"

MeshCollider::MeshCollider(float model[], int arraySize)
{
    for (int i = 0; i < arraySize; i += 3)
    {
        identity.push_back(glm::vec3(model[i], model[i + 1], model[i + 2]));
        vertices.push_back(glm::vec3(model[i], model[i + 1], model[i + 2]));
    }
}
MeshCollider::MeshCollider(){}

void MeshCollider::init(float model[], int arraySize)
{
    for (int i = 0; i < arraySize; i += 3)
    {
        identity.push_back(glm::vec3(model[i], model[i + 1], model[i + 2]));
        vertices.push_back(glm::vec3(model[i], model[i + 1], model[i + 2]));
    }
}
void MeshCollider::init(std::vector<glm::vec3> model, int arraySize)
{
    identity.clear();
    vertices.clear();
    for (int i = 0; i < arraySize; i ++)
    {
        identity.push_back(model[i]);
        vertices.push_back(model[i]);
    }
}

//SET TRANSFORMS FOR COLLIDER
void MeshCollider::setTransform(glm::vec3 newPos, glm::vec3 rotation, glm::vec3 newScale)
{

    for (int i = 0; i < vertices.size(); i++)
    {
        pos = newPos;
        rot = rotation;
        scale = newScale;
        //SET ROTATION AND SCALE
        vertices[i].x = ((identity[i].x * newScale.x) * (cos(rotation.y) * cos(rotation.z))) + ((identity[i].y * newScale.y) * ((sin(rotation.x) * sin(rotation.y) * cos(rotation.z)) - (cos(rotation.x) * sin(rotation.z)))) + ((identity[i].z * newScale.z) * ((cos(rotation.x) * sin(rotation.y) * cos(rotation.z)) + (sin(rotation.x) * sin(rotation.z))));
        vertices[i].y = ((identity[i].x * newScale.x) * (cos(rotation.y) * sin(rotation.z))) + ((identity[i].y * newScale.y) * ((sin(rotation.x) * sin(rotation.y) * sin(rotation.z)) + (cos(rotation.x) * cos(rotation.z)))) + ((identity[i].z * newScale.z) * ((cos(rotation.x) * sin(rotation.y) * sin(rotation.z)) - (sin(rotation.x) * cos(rotation.z))));
        vertices[i].z = ((identity[i].x * newScale.x) * (-sin(rotation.y))) + ((identity[i].y * newScale.y) * (sin(rotation.x) * cos(rotation.y))) + ((identity[i].z * newScale.z) * (cos(rotation.x) * cos(rotation.y)));

        //SET POSITION
        vertices[i] = vertices[i] + newPos;
    }
}

void MeshCollider::moveCollider(glm::vec3 velocity)
{
    pos += velocity;
    for (int i = 0; i < vertices.size(); i++)
    {
        vertices[i] = identity[i] + pos;
    }
}

glm::vec3 MeshCollider::FindFurthestVertex(glm::vec3 direction)
{
    glm::vec3 maxPoint = glm::vec3(-FLT_MAX);
    float maxDistance = -FLT_MAX;
    for (glm::vec3 vertex : vertices)
    {
        float distance = glm::dot(vertex, direction);
        if (distance > maxDistance)
        {
            maxDistance = distance;
            maxPoint = vertex;
        }
    }

    return maxPoint;
}



glm::vec3 Support(MeshCollider& collider1, MeshCollider& collider2, glm::vec3 direction)
{
    return collider1.FindFurthestVertex(direction) - collider2.FindFurthestVertex(-direction);
}


//GJK COLLISION AND HELPER FUNCTIONS
bool SameDirection(const glm::vec3& direction, const glm::vec3& ao)
{
    return dot(direction, ao) > 0;
}

bool Line(Simplex& points, glm::vec3& direction)
{
    glm::vec3 a = points[0];
    glm::vec3 b = points[1];

    glm::vec3 ab = b - a;
    glm::vec3 ao = -a;

    if (SameDirection(ab, ao))
    {
        direction = glm::cross(glm::cross(ab, ao), ab);
    }
    else
    {
        points = { a };
        direction = ao;
    }

    return false;
}

bool Triangle(Simplex& points, glm::vec3& direction)
{
    glm::vec3 a = points[0];
    glm::vec3 b = points[1];
    glm::vec3 c = points[2];

    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    glm::vec3 ao = -a;

    glm::vec3 abc = glm::cross(ab, ac);

    if (SameDirection(glm::cross(abc, ac), ao)) {
        if (SameDirection(ac, ao)) {
            points = { a, c };
            direction = glm::cross(glm::cross(ac, ao), ac);
        }

        else {
            return Line(points = { a, b }, direction);
        }
    }

    else {
        if (SameDirection(glm::cross(ab, abc), ao)) {
            return Line(points = { a, b }, direction);
        }

        else {
            if (SameDirection(abc, ao)) {
                direction = abc;
            }

            else {
                points = { a, c, b };
                direction = -abc;
            }
        }
    }

    return false;
}

bool Tetrahedron(Simplex& points, glm::vec3& direction)
{
    glm::vec3 a = points[0];
    glm::vec3 b = points[1];
    glm::vec3 c = points[2];
    glm::vec3 d = points[3];

    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    glm::vec3 ad = d - a;
    glm::vec3 ao = -a;

    glm::vec3 abc = glm::cross(ab, ac);
    glm::vec3 acd = glm::cross(ac, ad);
    glm::vec3 adb = glm::cross(ad, ab);

    if (SameDirection(abc, ao)) {
        return Triangle(points = { a, b, c }, direction);
    }

    if (SameDirection(acd, ao)) {
        return Triangle(points = { a, c, d }, direction);
    }

    if (SameDirection(adb, ao)) {
        return Triangle(points = { a, d, b }, direction);
    }

    return true;
}

bool NextSimplex(Simplex& points, glm::vec3& direction)
{
    //    std::cout << points.size() <<  "\n";
    switch (points.size())
    {
    case 2:  return Line(points, direction);
    case 3:  return Triangle(points, direction);
    case 4:  return Tetrahedron(points, direction);
    }
    return false;
}

//RUNS COLLSION TEST FOR GJK ALGORITHM OF TWO CONVEX OBJECTS
ResolutionData GJK(MeshCollider& collider1, MeshCollider& collider2, float deltaTime, bool resolve)
{
    Simplex points;
    float elapsedTime = deltaTime;
    glm::vec3 direction = collider1.pos - collider2.pos;
    if (direction == glm::vec3(0.0f))
    {
        direction = glm::vec3(0.01,0.01,0.01);
    }
    glm::vec3 supportPoint = Support(collider1, collider2, direction);
    direction = -supportPoint;
    ResolutionData r;
    r.Normal = glm::vec3(0);
    r.PenetrationDepth = -1;
    r.hasCollision = false;
    while (true)
    {
        elapsedTime += deltaTime;
        if (elapsedTime > 0.5)
        {
            r.hasCollision = false;
            return r;
        }
        supportPoint = Support(collider1, collider2, direction);
        if (dot(supportPoint, direction) <= 0)
        {
            return r;
        }
        points.push_front(supportPoint);
        if (NextSimplex(points, direction))
        {
            r.hasCollision = true;
            if (resolve == true)
            {
                r = EPA(points, collider1, collider2, deltaTime);
                
                collider1.setTransform(collider1.pos - (r.Normal * r.PenetrationDepth),glm::vec3(0.0f));

            }
            return r;
        }
    }
}

//EPA COLLISION AND HELPER FUNCTIONS

void AddIfUniqueEdge(std::vector<std::pair<size_t, size_t>>& edges,const std::vector<size_t>& faces,size_t a,size_t b)
{
    auto reverse = std::find(                       //      0--<--3
        edges.begin(),                              //     / \ B /   A: 2-0
        edges.end(),                                //    / A \ /    B: 0-2
        std::make_pair(faces[b], faces[a]) //   1-->--2
    );

    if (reverse != edges.end()) 
    {
        edges.erase(reverse);
    }

    else {
        edges.emplace_back(faces[a], faces[b]);
    }
}

std::pair<std::vector<glm::vec4>, size_t> GetFaceNormals(const std::vector<glm::vec3>& polytope,const std::vector<size_t>& faces)
{
    std::vector<glm::vec4> normals;
    size_t minTriangle = 0;
    float  minDistance = FLT_MAX;

    for (size_t i = 0; i < faces.size(); i += 3) {
        glm::vec3 a = polytope[faces[i]];
        glm::vec3 b = polytope[faces[i + 1]];
        glm::vec3 c = polytope[faces[i + 2]];

        glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
        float distance = glm::dot(normal, a);

        if (distance < 0) {
            normal *= -1;
            distance *= -1;
        }

        normals.emplace_back(normal, distance);

        if (distance < minDistance) {
            minTriangle = i / 3;
            minDistance = distance;
        }
    }

    return { normals, minTriangle };
}

ResolutionData EPA(Simplex& simplex,MeshCollider& colliderA, MeshCollider& colliderB, float deltaTime)
{
    float elapsedTime = deltaTime;
    std::vector<glm::vec3> polytope(simplex.begin(), simplex.end());
    std::vector<size_t> faces = {
        0, 1, 2,
        0, 3, 1,
        0, 2, 3,
        1, 3, 2
    };
    auto [normals, minFace] = GetFaceNormals(polytope, faces);
    
    glm::vec3 minNormal;
    float minDistance = FLT_MAX;


    ResolutionData epaData; 
    while (minDistance == FLT_MAX) 
    {
        
        minNormal = glm::vec3(normals[minFace].x, normals[minFace].y, normals[minFace].z);
        minDistance = normals[minFace].w;

        glm::vec3 support = Support(colliderA, colliderB, minNormal);
        float sDistance = glm::dot(minNormal, support);

        if (abs(sDistance - minDistance) > 0.2f) {
            minDistance = FLT_MAX;
            std::vector<std::pair<size_t, size_t>> uniqueEdges;

            for (size_t i = 0; i < normals.size(); i++) {
                if (SameDirection(normals[i], support - polytope[faces[i * 3]]))
                {
                    size_t f = i * 3;

                    AddIfUniqueEdge(uniqueEdges, faces, f, f + 1);
                    AddIfUniqueEdge(uniqueEdges, faces, f + 1, f + 2);
                    AddIfUniqueEdge(uniqueEdges, faces, f + 2, f);

                    faces[f + 2] = faces.back(); faces.pop_back();
                    faces[f + 1] = faces.back(); faces.pop_back();
                    faces[f] = faces.back(); faces.pop_back();

                    normals[i] = normals.back(); // pop-erase
                    normals.pop_back();

                    i--;
                }
            }
            std::vector<size_t> newFaces;
            for (auto [edgeIndex1, edgeIndex2] : uniqueEdges) {
                newFaces.push_back(edgeIndex1);
                newFaces.push_back(edgeIndex2);
                newFaces.push_back(polytope.size());
            }

            polytope.push_back(support);

            auto [newNormals, newMinFace] = GetFaceNormals(polytope, newFaces);
            float oldMinDistance = FLT_MAX;
            for (size_t i = 0; i < normals.size(); i++) {
                if (normals[i].w < oldMinDistance) {
                    oldMinDistance = normals[i].w;
                    minFace = i;
                }
            }

            if (newNormals[newMinFace].w < oldMinDistance) {
                minFace = newMinFace + normals.size();
            }

            faces.insert(faces.end(), newFaces.begin(), newFaces.end());
            normals.insert(normals.end(), newNormals.begin(), newNormals.end());
        }
        elapsedTime += deltaTime;
        if (elapsedTime > 0.1)
        {
            epaData.hasCollision = false;
            return epaData;
        }
    }
    epaData.Normal = minNormal;
    epaData.PenetrationDepth = minDistance + 0.01f;
    epaData.hasCollision = true;

    return epaData;
}

std::vector<MeshCollider> initCollisionMap(std::string filePath)
{

    std::vector<MeshCollider> collisionMap;
    std::vector<MeshCollider> clearVector;
    MeshCollider currentMeshData;
    std::fstream colliderData(filePath);
    std::string text;
    
    if (colliderData.is_open())
    {
        //SKIP JUNK AT START OF OBJ
        for (int i = 0; i < 3; i++)
        {
            getline(colliderData, text);
        }

        std::vector<glm::vec3> vertices;
        while (getline(colliderData, text))
        {
            //ADDS MESH OBJECT TO COLLISION MAP
            if (text[0] == 's')
            {
                currentMeshData.init(vertices, vertices.size());
                collisionMap.push_back(currentMeshData);
                vertices.clear();
            }
            //ALLOCATES VERTICES TO EACH MESHCOLLIDER IN A COLLISION MAP
            if (text[0] == 'v' && text[1] == ' ')
            {
                float vertice[3];
                std::string s = "";
                int vertIndex = 0;
                for (int i = 2; i < text.size(); i++)
                {
                    if (text[i] != ' ')
                    {
                        s = s + text[i];
                    }
                    else
                    {
                        vertice[vertIndex] = std::stof(s);
                        vertIndex += 1;
                        s = "";
                    }
                    if (i == (text.size() - 1))
                    {
                        vertice[vertIndex] = std::stof(s);
                        vertIndex = 0;
                        vertices.push_back(glm::vec3(vertice[0] + 100, vertice[1] + 100, vertice[2] + 100));
                    }
                }

            }
        }
    }
    return collisionMap;
}