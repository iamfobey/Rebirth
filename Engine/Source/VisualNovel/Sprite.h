#pragma once

#ifndef SPRITE_H
#define SPRITE_H

#include <OpenGL/GLCore.h>

#include <OpenGL/GLTexture.h>
#include <OpenGL/GLShader.h>

namespace rb
{
	class Sprite
	{
	public:
		Sprite() {}
		~Sprite() {}

		void Init();

		void Render();
		void Load(std::string imagePath);
		void SetPosition(double x, double y);
	private:
		Texture mSpriteTexture;
		Shader mSpriteShader;

		double mPosX, mPosY;
		double mVX, mVY;

		unsigned int mVAO, mVBO, mEBO;
	};
}

#endif