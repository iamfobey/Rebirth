#include "Menu.h"

const char* vvvTextShaderCode = {
"#version 330 core\n"
"layout(location = 0) in vec4 vertex;\n"
"out vec2 TexCoords;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"	gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
"	TexCoords = vertex.zw;\n"
"}\n"
};

const char* fffTextShaderCode = { "#version 330 core\n"
"in vec2 TexCoords;\n"
"out vec4 color;\n"
"uniform sampler2D text;\n"
"uniform vec3 textColor;\n"
"void main()\n"
"{\n"
"	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
"	color = vec4(textColor, 1.0) * sampled;\n"
"}\n"
};

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace rb
{
	void Menu::Init(std::string fontPath, int width, int height)
	{
		glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));

		mTextShader.load(vvvTextShaderCode, fffTextShaderCode);

		mTextShader.use();
		glUniformMatrix4fv(glGetUniformLocation(mTextShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		
		this->fontPath = fontPath;
	}

	int Menu::CreateTextButton()
	{
		TextRenderer t;
		t.setShader(&mTextShader);
		t.Init(&VAO, &VBO, fontPath, 35);
		
		mTextButtonList.push_back(t);

		return mTextButtonList.size()-1;
	}

	bool Menu::RenderTextButton(int id, std::string text, double x, double y, GLFWwindow* window)
	{
		static int oldState = GLFW_RELEASE;
		int newState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		if (mPosMouseX >= x && mPosMouseX <= mTextButtonList[id].xpostemp && mPosMouseY >= mTextButtonList[id].ypostemp-8 && mPosMouseY <= mTextButtonList[id].ypostemp+20)
		{
			if (newState == GLFW_RELEASE && oldState == GLFW_PRESS) {
				oldState = newState;
				return true;
			}
			else
			{
				mTextButtonList[id].Render(text, &VAO, &VBO, -1, -1, x, y, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
				oldState = newState;
				return false;
			}
		}
		else
		{
			mTextButtonList[id].Render(text, &VAO, &VBO, -1, -1, x, y, 1.0f, glm::vec3(0.8f, 0.2f, 0.4f));
			return false;
		}
	}

	void Menu::SetMousePos(double x, double y)
	{
		mPosMouseX = x;
		mPosMouseY = 900-y;
	}
}