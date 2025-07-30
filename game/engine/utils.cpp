#include "utils.hpp"

std::map<std::string, Texture2D> AssetManager::loadedTextures;

Texture2D AssetManager::loadTexture(std::string path) {
    if (loadedTextures.contains(path)) {
        return loadedTextures[path];
    }

    Texture2D texture = LoadTexture((ASSETS_ROOT + path).c_str());
    loadedTextures[path] = texture;
    return texture;
}

void AssetManager::unloadTextures() {
    for (auto& texture : loadedTextures) {
        UnloadTexture(texture.second);
    }
}