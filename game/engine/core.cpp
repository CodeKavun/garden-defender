#include "core.hpp"

void Game::run()
{
    InitWindow(width, height, title.c_str());

    Texture2D gardenTilesetTex = AssetManager::loadTexture("GardenTS.png");
    atlas = new TextureAtlas(48, 32, 16, 16);

    while (!WindowShouldClose()) {
        update();
        draw();
    }

    AssetManager::unloadTextures();
    CloseWindow();
}

void Game::update()
{
}

void Game::draw()
{
    BeginDrawing();

    ClearBackground(RAYWHITE);

    // DrawTextureRec(AssetManager::loadTexture("GardenTS.png"), atlas->getRegion(0), { 20, 20 }, WHITE);

    EndDrawing();
}
