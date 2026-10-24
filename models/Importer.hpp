#pragma once

#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/config.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <stb_image.h>

#include "Model.hpp"
#include "Textures.hpp"
#include "assimp/mesh.h"

class Importer
{
    private: 

    /*
        Helper Class to Track Previously Loaded Textures
    */

    struct Texture2DCache
    {
        Texture2DCache() = delete;
        Texture2DCache(std::string modelPath) : basePath(modelPath) {}

        std::string basePath;
        std::unordered_map<std::string, std::shared_ptr<Texture2D>> files;

        void Load(const std::string& texturePath, std::shared_ptr<Texture2D> texture)
        {
            files.emplace(texturePath, texture);
        }

        std::shared_ptr<Texture2D> Get(const std::string& texturePath)
        {
            auto found = files.find(texturePath);
            if (found != files.end()) { return found->second; }
            else { return nullptr; }
        }
    };

    public:

    /*
        Public Loaders to Create Native Engine Objects from Filepaths
    */

    static bool ParseFile(std::string filePath, std::string& outString)
    {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return false;
        }

        std::string tempStringLine;
        outString.clear();
        while (std::getline(file, tempStringLine)) {
            outString += tempStringLine;
            outString += '\n';
        }
        return true;
    }

    // Loads as static object 
    static std::shared_ptr<Model> LoadModel(const std::string& filePath)
    {
        Assimp::Importer importer;

        importer.SetPropertyInteger(
            AI_CONFIG_PP_SBP_REMOVE, 
            aiPrimitiveType_POINT | 
            aiPrimitiveType_LINE);
        const aiScene* scene = importer.ReadFile(filePath,
            aiProcess_Triangulate |
            aiProcess_SortByPType |
            aiProcess_JoinIdenticalVertices |
            aiProcess_GenSmoothNormals |
            aiProcess_FlipUVs |
            aiProcess_PreTransformVertices |
            aiProcess_ValidateDataStructure
        );

        if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
            throw std::runtime_error("Failed to load model " + filePath + ": " + importer.GetErrorString());
        }

        auto model = std::make_shared<Model>();
        model->textures.reserve(scene->mNumTextures);
        model->materials.reserve(scene->mNumMaterials);
        model->triMeshes.reserve(scene->mNumMeshes);

        // Preserve embedded texture indices. External textures are appended later.
        Texture2DCache textureCache{filePath};
        for (unsigned int i = 0; i < scene->mNumTextures; i++) {
            ProcessEmbeddedTexture(scene->mTextures[i], *scene, *model, textureCache);
        }
        for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
            ProcessMaterial(scene->mMaterials[i], *scene, *model, textureCache);
        }
        
        auto isTriangleMesh = [](aiMesh* mesh) -> bool { 
            return mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE; 
        };
        for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
            if (!isTriangleMesh(scene->mMeshes[i])) { continue; } 
            ProcessTriMesh(scene->mMeshes[i], *model);
        }

        return model;
    }

    static std::shared_ptr<Texture2D> LoadTexture2D(const std::string& filePath)
    {
        int width = 0, height = 0, channels = 4;
        unsigned char* data = stbi_load(
            filePath.c_str(),
            &width,
            &height,
            nullptr,
            channels
        );

        if (!data) {
            stbi_image_free(data);
            throw std::runtime_error("Failed to load texture: " + filePath);
        }

        std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>();
        size_t dataCount = static_cast<size_t>(width) * static_cast<size_t>(height) * static_cast<size_t>(channels);        
        texture->data.assign(data, data + dataCount);
        texture->metaData.width = width;
        texture->metaData.height = height;
        texture->metaData.channels = channels;
        texture->metaData.format = TextureMetaData::RGBA8;
        stbi_image_free(data);

        return texture;
    }

    private:

    /*
        Geometry Loading
    */

    static void ProcessTriMesh(const aiMesh* sourceMesh, Model& model)
    {
        if (!sourceMesh->HasPositions() || !sourceMesh->HasNormals()) {
            throw std::runtime_error("Triangle mesh is missing positions or normals");
        }

        auto mesh = std::make_shared<TriMesh>();
        mesh->positions.reserve(sourceMesh->mNumVertices);
        mesh->normals.reserve(sourceMesh->mNumVertices);
        mesh->texCoords.reserve(sourceMesh->mNumVertices);
        mesh->indices.reserve(static_cast<size_t>(sourceMesh->mNumFaces) * 3);

        const bool hasUVs = sourceMesh->HasTextureCoords(0);
        for (unsigned int i = 0; i < sourceMesh->mNumVertices; i++) {
            mesh->positions.emplace_back(glm::vec3(
                sourceMesh->mVertices[i].x, 
                sourceMesh->mVertices[i].y, 
                sourceMesh->mVertices[i].z)
            );
            mesh->normals.emplace_back(glm::vec3(
                sourceMesh->mNormals[i].x,
                sourceMesh->mNormals[i].y,
                sourceMesh->mNormals[i].z)
            );
            if (hasUVs) { 
                mesh->texCoords.push_back(glm::vec2(
                    sourceMesh->mTextureCoords[0][i].x, 
                    sourceMesh->mTextureCoords[0][i].y)
                ); 
            }
        }

        auto checkTriangleFace = [](aiFace& face) -> void { 
            if (face.mNumIndices != 3) { throw std::runtime_error("Expected a triangle face"); }
        };
        for (unsigned int i = 0; i < sourceMesh->mNumFaces; i++) {
            checkTriangleFace(sourceMesh->mFaces[i]);
            for (unsigned int j = 0; j < 3; j++) {
                mesh->indices.push_back(sourceMesh->mFaces[i].mIndices[j]);
            }
        }

        mesh->materialIndex = static_cast<int>(sourceMesh->mMaterialIndex);
        model.triMeshes.push_back(mesh);
    }

    /*
        Texture Loading
    */

    // Returns an empty key when the reference is not an embedded texture.
    static std::string GetEmbeddedTexturePath(const aiScene& sourceScene, const aiTexture* sourceTexture)
    {
        if (!sourceTexture) { return {}; }
        for (unsigned int i = 0; i < sourceScene.mNumTextures; i++) {
            if (sourceScene.mTextures[i] == sourceTexture) {
                return "*" + std::to_string(i);
            }
        }
        throw std::runtime_error("Embedded texture not found in scene");
    }

    static std::string GetExternalTexturePath(const std::string& filePath, const std::string& modelPath)
    {
        std::filesystem::path texturePath(filePath);
        if (texturePath.is_relative()) {
            texturePath = std::filesystem::path(modelPath).parent_path() / texturePath;
        }
        return std::filesystem::weakly_canonical(
            std::filesystem::absolute(texturePath)).generic_string();
    }

    static void ProcessEmbeddedTexture(const aiTexture* sourceTexture, const aiScene& sourceScene, Model& model, Texture2DCache& cache)
    {
        if (!sourceTexture || !sourceTexture->pcData) { 
            throw std::runtime_error("Invalid embedded texture"); 
        }
        std::string key = GetEmbeddedTexturePath(sourceScene, sourceTexture);
        
        std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>();
        if (sourceTexture->mHeight == 0) {
            // Load image data (PNG, JPG, etc)
            int width = 0, height = 0, channels = 4;
            unsigned char* data = stbi_load_from_memory(
                reinterpret_cast<const unsigned char*>(sourceTexture->pcData),
                static_cast<int>(sourceTexture->mWidth), 
                &width, 
                &height, 
                nullptr, 
                channels);
            if (!data) {
                throw std::runtime_error("Failed to decode embedded texture");
            }

            size_t dataCount = static_cast<size_t>(width) * static_cast<size_t>(height) * static_cast<size_t>(channels);        
            texture->data.assign(data, data + dataCount);
            texture->metaData.width = width;
            texture->metaData.height = height;
            texture->metaData.channels = channels;
            texture->metaData.format = TextureMetaData::RGBA8;
            stbi_image_free(data);
            
            model.textures.push_back(texture);
        } else {
            // Load already decoded string of RGBA data
            int width = sourceTexture->mWidth, height = sourceTexture->mHeight, channels = 4;
            texture->data.resize(static_cast<size_t>(width) * static_cast<size_t>(height) * channels);
            for (size_t i = 0; i < texture->data.size() / 4; i++) {
                const aiTexel& pixel = sourceTexture->pcData[i];
                texture->data[i * 4 + 0] = pixel.r;
                texture->data[i * 4 + 1] = pixel.g;
                texture->data[i * 4 + 2] = pixel.b;
                texture->data[i * 4 + 3] = pixel.a; 
            }
            texture->metaData.width = sourceTexture->mWidth;
            texture->metaData.height = sourceTexture->mHeight;
            texture->metaData.channels = 4;
            texture->metaData.format = TextureMetaData::RGBA8;
            model.textures.push_back(texture);
        }

        cache.Load(key, texture);
    }

    static void ProcessMaterial(const aiMaterial* sourceMaterial, const aiScene& sourceScene, Model& model, Texture2DCache& cache)
    {
        auto material = std::make_shared<Material>();

        auto loadMaterialTexture = [&](aiTextureType type) -> int {
            // Get texture if filepath exists else return texture index of -1
            std::string filePath;
            aiString    aiFilePath;
            if (sourceMaterial->GetTexture(type, 0, &aiFilePath) == AI_SUCCESS) {
                filePath = std::string(aiFilePath.C_Str());
            }
            if (filePath.empty()) {
                return -1;
            }
            
            std::shared_ptr<Texture2D> texture;
            std::string embeddedPath = GetEmbeddedTexturePath(sourceScene, sourceScene.GetEmbeddedTexture(filePath.c_str()));
            if (!embeddedPath.empty()) {
                // Load as Embedded Texture
                texture = cache.Get(embeddedPath);
                if (!texture) { throw std::runtime_error("Embedded texture was not loaded before material was processed: " + filePath); }
            } else {
                // Load as External Texture
                if (filePath[0] == '*') { throw std::runtime_error("External texture has embedded texture file path " + filePath); }
                std::string key = GetExternalTexturePath(filePath, cache.basePath);
                texture = cache.Get(key);
                if (!texture) {
                    texture = LoadTexture2D(key);
                    cache.Load(key, texture);
                }
            }

            // If texture is not already added to list of textures add it and save index
            for (size_t i = 0; i < model.textures.size(); ++i) {
                if (model.textures[i] == texture) { return static_cast<int>(i); }
            }
            model.textures.push_back(texture);
            return static_cast<int>(model.textures.size() - 1);
        };

        // Legacy Material Properties
        aiColor3D color;
        float value;
        if (sourceMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
            material->Ka = glm::vec3(color.r, color.g, color.b);
        }
        if (sourceMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
            material->Kd = glm::vec3(color.r, color.g, color.b);
        }
        if (sourceMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
            material->Ks = glm::vec3(color.r, color.g, color.b);
        }
        if (sourceMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS) {
            material->Ke = glm::vec3(color.r, color.g, color.b);
        }
        if (sourceMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, color) == AI_SUCCESS) {
            material->Tf = glm::vec3(color.r, color.g, color.b);
        }
        if (sourceMaterial->Get(AI_MATKEY_SHININESS, value) == AI_SUCCESS) {
            material->Ns = value;
        }
        if (sourceMaterial->Get(AI_MATKEY_REFRACTI, value) == AI_SUCCESS) {
            material->Ni = value;
        }
        if (sourceMaterial->Get(AI_MATKEY_OPACITY, value) == AI_SUCCESS) {
            material->d = value;
        }

        // PBR Material Properties
        aiColor4D baseColor;
        if (sourceMaterial->Get(AI_MATKEY_BASE_COLOR, baseColor) == AI_SUCCESS) {
            material->Kd = glm::vec3(baseColor.r, baseColor.g, baseColor.b);
            material->d = baseColor.a;
        }
        if (sourceMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, value) == AI_SUCCESS) {
            material->Pr = value;
        }
        if (sourceMaterial->Get(AI_MATKEY_METALLIC_FACTOR, value) == AI_SUCCESS) {
            material->Pm = value;
        }

        // Legacy Texture Maps
        material->map_Kd_index   = loadMaterialTexture(aiTextureType_BASE_COLOR);
        material->map_Ka_index   = loadMaterialTexture(aiTextureType_AMBIENT);
        material->map_Ks_index   = loadMaterialTexture(aiTextureType_SPECULAR);
        material->map_Ns_index   = loadMaterialTexture(aiTextureType_SHININESS);
        material->map_d_index    = loadMaterialTexture(aiTextureType_OPACITY);
        material->map_bump_index = loadMaterialTexture(aiTextureType_HEIGHT);
        material->map_disp_index = loadMaterialTexture(aiTextureType_DISPLACEMENT);
        material->map_refl_index = loadMaterialTexture(aiTextureType_REFLECTION);

        // PBR Texture Maps
        material->map_Pr_index   = loadMaterialTexture(aiTextureType_DIFFUSE_ROUGHNESS);
        material->map_Pm_index   = loadMaterialTexture(aiTextureType_METALNESS);
        material->map_Ke_index   = loadMaterialTexture(aiTextureType_EMISSION_COLOR);
        material->map_norm_index = loadMaterialTexture(aiTextureType_NORMALS);

        // Fall back to legacy texture names
        if (material->map_Kd_index == -1) {
            material->map_Kd_index = loadMaterialTexture(aiTextureType_DIFFUSE);
        }
        if (material->map_Ke_index == -1) {
            material->map_Ke_index = loadMaterialTexture(aiTextureType_EMISSIVE);
        }

        model.materials.push_back(material);
    }

};



