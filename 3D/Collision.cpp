#include "Collision.h"

MeshCollider::MeshCollider(float model[], int arraySize)
{
    for (int i = 0; i < arraySize; i += 3)
    {
        identity.push_back(glm::vec3(model[i], model[i + 1], model[i + 2]));
        vertices.push_back(glm::vec3(model[i], model[i + 1], model[i + 2]));
    }
}

void MeshCollider::colliderSet(glm::vec3 newPos, glm::vec3 rotation)
{
    for (int i = 0; i < vertices.size(); i++)
    {
        pos = newPos;
        rot = rotation;
        vertices[i].x = (identity[i].x * (cos(rotation.y) * cos(rotation.z))) + (identity[i].y * ((sin(rotation.x) * sin(rotation.y) * cos(rotation.z)) - (cos(rotation.x) * sin(rotation.z)))) + (identity[i].z * ((cos(rotation.x) * sin(rotation.y) * cos(rotation.z)) + (sin(rotation.x) * sin(rotation.z))));
        vertices[i].y = (identity[i].x * (cos(rotation.y) * sin(rotation.z))) + (identity[i].y * ((sin(rotation.x) * sin(rotation.y) * sin(rotation.z)) + (cos(rotation.x) * cos(rotation.z)))) + (identity[i].z * ((cos(rotation.x) * sin(rotation.y) * sin(rotation.z)) - (sin(rotation.x) * cos(rotation.z))));
        vertices[i].z = (identity[i].x * (-sin(rotation.y))) + (identity[i].y * (sin(rotation.x) * cos(rotation.y))) + (identity[i].z * (cos(rotation.x) * cos(rotation.y)));


        vertices[i] = vertices[i] + newPos;
    }
}
glm::vec3 MeshCollider::FindFurthestVertex(glm::vec3 direction)
{
    glm::vec3 maxPoint = glm::vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
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



//RUNS BASIC COLLSION TEST FOR GJK ALGORITHM
bool GJK(MeshCollider& collider1, MeshCollider& collider2)
{
    Simplex points;
    glm::vec3 direction = collider1.pos - collider2.pos;
    glm::vec3 supportPoint = Support(collider1, collider2, direction);
    direction = -supportPoint;

    while (true)
    {
        supportPoint = Support(collider1, collider2, direction);
        if (dot(supportPoint, direction) < 0)
        {
            return false;
        }
        points.push_front(supportPoint);
        if (NextSimplex(points, direction))
        {
            return true;
        }
    }
}