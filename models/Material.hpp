#pragma once

#include <glm/vec3.hpp>

/* Wavefront MTL PBR Extension */
/* https://web.archive.org/web/20230210121526/http://exocortex.com/blog/extending_wavefront_mtl_to_support_pbr */
struct Material
{
    // Legacy Material Properties
    glm::vec3 Ka {0.0f}; // Ambient (NOT SUPPORTED IN RENDER)
    glm::vec3 Kd {1.0f}; // Diffuse / Base color 
    glm::vec3 Ks {0.0f}; // Specular (NOT SUPPORTED IN RENDER)
    glm::vec3 Ke {0.0f}; // Emissive (NOT SUPPORTED IN RENDER)
    glm::vec3 Tf {0.0f}; // Transmission filter (NOT SUPPORTED IN RENDER)

    float Ns = 0.0f;     // Shininess (NOT SUPPORTED IN RENDER)
    float Ni = 1.0f;     // Index of refraction (NOT SUPPORTED IN RENDER)
    float d = 1.0f;      // Opacity (NOT SUPPORTED IN RENDER)
    int illum = 2;       // Illumination model (NOT SUPPORTED IN RENDER)

    // PBR Properties
    float Pr = 1.0f;     // Roughness
    float Pm = 0.0f;     // Metallic
    float Ps = 0.0f;     // Sheen (NOT SUPPORTED IN RENDER)
    float Pc = 0.0f;     // Clearcoat (NOT SUPPORTED IN RENDER)
    float Pcr = 0.0f;    // Clearcoat roughness (NOT SUPPORTED IN RENDER)
    float aniso = 0.0f;  // Anisotropy (NOT SUPPORTED IN RENDER)
    float anisor = 0.0f; // Anisotropy rotation (NOT SUPPORTED IN RENDER)

    // Legacy Texture Maps
    int map_Ka_index = -1;
    int map_Kd_index = -1;
    int map_Ks_index = -1;
    int map_Ns_index = -1;
    int map_d_index = -1;
    int map_bump_index = -1;
    int map_disp_index = -1;
    int map_refl_index = -1;

    // PBR Texture Maps
    int map_Pr_index = -1;
    int map_Pm_index = -1;
    int map_Ps_index = -1;
    int map_Ke_index = -1;
    int map_norm_index = -1;
};

