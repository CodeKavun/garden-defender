#pragma once
#include <string>
#include <vector>
#include <raylib.h>
#include <nlohmann/json.hpp>

enum AnimationType
{
	ORDINAR,
	ASEPRITE
};

struct AnimationFrame
{
	int regionIndex;
	float delay;
	Rectangle source;
};

class Animation
{
public:
	Animation() {}
	Animation(std::string name, std::vector<AnimationFrame> frames)
        : name(name), frames(frames) {}

	inline std::string getName() const { return name; }

	inline std::vector<AnimationFrame>& getFrames() { return frames; }
	inline AnimationFrame& getFrame(int index) { return frames[index]; }
	inline int getFrameCount() const { return frames.size(); }

private:
	std::string name;
	std::vector<AnimationFrame> frames;
	int* reapeat;
};


class AnimationPlayer
{
public:
	AnimationPlayer() {}
	AnimationPlayer(std::string animFile);

	inline std::string getAnimationPath() const { return animFile; }
	inline std::string getTexturePath() const { return texturePath; }

	inline std::map<std::string, Animation*>& getAnimations() { return animations; }

	inline Texture2D getTexture() const { return texture; }

	inline AnimationType getType() const { return type; }

	inline bool isPlaying() const { return playing; }

	inline Animation* getCurrentAnimation() { return animations[currentAnimationName]; }
	inline AnimationFrame& getCurrentFrame() { return getCurrentAnimation()->getFrame(currentFrameIndex); }

	inline Rectangle getSource() {
		Rectangle defaultRect = { 0, 0, (float)texture.width, (float)texture.height };
		return getCurrentAnimation() != nullptr ? getCurrentFrame().source : defaultRect;
	}

	void play(std::string name, bool repeat);
	void stop();
	void update();
	void draw(Vector2 position, Vector2 scale, Vector2 origin, float rotation = 0, bool flipX = false, bool flipY = false);

private:
	std::string animFile;
	std::string texturePath;
	std::map<std::string, Animation*> animations;
	AnimationType type;

	Texture2D texture;
	bool playing = false;
	float timer = 0;

	std::string currentAnimationName;
	int currentFrameIndex = 0;
};
