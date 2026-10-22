#pragma once

#include <vector>
#include <stddef.h>

/* 
    Loaded Texture MetaData from File
*/

enum TextureDataFormat {
    UNSIGNED_CHAR,
    // Could be expanded to include HDR float32...
};

struct TextureMetaData
{
    size_t width; 
    size_t height; 

    size_t channels;
    TextureDataFormat format = TextureDataFormat::UNSIGNED_CHAR;
};



/*
    Texture Container Classes
*/

struct Texture2D
{
    TextureMetaData metaData; 
    std::vector<unsigned char> data;
};

struct TextureCubeMap
{
    // All images must be same deminsion and format
    TextureMetaData metaData; 
    std::vector<unsigned char> dataPosX;
    std::vector<unsigned char> dataNegX;
    std::vector<unsigned char> dataPosY;
    std::vector<unsigned char> dataNegY;
    std::vector<unsigned char> dataPosZ;
    std::vector<unsigned char> dataNegZ;
};
