#pragma once
#include <raylib.h>
#include <map>
#include <string>

#define ASSETS_ROOT (std::string)"assets/"

class AssetManager
{
private:
    static std::map<std::string, Texture2D> loadedTextures;

public:
    static Texture2D loadTexture(std::string path);

    static void unloadTextures();
};