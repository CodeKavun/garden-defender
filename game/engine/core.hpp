#include <raylib.h>
#include <string>
#include "utils.hpp"
#include "gfx.hpp"

class Game
{
protected:
    std::string title;
    int width;
    int height;

    TextureAtlas* atlas;
public:
    Game(std::string title, int width, int height) : title(title), width(width), height(height) {}

    std::string getTitle() const { return title; }

    int getWidth() const { return width; }
    int getHeight() const { return height; }

    void run();
    void update();
    void draw();
};
