#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <stack>

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Material.hpp"
#include "Textures.hpp"
#include "TriMesh.hpp"
#include "Model.hpp"

/*

    Class Holds all Functions for Importing Models, Textures, etc

*/

struct Importer
{
    /* 
        Caching 
    */

    // Stored File Path of Cached Prior Imports
    static std::unordered_map<std::string, std::shared_ptr<Model>>          pathToModel;
    static std::unordered_map<std::string, std::shared_ptr<Texture2D>>      pathToTexture2D;
    static std::unordered_map<std::string, std::shared_ptr<TextureCubeMap>> pathToTextureCubeMap;

    // Check if File Path has been Cached
    static bool hasCachedModel(const std::string& filePath)
    {
        return pathToModel.find(filePath) != pathToModel.end();
    }
    static bool hasCachedTexture2D(const std::string& filePath)
    {
        return pathToTexture2D.find(filePath) != pathToTexture2D.end();
    }
    static bool hasCachedTextureCubeMap(const std::string& filePath)
    {
        return pathToTextureCubeMap.find(filePath) != pathToTextureCubeMap.end();
    }

    /* 
        Texture Loading
    */

    static std::shared_ptr<Texture2D> LoadTexture2D(const std::string& filePath, bool getFromCache=true, bool cacheResult=true)
    {
        if (getFromCache && hasCachedTexture2D(filePath))
        {
            return pathToTexture2D[filePath];
        }
        
        int width, height, channels;
        unsigned char* data = stbi_load(
            filePath.c_str(),
            &width,
            &height,
            &channels,
            0
        );

        if (!data) 
        {
            stbi_image_free(data);
            throw std::runtime_error("Failed to load texture: " + filePath);
        }

        size_t dataCount = static_cast<size_t>(width) * static_cast<size_t>(height) * static_cast<size_t>(channels);

        std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>();
        texture->data.assign(data, data + dataCount);
        texture->metaData.width = width;
        texture->metaData.height = height;
        texture->metaData.channels = channels;
        texture->metaData.format = TextureDataFormat::UNSIGNED_CHAR;
        stbi_image_free(data);

        if (cacheResult && !hasCachedTexture2D(filePath))
        {
            pathToTexture2D[filePath] = texture;
        }

        return texture;
    }

    static std::shared_ptr<TextureCubeMap> LoadCubeMap(
        const std::string& posXFilePath,
        const std::string& negXFilePath,
        const std::string& posYFilePath,
        const std::string& negYFilePath,
        const std::string& posZFilePath,
        const std::string& negZFilePath,
        bool getFromCache = true, 
        bool cacheResult = true)
    {
        std::string combinedFilePath = posXFilePath + "||" + negXFilePath + "||" 
            + posYFilePath + "||" + negYFilePath + "||" + posZFilePath + "||" + negZFilePath;
        if(getFromCache && hasCachedTextureCubeMap(combinedFilePath))
        {
            auto texture = pathToTextureCubeMap[combinedFilePath];
            return texture;
        }

        int posXWidth = 0, posXHeight = 0, posXChannels = 0;
        int negXWidth = 0, negXHeight = 0, negXChannels = 0;
        int posYWidth = 0, posYHeight = 0, posYChannels = 0;
        int negYWidth = 0, negYHeight = 0, negYChannels = 0;
        int posZWidth = 0, posZHeight = 0, posZChannels = 0;
        int negZWidth = 0, negZHeight = 0, negZChannels = 0;

        unsigned char* posXData = stbi_load(posXFilePath.c_str(), &posXWidth, &posXHeight, &posXChannels, 0);
        unsigned char* negXData = stbi_load(negXFilePath.c_str(), &negXWidth, &negXHeight, &negXChannels, 0);
        unsigned char* posYData = stbi_load(posYFilePath.c_str(), &posYWidth, &posYHeight, &posYChannels, 0);
        unsigned char* negYData = stbi_load(negYFilePath.c_str(), &negYWidth, &negYHeight, &negYChannels, 0);
        unsigned char* posZData = stbi_load(posZFilePath.c_str(), &posZWidth, &posZHeight, &posZChannels, 0);
        unsigned char* negZData = stbi_load(negZFilePath.c_str(), &negZWidth, &negZHeight, &negZChannels, 0);
        
        auto freeTextureData = [&]() -> void
        {
            stbi_image_free(posXData);
            stbi_image_free(negXData);
            stbi_image_free(posYData);
            stbi_image_free(negYData);
            stbi_image_free(posZData);
            stbi_image_free(negZData);
        };

        // Check all images loaded successfully
        if (!posXData || !negXData || !posYData || !negYData || !posZData || !negZData)
        {
            freeTextureData();
            throw std::runtime_error("Failed to load one or more cube map faces");
        }
        if (!(posXWidth == negXWidth && posXWidth == posYWidth && posXWidth == negYWidth && posXWidth == posZWidth && posXWidth == negZWidth))
        {
            freeTextureData();
            throw std::runtime_error("All cube map face widths must match");
        }
        if (!(posXHeight == negXHeight && posXHeight == posYHeight && posXHeight == negYHeight && posXHeight == posZHeight && posXHeight == negZHeight))
        {
            freeTextureData();
            throw std::runtime_error("All cube map face heights must match");
        }
        if (!(posXChannels == negXChannels && posXChannels == posYChannels && posXChannels == negYChannels && posXChannels == posZChannels && posXChannels == negZChannels))
        {
            freeTextureData();
            throw std::runtime_error("All cube map face channel counts must match");
        }
        if (posXWidth != posXHeight)
        {
            freeTextureData();
            throw std::runtime_error("Cube map faces must be square");
        }

        auto texture = std::make_shared<TextureCubeMap>();

        // Copy image data
        size_t dataCount = static_cast<size_t>(posXWidth) * static_cast<size_t>(posXHeight) * static_cast<size_t>(posXChannels);
        texture->dataPosX.assign(posXData, posXData + dataCount);
        texture->dataNegX.assign(negXData, negXData + dataCount);
        texture->dataPosY.assign(posYData, posYData + dataCount);
        texture->dataNegY.assign(negYData, negYData + dataCount);
        texture->dataPosZ.assign(posZData, posZData + dataCount);
        texture->dataNegZ.assign(negZData, negZData + dataCount);

        // Set metadata
        texture->metaData.width    = posXWidth;
        texture->metaData.height   = posXHeight;
        texture->metaData.channels = posXChannels;
        texture->metaData.format   = TextureDataFormat::UNSIGNED_CHAR;

        freeTextureData();
        
        if (cacheResult && !hasCachedTextureCubeMap(combinedFilePath))
        {
            pathToTextureCubeMap[combinedFilePath] = texture;
        }
        return texture;
    }

    /* 
        Model Loading
    */
    static std::shared_ptr<Model> LoadModel(const std::string& filePath, bool getFromCache = true, bool cacheResult = true)
    {
        if (getFromCache && hasCachedModel(filePath))
        {
            return pathToModel[filePath];
        }

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            filePath, 
            aiProcess_Triangulate | 
            aiProcess_FlipUVs
        );
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
        {
            throw std::runtime_error(importer.GetErrorString());
        }

        
        std::shared_ptr<Model> model = std::make_shared<Model>();
        model->materials.reserve(scene->mNumMaterials);
        model->triMeshes.reserve(scene->mNumMeshes);
        
        // Process materials
        for (unsigned int i = 0; i < scene->mNumMaterials; i++)
        {
            const aiMaterial* material = scene->mMaterials[i];
            model->materials.push_back(processMaterial(material, filePath));
        }

        // Depth first traversal of meses in scene tree
        std::stack<aiNode*> nodeFrontier;
        nodeFrontier.push(scene->mRootNode);
        while(!nodeFrontier.empty())
        {
            aiNode* node = nodeFrontier.top();
            nodeFrontier.pop();

            for (unsigned int i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                model->triMeshes.push_back(processTriMesh(mesh, scene));
            }

            for (unsigned int i = 0; i < node->mNumChildren; i++)
            {
                nodeFrontier.push(node->mChildren[i]);
            }
        }

        if (cacheResult && !hasCachedModel(filePath))
        {
            pathToModel[filePath] = model;
        }

        return model;
    }

    /* Utilities to Load Assimp Objects into Custom Formats */
    private:

    // Assimp Object to Custom TriMesh
    static std::shared_ptr<TriMesh> processTriMesh(const aiMesh* sourceMesh, const aiScene* sourceScene)
    {
        std::shared_ptr<TriMesh> mesh = std::make_shared<TriMesh>();
        mesh->positions.reserve(sourceMesh->mNumVertices);
        mesh->normals.reserve(sourceMesh->mNumVertices);
        mesh->texCoords.reserve(sourceMesh->mNumVertices);
        mesh->indices.reserve(sourceMesh->mNumFaces * 3);

        // Process vertices
        for (unsigned int i = 0; i < sourceMesh->mNumVertices; i++)
        {
            glm::vec3 position(0.0f);
            glm::vec3 normal(0.0f);
            glm::vec2 texCoord(0.0f);

            if (sourceMesh->HasPositions())
            {
                position = glm::vec3(
                    sourceMesh->mVertices[i].x,
                    sourceMesh->mVertices[i].y,
                    sourceMesh->mVertices[i].z
                );
            }

            if (sourceMesh->HasNormals())
            {
                normal = glm::vec3(
                    sourceMesh->mNormals[i].x,
                    sourceMesh->mNormals[i].y,
                    sourceMesh->mNormals[i].z
                );
            }

            if (sourceMesh->HasTextureCoords(0))
            {
                texCoord = glm::vec2(
                    sourceMesh->mTextureCoords[0][i].x,
                    sourceMesh->mTextureCoords[0][i].y
                );
            }

            mesh->positions.push_back(position);
            mesh->normals.push_back(normal);
            mesh->texCoords.push_back(texCoord);
        }

        // Process Indices
        for (unsigned int i = 0; i < sourceMesh->mNumFaces; i++)
        {
            const aiFace& face = sourceMesh->mFaces[i];

            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                mesh->indices.push_back(face.mIndices[j]);
            }
        }

        // Assign material index (-1 is no material)
        if (sourceMesh->mMaterialIndex < sourceScene->mNumMaterials){
            mesh->materialIndex = static_cast<int>(sourceMesh->mMaterialIndex);
        } else {
            mesh->materialIndex = -1;     
        }

        return mesh;
    }

    // Assimp Material to Custom Material
    static std::shared_ptr<Material> processMaterial(const aiMaterial* sourceMaterial, const std::string& modelPath)
    {
         // Model has No Fixed Material Format Property so We Need to Find it Ourselves
        auto getMaterialFormat = [](const aiMaterial* sourceMaterial) -> MaterialFormat
        {
            float value;
            bool isMtlPbr = sourceMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, value) == AI_SUCCESS ||
                sourceMaterial->Get(AI_MATKEY_METALLIC_FACTOR, value) == AI_SUCCESS ||
                sourceMaterial->GetTextureCount(aiTextureType_BASE_COLOR) > 0 ||
                sourceMaterial->GetTextureCount(aiTextureType_METALNESS) > 0 ||
                sourceMaterial->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0;

            if (isMtlPbr) {
                return MaterialFormat::MTL_PBR;
            } else {
                return MaterialFormat::MTL;
            }
        };

        // Process Material of Specific Format
        switch(getMaterialFormat(sourceMaterial)) {
            case MaterialFormat::MTL_PBR:
                return processMaterial_MtlPbr(sourceMaterial, modelPath);
                break;
            case MaterialFormat::MTL:
                return processMaterial_Mtl(sourceMaterial, modelPath);
                break;
            default:
                return processMaterial_Mtl(sourceMaterial, modelPath);
                break;
        }
    }

    // Load Assimp Material as Mtl-Pbr Format
    static std::shared_ptr<Material> processMaterial_MtlPbr(const aiMaterial* sourceMaterial, const std::string& modelPath)
    {
        auto material = std::make_shared<Material>(MaterialFormat::MTL_PBR);

        // Legacy Material Properties
        aiColor3D color;
        float value;
        if (sourceMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
            material->data.mtlPbr.Ka = glm::vec3(color.r, color.g, color.b);
        }
        if (sourceMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
            material->data.mtlPbr.Kd = glm::vec3(color.r, color.g, color.b);
        }
        if (sourceMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
            material->data.mtlPbr.Ks = glm::vec3(color.r, color.g, color.b);
        }
        if (sourceMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS) {
            material->data.mtlPbr.Ke = glm::vec3(color.r, color.g, color.b);
        }
        if (sourceMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, color) == AI_SUCCESS) {
            material->data.mtlPbr.Tf = glm::vec3(color.r, color.g, color.b);
        }
        if (sourceMaterial->Get(AI_MATKEY_SHININESS, value) == AI_SUCCESS) {
            material->data.mtlPbr.Ns = value;
        }
        if (sourceMaterial->Get(AI_MATKEY_REFRACTI, value) == AI_SUCCESS) {
            material->data.mtlPbr.Ni = value;
        }
        if (sourceMaterial->Get(AI_MATKEY_OPACITY, value) == AI_SUCCESS) {
            material->data.mtlPbr.d = value;
        }

        // PBR Material Properties
        aiColor4D baseColor;
        if (sourceMaterial->Get(AI_MATKEY_BASE_COLOR, baseColor) == AI_SUCCESS) {
            material->data.mtlPbr.Kd = glm::vec3(baseColor.r, baseColor.g, baseColor.b);
            material->data.mtlPbr.d *= baseColor.a;
        }
        if (sourceMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, value) == AI_SUCCESS) {
            material->data.mtlPbr.Pr = value;
        }
        if (sourceMaterial->Get(AI_MATKEY_METALLIC_FACTOR, value) == AI_SUCCESS) {
            material->data.mtlPbr.Pm = value;
        }

        auto loadMaterialTexture = [&](aiTextureType type) -> std::shared_ptr<Texture2D> {
            std::string filePath;
            aiString aiFilePath;

            if (sourceMaterial->GetTexture(type, 0, &aiFilePath) == AI_SUCCESS) {
                filePath = std::string(aiFilePath.C_Str());
            }

            if (filePath.empty()) {
                return nullptr;
            }
            if (filePath[0] == '*') {
                throw std::runtime_error("Embedded textures not supported: " + filePath);
            }

            return LoadTexture2D(filePath);
        };

        // Legacy Texture Maps
        material->data.mtlPbr.map_Kd = loadMaterialTexture(aiTextureType_BASE_COLOR);
        material->data.mtlPbr.map_Ka = loadMaterialTexture(aiTextureType_AMBIENT);
        material->data.mtlPbr.map_Ks = loadMaterialTexture(aiTextureType_SPECULAR);
        material->data.mtlPbr.map_Ns = loadMaterialTexture(aiTextureType_SHININESS);
        material->data.mtlPbr.map_d  = loadMaterialTexture(aiTextureType_OPACITY);
        material->data.mtlPbr.bump   = loadMaterialTexture(aiTextureType_HEIGHT);
        material->data.mtlPbr.disp   = loadMaterialTexture(aiTextureType_DISPLACEMENT);
        material->data.mtlPbr.refl   = loadMaterialTexture(aiTextureType_REFLECTION);

        // PBR Texture Maps
        material->data.mtlPbr.map_Pr = loadMaterialTexture(aiTextureType_DIFFUSE_ROUGHNESS);
        material->data.mtlPbr.map_Pm = loadMaterialTexture(aiTextureType_METALNESS);
        material->data.mtlPbr.map_Ke = loadMaterialTexture(aiTextureType_EMISSION_COLOR);
        material->data.mtlPbr.norm   = loadMaterialTexture(aiTextureType_NORMALS);

        if (!material->data.mtlPbr.map_Kd)
            material->data.mtlPbr.map_Kd = loadMaterialTexture(aiTextureType_DIFFUSE);

        if (!material->data.mtlPbr.map_Ke)
            material->data.mtlPbr.map_Ke = loadMaterialTexture(aiTextureType_EMISSIVE);

        return material;
    }

    // Load Assimp Material as MtlPbr Format
    static std::shared_ptr<Material> processMaterial_Mtl(const aiMaterial* sourceMaterial, const std::string& modelPath)
    {
        auto material = std::make_shared<Material>(MaterialFormat::MTL);

        // Legacy Material Properties
        aiColor3D color;
        float value;
        if (sourceMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
            material->data.mtl.Ka = glm::vec3(color.r, color.g, color.b);
        }
        if (sourceMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
            material->data.mtl.Kd = glm::vec3(color.r, color.g, color.b);
        }
        if (sourceMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
            material->data.mtl.Ks = glm::vec3(color.r, color.g, color.b);
        }
        if (sourceMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, color) == AI_SUCCESS) {
            material->data.mtl.Tf = glm::vec3(color.r, color.g, color.b);
        }
        if (sourceMaterial->Get(AI_MATKEY_SHININESS, value) == AI_SUCCESS) {
            material->data.mtl.Ns = value;
        }
        if (sourceMaterial->Get(AI_MATKEY_REFRACTI, value) == AI_SUCCESS) {
            material->data.mtl.Ni = value;
        }
        if (sourceMaterial->Get(AI_MATKEY_OPACITY, value) == AI_SUCCESS) {
            material->data.mtl.d = value;
        }

        // Load Material Textures
        auto loadMaterialTexture = [&](aiTextureType type) -> std::shared_ptr<Texture2D> {
            std::string filePath;
            aiString aiFilePath;

            if (sourceMaterial->GetTexture(type, 0, &aiFilePath) == AI_SUCCESS) {
                filePath = std::string(aiFilePath.C_Str());
            }

            if (filePath.empty()) {
                return nullptr;
            }
            if (filePath[0] == '*') {
                throw std::runtime_error("Embedded textures not supported: " + filePath);
            }

            return LoadTexture2D(filePath);
        };

        // Legacy Texture Maps
        material->data.mtl.map_Ka = loadMaterialTexture(aiTextureType_AMBIENT);
        material->data.mtl.map_Kd = loadMaterialTexture(aiTextureType_DIFFUSE);
        material->data.mtl.map_Ks = loadMaterialTexture(aiTextureType_SPECULAR);
        material->data.mtl.map_Ns = loadMaterialTexture(aiTextureType_SHININESS);
        material->data.mtl.map_d  = loadMaterialTexture(aiTextureType_OPACITY);
        material->data.mtl.bump   = loadMaterialTexture(aiTextureType_HEIGHT);
        material->data.mtl.disp   = loadMaterialTexture(aiTextureType_DISPLACEMENT);
        material->data.mtl.refl   = loadMaterialTexture(aiTextureType_REFLECTION);

        return material;
    }

};
