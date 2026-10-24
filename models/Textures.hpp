#pragma once

#include <vector>
#include <stddef.h>

/* 
    Loaded Texture MetaData from File
*/

struct TextureMetaData
{
    enum Format {
        RGBA8,
        //.. Expand to HDR float32 someday?  
    };
    size_t width    = 0;
    size_t height   = 0;
    size_t channels = 4;

    Format format = RGBA8;
};


/*
    Texture Container Classes
*/

struct Texture2D
{
    TextureMetaData metaData; 

    // Byte Arrays
    std::vector<unsigned char> data;
};
