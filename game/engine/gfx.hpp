#pragma once
#include <raylib.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include <memory>
#include <fstream>

class TextureAtlas
{
public:
	TextureAtlas(int width, int height, int regionWidth, int regionHeight);

	inline int getWidth() const { return width; }
	inline int getHeight() const { return height; }
	inline int getRegionWidth() const { return regionWidth; }
	inline int getRegionHeight() const { return regionHeight; }

	inline std::vector<Rectangle> getRegions() { return regions; }
	inline Rectangle getRegion(int index) { return regions[index]; }

	void addRegion(Rectangle source);
	void removeRegion(int index);
	void createGrid();

private:
	int width;
	int height;
	int regionWidth;
	int regionHeight;
	std::vector<Rectangle> regions;
};


// Map-related graphics
struct Tile
{
	int id = 1;
	int regionIndex = 0;

	bool solid = false;

	std::string* soundPath = nullptr;

	float timer = 0;
	int currentTileId = 0;
	float animationDelay = 0;
	std::vector<int> frames;
};

class Tileset
{
public:
	Tileset() {}
	Tileset(std::string name, Texture2D texture, int cellSize);
	~Tileset();

	static Tileset* fromFile(std::string path);

	inline std::string getName() const { return name; }
	inline void setName(std::string name) { this->name = name; }

	inline std::string getTexturePathInfo() const { return texturePathInfo; }
	inline void setTexturePathInfo(std::string path) { texturePathInfo = path; }

	inline Texture2D getTexture() const { return texture; }
	inline void setTexture(Texture2D texture) {
		this->texture = texture;
		generateTiles();
	}

	inline int getCellSize() const { return cellSize; }
	inline void setCellSize(int cellSize) {
		this->cellSize = cellSize;
		generateTiles();
	}

	inline int getRows() { return texture.height / cellSize; }
	inline int getColumns() { return texture.width / cellSize; }

	inline TextureAtlas* getAtlas() { return atlas; }

	inline std::vector<Tile*> getTiles() { return tiles; }
	inline Tile* getTile(int index) { return tiles[index]; }
	inline Tile* getTileById(int id) { return getTile(id - 1); }

	void generateTiles();
	void update();

private:
	std::string name;

	Texture2D texture;
	std::string texturePathInfo;
	int cellSize;

	TextureAtlas* atlas;
	std::vector<Tile*> tiles;
};

enum TileRenderType
{
	TILE_ON_BACK,
	TILE_ON_TOP
};

class TilemapLayer
{
public:
	TilemapLayer(std::string name, Tileset* tileset, int width, int height, int cellSize);

	inline std::string getName() { return name; }
	inline void setName(std::string name) { this->name = name; }

	inline Tileset* getTileset() { return tileset; }
	inline void setTileset(Tileset* tileset) { this->tileset = tileset; }

	inline int getWidth() const { return width; }
	inline int getWidthInPixels() const { return width * cellSize; }
	inline void setWidth(int width) { resize(width, this->height); }

	inline int getHeight() const { return height; }
	inline int getHeightInPixels() const { return height * cellSize; }
	inline void setHeight(int height) { resize(this->width, height); }

	inline int getCellSize() const { return cellSize; }
	inline void setCellSize(int cellSize) { this->cellSize = cellSize; }

	inline int* getLayout() { return layout.get(); }

	int getTile(int row, int col) { return layout[row * this->width + col]; }
	void setTile(int tileId, int row, int col);
	void eraseTile(int row, int col);

	void initLayout();

	void update();
	void draw(const Camera2D& camera, int viewportWidth, int viewportHeight);

private:
	std::string name;

	Tileset* tileset;

	int width;
	int height;
	int cellSize;

	std::unique_ptr<int[]> layout;

	void resize(int width, int height);
};

