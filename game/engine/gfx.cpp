#include "gfx.hpp"
#include "utils.hpp"

TextureAtlas::TextureAtlas(int width, int height, int regionWidth, int regionHeight)
{
	this->width = width;
	this->height = height;
	this->regionWidth = regionWidth;
	this->regionHeight = regionHeight;
}

void TextureAtlas::addRegion(Rectangle source)
{
	regions.push_back(source);
}

void TextureAtlas::removeRegion(int index)
{
	regions.erase(regions.begin() + index);
}

void TextureAtlas::createGrid()
{
	int columns = width / regionWidth;
	int rows = height / regionHeight;
	int count = columns * rows;

	for (int i = 0; i < count; i++)
	{
		int x = i % columns * regionWidth;
		int y = i / columns * regionHeight;
		regions.push_back(Rectangle{ (float)x, (float)y, (float)regionWidth, (float)regionHeight });
	}
}

// Tilemaps
Tileset::Tileset(std::string name, Texture2D texture, int cellSize)
{
	this->name = name;
	this->texture = texture;
	this->cellSize = cellSize;
	generateTiles();
}

Tileset::~Tileset()
{
	for (int i = 0; i < tiles.size(); i++) {
		delete[] tiles[i];
	}
}

Tileset* Tileset::fromFile(std::string path)
{
	std::ifstream tilesetFile(path);

	std::string jsonLine;
	std::string jsonString;
	while (tilesetFile >> jsonLine) {
		jsonString += jsonLine;
	}

	nlohmann::json tilesetData = nlohmann::json::parse(jsonString);

	std::string name = tilesetData["name"];
	Texture2D texture = AssetManager::loadTexture(tilesetData["texture"]);
	int cellSize = tilesetData["cellSize"];

	Tileset* tileset = new Tileset(name, texture, cellSize);
	tileset->setTexturePathInfo(tilesetData["texture"]);

	if (tilesetData.contains("tileDatas")) {
		for (auto& item : tilesetData["tileDatas"].items()) {
			Tile* tile = tileset->getTileById(std::stoi(item.key()));

			if (item.value().contains("solid")) tile->solid = item.value()["solid"];
			if (item.value().contains("delay")) tile->animationDelay = item.value()["delay"];
			
			if (item.value().contains("solid")) {
				tile->frames.clear();
				for (auto& frame : item.value()["frames"]) {
					tile->frames.push_back(frame);
				}
			}
		}
	}

	return tileset;
}

void Tileset::generateTiles()
{
	atlas = new TextureAtlas(this->texture.width, this->texture.height, this->cellSize, this->cellSize);
	atlas->createGrid();

	tiles.clear();
	for (int i = 0; i < atlas->getRegions().size(); i++)
	{
		Tile* tile = new Tile;
		tile->regionIndex = i;
		tile->id = i + 1;
		tiles.push_back(tile);
	}
}

void Tileset::update()
{
	for (Tile* tile : tiles) {
		if (tile->frames.empty()) continue;

		tile->timer += GetFrameTime();
		if (tile->timer >= tile->animationDelay) {
			tile->timer = 0;

			tile->currentTileId++;
			if (tile->currentTileId >= tile->frames.size())
				tile->currentTileId = 0;

			tile->regionIndex = tile->frames[tile->currentTileId] - 1;
		}
	}
}

TilemapLayer::TilemapLayer(std::string name, Tileset* tileset, int width, int height, int cellSize)
{
	this->name = name;
	this->tileset = tileset;
	this->width = width;
	this->height = height;
	this->cellSize = cellSize;

	initLayout();
}

void TilemapLayer::setTile(int tileId, int row, int col)
{
	layout[row * width + col] = tileId;
}

void TilemapLayer::eraseTile(int row, int col)
{
	layout[row * width + col] = 0;
}

void TilemapLayer::initLayout()
{
	layout = std::make_unique<int[]>(height * width);

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			layout[i * width + j] = 0;
		}
	}
}

void TilemapLayer::update()
{
	//tileset->update();
}

void TilemapLayer::draw(const Camera2D& camera, int viewportWidth, int viewportHeight)
{
	int startCol = (camera.target.x - camera.offset.x) / cellSize;
	int endCol = (camera.target.x - camera.offset.x + viewportWidth + cellSize) / cellSize;
	int startRow = (camera.target.y - camera.offset.y) / cellSize;
	int endRow = (camera.target.y - camera.offset.y + viewportHeight + cellSize) / cellSize;

	startCol = std::clamp(startCol, 0, width - 1);
	endCol = std::clamp(endCol, 0, width - 1);
	startRow = std::clamp(startRow, 0, height - 1);
	endRow = std::clamp(endRow, 0, height - 1);

	for (int i = startRow; i < endRow; i++)
	{
		for (int j = startCol; j < endCol; j++)
		{
			int id = layout[i * width + j];
			if (id <= 0) continue;

			Tile* tile = tileset->getTileById(id);
			Rectangle tileRect = tileset->getAtlas()->getRegion(tile->regionIndex);

			DrawTextureRec(
				tileset->getTexture(),
				tileRect,
				{ (float)cellSize * j, (float)cellSize * i },
				WHITE
			);
		}
	}
}

void TilemapLayer::resize(int newWidth, int newHeight)
{
	int oldSize = this->height * this->width;
	int newSize = newWidth * newHeight;

	int copiedWidth = std::min(this->width, newWidth);
	int copiedHeight = std::min(this->height, newHeight);

	std::unique_ptr<int[]> newLayout = std::make_unique<int[]>(newSize);

	for (int i = 0; i < copiedHeight; i++) {
		for (int j = 0; j < copiedWidth; j++) {
			newLayout[i * newWidth + j] = getTile(i, j);
		}
	}

	for (int i = oldSize; i < newSize; i++) {
		newLayout[i] = 0;
	}

	this->width = newWidth;
	this->height = newHeight;
	layout = std::move(newLayout);
}

