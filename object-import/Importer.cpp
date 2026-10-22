#include "Importer.hpp"

std::unordered_map<std::string, std::shared_ptr<Model>>          Importer::pathToModel{};
std::unordered_map<std::string, std::shared_ptr<Texture2D>>      Importer::pathToTexture2D{};
std::unordered_map<std::string, std::shared_ptr<TextureCubeMap>> Importer::pathToTextureCubeMap{};