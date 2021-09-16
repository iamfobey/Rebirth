/*
*  Thank you for staying with us.
*  (c) Oneiro Games 2019 - 2021
*/

#include "Text.h"

extern int WNDwidth, WNDheight;

namespace rb
{
	void Text::Init(std::string fontPath, int width, int height, int size)
	{
		const char* TextVertexShader = R"(
			#version 330 core
			layout(location = 0) in vec4 vertex;
			out vec2 TexCoords;
			uniform mat4 projection;
			void main()
			{
				gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
				TexCoords = vertex.zw;
			}
		)";

		const char* TextFragmentShader = R"(
			#version 330 core
			in vec2 TexCoords;
			out vec4 color;
			uniform sampler2D text;
			uniform vec3 textColor;
			void main()
			{
				vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
				color = vec4(textColor, 1.0) * sampled;
			}
		)";

		glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));

		mTextShader.load(TextVertexShader, TextFragmentShader);

		mTextShader.use();
		glUniformMatrix4fv(glGetUniformLocation(mTextShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		mTextRenderer.setShader(&mTextShader);
		mTextRenderer.Init(&VAO, &VBO, fontPath, size);
	}
	void Text::Render(std::string text, int width, int height, float x, float y, float scale, glm::vec3 color)
	{
		glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(1600), 0.0f, static_cast<float>(900));

		mTextShader.use();
		glUniformMatrix4fv(glGetUniformLocation(mTextShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		mTextRenderer.Render(text, &VAO, &VBO, width, height, x, y, scale, color);
	}
}