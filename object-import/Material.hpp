#pragma once

#include <memory>

#include <glm/vec3.hpp>

#include "Textures.hpp"

/*
    Supported Material Formats 
    -> At the moment not every material property has actually been implemented in the rendering pipeline
    it is just too much manual labor for me at the moment, but in fairness alot of the parameters are
    unused by most models anyways.
*/

enum MaterialFormat {
    MTL,
    MTL_PBR,
    //... Could be expanded to include other formats: ie Disney PBR, Blender Principled, etc 
};

/* Traditional Wavefront MTL Material */
/* https://paulbourke.net/dataformats/mtl/ */
struct Material_Mtl
{
    // Material Properties
    glm::vec3 Ka {0.0f}; // Ambient
    glm::vec3 Kd {1.0f}; // Diffuse
    glm::vec3 Ks {0.0f}; // Specular
    glm::vec3 Tf {0.0f}; // Transmission filter (NOT SUPPORTED IN RENDER)

    float Ns = 0.0f;     // Shininess
    float Ni = 1.0f;     // Index of refraction (NOT SUPPORTED IN RENDER)
    float d = 1.0f;      // Opacity (NOT SUPPORTED IN RENDER)
    int illum = 2;       // Illumination model (NOT SUPPORTED IN RENDER)

    // Texture Maps
    std::shared_ptr<Texture2D> map_Ka;
    std::shared_ptr<Texture2D> map_Kd;
    std::shared_ptr<Texture2D> map_Ks;
    std::shared_ptr<Texture2D> map_Ns;
    std::shared_ptr<Texture2D> map_d;
    std::shared_ptr<Texture2D> bump;
    std::shared_ptr<Texture2D> disp;
    std::shared_ptr<Texture2D> refl;
};

/* Wavefront MTL PBR Extension */
/* https://web.archive.org/web/20230210121526/http://exocortex.com/blog/extending_wavefront_mtl_to_support_pbr */
struct Material_MtlPbr
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
    std::shared_ptr<Texture2D> map_Ka;
    std::shared_ptr<Texture2D> map_Kd;
    std::shared_ptr<Texture2D> map_Ks;
    std::shared_ptr<Texture2D> map_Ns;
    std::shared_ptr<Texture2D> map_d;
    std::shared_ptr<Texture2D> bump;
    std::shared_ptr<Texture2D> disp;
    std::shared_ptr<Texture2D> refl;

    // PBR Texture Maps
    std::shared_ptr<Texture2D> map_Pr;
    std::shared_ptr<Texture2D> map_Pm;
    std::shared_ptr<Texture2D> map_Ps;
    std::shared_ptr<Texture2D> map_Ke;
    std::shared_ptr<Texture2D> norm;
};

//.. Define other material formats here

/*
    Composition-Based Material Base Class
    -> Enum of specific format is provided in constructor for polymorphism: 
    -> Material mtl_material    = Material(MaterialFormat::MTL)
    -> Material mtlPbr_material = Material(MaterialFormat::MTL_PBR)
*/

struct Material
{
    MaterialFormat format;

    union Data
    {
        Material_Mtl mtl;
        Material_MtlPbr mtlPbr;

        Data()  {}
        ~Data() {}
    } data;

    // Constructor: Material mtl_material = Material(MaterialFormat::MTL)
    explicit Material(MaterialFormat type) : format(type)
    {
        switch (format)
        {
            case MTL:
                new (&data.mtl) Material_Mtl();
                break;

            case MTL_PBR:
                new (&data.mtlPbr) Material_MtlPbr();
                break;
        }
    }

    // Destroy Specific Format Specified in Constructor
    ~Material()
    {
        switch (format)
        {
            case MTL:
                data.mtl.~Material_Mtl();
                break;

            case MTL_PBR:
                data.mtlPbr.~Material_MtlPbr();
                break;
        }
    }

    // Because of the union these are ambigous and dangerous
    Material(const Material&) = delete;
    Material& operator=(const Material&) = delete;
    Material(Material&&) = delete;
    Material& operator=(Material&&) = delete;
};
