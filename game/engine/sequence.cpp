#include "sequence.hpp"
#include <fstream>
#include <iostream>
#include "utils.hpp"
#include "gfx.hpp"

AnimationPlayer::AnimationPlayer(std::string animFile)
{
	this->animFile = animFile;

	// Get relative path to a texture
	std::istringstream pathStream(this->animFile);
	std::string subPath;
	std::vector<std::string> subPaths;

	while (std::getline(pathStream, subPath, '/')) {
		subPaths.push_back(subPath);
	}

	subPaths.erase(subPaths.begin());
	subPaths.pop_back();

	std::string textureDir = "";
	for (std::string forlder : subPaths) {
		textureDir += forlder + "/";
	}
	// ------------

	// Read json
	std::ifstream animReader(this->animFile);

	std::string jsonLine;
	std::string jsonString;
	while (animReader >> jsonLine) {
		jsonString += jsonLine;
	}

	nlohmann::ordered_json jsonData = nlohmann::ordered_json::parse(jsonString);

	// Check if it's Aseprite's or my own format
	this->type = jsonData.contains("meta") ? ASEPRITE : ORDINAR;

	switch (this->type)
	{
	case ORDINAR:
	{
		this->texturePath = textureDir + (std::string)jsonData["texture"];
		this->texture = AssetManager::loadTexture(this->texturePath);

		int regionWidth = jsonData["width"];
		int regionHeight = jsonData["height"];

		TextureAtlas atlas(this->texture.width, this->texture.height, regionWidth, regionHeight);
		atlas.createGrid();

		for (auto& anim : jsonData["animations"].items()) {
			std::vector<AnimationFrame> frames;

			for (auto& frame : anim.value()) {
				int regionIndex = frame["index"];
				float delay = frame["delay"];

				AnimationFrame frameToAdd;
				frameToAdd.regionIndex = regionIndex;
				frameToAdd.delay = delay;
				frameToAdd.source = atlas.getRegion(regionIndex);

				frames.push_back(frameToAdd);
			}

			Animation* animationToAdd = new Animation(anim.key(), frames);
			this->animations.insert({ anim.key(), animationToAdd });
			anim.key();
		}

		break;
	}
	case ASEPRITE:
	{
		// Load a texture
		this->texturePath = textureDir + (std::string)jsonData["meta"]["image"];
		this->texture = AssetManager::loadTexture(this->texturePath);

		// Adding aseprite animations
		nlohmann::ordered_json tags = jsonData["meta"]["frameTags"];

		for (auto& tag : tags) {
			// Getting animation datas from Ase
			std::string tagName = (std::string)tag["name"];

			int fromFrame = (int)tag["from"];
			int toFrame = (int)tag["to"];

			//int repeat = (int)tags[i]["repeat"];

			TraceLog(LOG_INFO, ("ANIMATION: " + tagName + " -- " + std::to_string(fromFrame) + " -> " + std::to_string(toFrame)).c_str());

			// adding frames
			nlohmann::ordered_json frameArrayData = jsonData["frames"];
			std::vector<AnimationFrame> frames;

			int frameDataIndex = 0;
			for (auto it = frameArrayData.begin(); it != frameArrayData.end(); it++, frameDataIndex++) {
				if (frameDataIndex < fromFrame) continue;
				if (frameDataIndex > toFrame) break;

				std::string key = it.key();

				float delay = frameArrayData[key]["duration"].get<float>();

				nlohmann::json sourceData = frameArrayData[key]["frame"];
				Rectangle source = { 0 };
				source.x = sourceData["x"].get<float>();
				source.y = sourceData["y"].get<float>();
				source.width = sourceData["w"].get<float>();
				source.height = sourceData["h"].get<float>();

				AnimationFrame frame;
				frame.regionIndex = frameDataIndex;
				frame.delay = delay * 0.001f;
				frame.source = source;

				frames.push_back(frame);

				TraceLog(LOG_INFO, ("\t\t Added frame " + std::to_string(frameDataIndex)
					+ " with delay of " + std::to_string(delay * 0.001f) + "s").c_str());
			}

			Animation* anim = new Animation(tagName, frames);
			this->animations[tagName] = anim;
		}

		break;
	}
	}
}

void AnimationPlayer::play(std::string name, bool repeat)
{
	if (getCurrentAnimation() != nullptr && getCurrentAnimation()->getName() == name) return;

	if (animations.contains(name)) {
		currentAnimationName = name;
		currentFrameIndex = 0;
		timer = 0;
		playing = true;
	}
}

void AnimationPlayer::stop()
{
	playing = false;
	timer = 0;
}

void AnimationPlayer::update()
{
	if (getCurrentAnimation() == nullptr || !playing) {
		return;
	}

	timer += GetFrameTime();
	if (timer >= getCurrentFrame().delay) {
		timer = 0;

		currentFrameIndex++;
		if (currentFrameIndex >= getCurrentAnimation()->getFrames().size()) currentFrameIndex = 0;
	}
}

void AnimationPlayer::draw(Vector2 position, Vector2 scale, Vector2 origin, float rotation, bool flipX, bool flipY)
{
	if (getCurrentAnimation() == nullptr) {
		DrawTexture(texture, position.x, position.x, WHITE);
		return;
	}

	Rectangle source = getCurrentFrame().source;

	if (flipX) source.width = -source.width;
	if (flipY) source.height = -source.height;

	DrawTexturePro(
		texture,
		source,
		{
			position.x,
			position.y,
			getCurrentFrame().source.width * scale.x,
			getCurrentFrame().source.height * scale.y
		},
		origin,
		rotation,
		WHITE
	);
}
