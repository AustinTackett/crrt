#pragma once

#include <memory>
#include <vector>

#include "Textures.hpp"
#include "TriMesh.hpp"
#include "Material.hpp"

/*
    Model parent class that can hold multiple meshes, materials, and other parameters
*/

// Materials can store indices to Textures in the Model class
// TriMeshes store indices to Materials in Model class

struct Model
{
    std::vector<std::shared_ptr<TriMesh>>   triMeshes;
    std::vector<std::shared_ptr<Material>>  materials;
    std::vector<std::shared_ptr<Texture2D>> textures; 
};
