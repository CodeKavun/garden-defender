#include "world.hpp"

Sprite::Sprite(AnimationPlayer* animPlayer, Vector2 position)
	: Sprite(animPlayer, position, { 1, 1 })
{
}

Sprite::Sprite(AnimationPlayer* animPlayer, Vector2 position, Vector2 scale)
	: Sprite(animPlayer, position, scale, { 0, 0 }, true)
{
}

Sprite::Sprite(AnimationPlayer* animPlayer, Vector2 position, Vector2 scale, Vector2 origin, bool centered)
{
	this->animPlayer = animPlayer;
	this->position = position;
	this->scale = scale;
	this->origin = origin;
	this->centered = centered;
}

void Sprite::update()
{
	animPlayer->update();
}

void Sprite::draw()
{
	if (shader) BeginShaderMode(*shader.get());

	animPlayer->draw(
		getRoundedPosition(),
		scale,
		centered ? Vector2{ animPlayer->getSource().width / 2 + origin.x,
			animPlayer->getSource().height / 2 + origin.y } : origin,
		0,
		flipX, flipY);

	if (shader) EndShaderMode();
}
