#pragma once

#ifndef MENU_H
#define MENU_H

#include <OpenGL/GLTextRenderer.h>
#include <OpenGL/GLShader.h>

#include <GLFW/glfw3.h>

namespace rb
{
	class Menu
	{
	public:
		Menu() {}
		~Menu() {}

		void Init(std::string fontPath, int width, int height);

		int CreateTextButton();
		bool RenderTextButton(int id, std::string text, double x, double y, GLFWwindow* window);
		void SetMousePos(double x, double y);
	private:
		std::vector<TextRenderer> mTextButtonList;
		std::string mText;
		double mPosX, mPosY;

		double mPosMouseX, mPosMouseY;

		Shader mTextShader;

		unsigned int VAO, VBO;

		std::string fontPath;
		int width, height;
	};
}

#endif