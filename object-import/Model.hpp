#pragma once

#include <memory>
#include <vector>

#include "TriMesh.hpp"
#include "Material.hpp"

/*
    Model parent class that can hold multiple meshes, materials, and other parameters
*/

struct Model
{
    std::vector<std::shared_ptr<TriMesh>>  triMeshes;
    std::vector<std::shared_ptr<Material>> materials;
    //... More Configurable Parameters to be Added
};