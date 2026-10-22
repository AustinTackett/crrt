#pragma once

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

/* 
    Triangle Mesh Container Classes
*/

struct TriMesh
{
    // Vertex Data
    std::vector<glm::vec3>    positions;
    std::vector<glm::vec3>    normals;
    std::vector<glm::vec2>    texCoords;
    std::vector<unsigned int> indices;

    // Index of Corresponeding in Model Struct
    int materialIndex = -1;
};
