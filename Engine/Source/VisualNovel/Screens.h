#pragma once

#include <OpenGL/GLCore.h>

#include <OpenGL/GLShader.h>

namespace rb
{
	class Screen
	{
	public:
		void Init();
		void Render();
	private:
		unsigned int mVAO, mVBO, mEBO;
		Shader sh;
	};
}