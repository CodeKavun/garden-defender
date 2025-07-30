#pragma once
#include <raylib.h>
#include <string>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include "sequence.hpp"
#include "gfx.hpp"

class Sprite
{
public:
	Sprite(AnimationPlayer* animPlayer, Vector2 position);
	Sprite(AnimationPlayer* animPlayer, Vector2 position, Vector2 scale);
	Sprite(AnimationPlayer* animPlayer, Vector2 position, Vector2 scale, Vector2 origin, bool centered);

	inline AnimationPlayer* getAnimationPlayer() const { return animPlayer; }
	inline void setAnimationPlayer(AnimationPlayer* animPlayer) { this->animPlayer = animPlayer; }

	inline Vector2 getPosition() const { return position; }
	inline Vector2 getRoundedPosition() { return { (float)(int)std::round(position.x), (float)(int)std::round(position.y) }; }
	inline void setPosition(Vector2 position) { this->position = position; }

	inline Vector2 getScale() const { return scale; }
	inline void setScale(Vector2 scale) { this->scale = scale; }
	inline void setScale(float scale) { this->scale = { scale, scale }; }

	inline Vector2 getOrigin() const { return origin; }
	inline void setOrigin(Vector2 origin) { this->origin = origin; }

	inline bool getFlipX() const { return flipX; }
	inline void setFlipX(bool flipX) { this->flipX = flipX; }

	inline bool getFlipY() const { return flipY; }
	inline void setFlipY(bool flipY) { this->flipY = flipY; }

	inline Shader* getShader() { return shader.get(); }
	inline void setShader(const Shader& shader) { this->shader = std::make_unique<Shader>(shader); }
	inline void unsetShader() {
		shader = nullptr;
		shaderParameterLocations.clear();
	}
	inline void setShaderParameter(std::string name, const void* value, int valueType) {
		if (!shader)
			return;

		if (shaderParameterLocations.count(name) == 0) {
			shaderParameterLocations[name] = GetShaderLocation(*shader.get(), name.c_str());
		}

		SetShaderValue(*shader.get(), shaderParameterLocations[name], value, valueType);
	}

	virtual void update();
	virtual void draw();

protected:
	AnimationPlayer* animPlayer;
	Vector2 position;
	Vector2 scale;
	Vector2 origin;
	bool centered;

	bool flipX = false;
	bool flipY = false;

	std::unique_ptr<Shader> shader;
	std::unordered_map<std::string, int> shaderParameterLocations;
};


class Map
{
public:
	Map(std::string name, int width, int height, int cellSize) : name(name), width(width), height(height), cellSize(cellSize) {}

	static Map* fromFile(std::string path);

	inline std::string getName() const { return name; }
	inline void setName(std::string name) { this->name = name; }

	inline int getWidth() const { return width; }
	inline int getWidthInPixels() const { return width * cellSize; }
	inline void setWidth(int width) {
		this->width = width;
		for (std::shared_ptr<TilemapLayer> layer : mapLayers) {
			layer.get()->setWidth(this->width);
		}
	}

	inline int getHeight() const { return height; }
	inline int getHeightInPixels() const { return height * cellSize; }
	inline void setHeight(int height) {
		this->height = height;
		for (std::shared_ptr<TilemapLayer> layer : mapLayers) {
			layer.get()->setHeight(height);
		}
	}

	inline int getCellSize() const { return cellSize; }
	inline void setCellSize(int cellSize) {
		this->cellSize = cellSize;
		for (std::shared_ptr<TilemapLayer> layer : mapLayers) {
			layer.get()->setCellSize(this->cellSize);
		}
	}

	inline std::vector<std::shared_ptr<TilemapLayer>>& getMapLayers() { return mapLayers; }
	inline TilemapLayer* getMapLayer(int index) { return mapLayers[index].get(); }
	inline void addMapLayer(std::string name, Tileset* tileset) {
		std::shared_ptr<TilemapLayer> layer(new TilemapLayer(name, tileset, width, height, cellSize));
		mapLayers.push_back(layer);
	}
	inline void removeMapLayer(int index) { mapLayers.erase(mapLayers.begin() + index); }

	// inline std::vector<Collider>& getColliders() { return colliders; }
	// inline void updateQuadtree() {
	// 	quadtree = std::make_unique<Quadtree>(Rectangle{ 0, 0, float(width * cellSize), float(height * cellSize) }, 50);
	// 	for (const Collider& collider : colliders) quadtree.get()->insert(collider);
	// }
	// inline Quadtree* getQuadtree() { return quadtree.get(); }

	// inline void generateWalls() {
	// 	for (std::shared_ptr<TilemapLayer> layer : mapLayers) {
	// 		for (int i = 0; i < height; i++) {
	// 			for (int j = 0; j < width; j++) {
	// 				int id = layer->getTile(i, j);
	// 				if (id == 0) continue;

	// 				bool isSolid = layer->getTileset()->getTileById(id)->solid;
	// 				if (isSolid) {
	// 					Collider collider(cellSize * j, cellSize * i, cellSize, cellSize);
	// 					colliders.push_back(collider);
	// 				}
	// 			}
	// 		}
	// 	}
	// }

	inline std::vector<int>& getNavigationMap() { return navigationMap; }
	void generateNavigationMap() {
		navigationMap.clear();
		for (int i = 0; i < height * width; i++) {
			navigationMap.push_back(0);
		}

		for (std::shared_ptr<TilemapLayer> layer : mapLayers) {
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					if (navigationMap[i * width + j] == 1) continue;

					int id = layer.get()->getTile(i, j);
					if (id == 0) continue;

					Tile* tile = layer.get()->getTileset()->getTileById(id);
					if (tile->solid) navigationMap[i * width + j] = 1;
				}
			}
		}
	}

	void update();
	void draw(const Camera2D& camera, int viewportWidth, int viewportHeight);

private:
	std::string name;

	int width;
	int height;
	int cellSize;

	std::vector<std::shared_ptr<TilemapLayer>> mapLayers;
	std::vector<std::shared_ptr<Sprite>> sprites;

	// std::vector<Collider> colliders;
	// std::unique_ptr<Quadtree> quadtree;

	Tileset* currentTileset = nullptr;

	std::vector<int> navigationMap;

};
