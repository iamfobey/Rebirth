/*
*  Thank you for staying with us.
*  (c) Oneiro Games 2019 - 2021
*/

#include "Sprite.h"

extern int WNDwidth, WNDheight;

namespace rb
{
	void Sprite::Init()
	{
		const char* SpriteVertexShader = R"(
			#version 330 core
			layout(location = 0) in vec3 aPos;
			layout(location = 2) in vec3 aColor;
			layout(location = 2) in vec2 aTexCoord;
			out vec2 TexCoord;
			out vec3 ourColor;
			uniform mat4 view;
			uniform mat4 proj;
			void main()
			{
				gl_Position = proj * view * vec4(aPos, 1.0);
				TexCoord = aTexCoord;
				ourColor = aColor;
			}
		)";

		const char* SpriteFragmentShader = R"(
			#version 330 core
			out vec4 FragColor;
			in vec2 TexCoord;
			uniform sampler2D ourTexture;
			void main()
			{
				vec4 texColor = texture(ourTexture, TexCoord);
				if(texColor.a < 0.1)
					discard;\
				FragColor = texColor;
			}
		)";

		float spriteVertices[] = {
		   -0.85f, -1.15f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
			0.85f, -1.15f, 0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
			0.85f,  0.85f, 0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
		   -0.85f,  0.85f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f
		};

		unsigned int spriteindices[] = {
			0, 1, 2,
			2, 3, 0
		};

		mSpriteShader.load(SpriteVertexShader, SpriteFragmentShader);

		glGenVertexArrays(1, &mVAO);
		glGenBuffers(1, &mVBO);
		glGenBuffers(1, &mEBO);

		glBindVertexArray(mVAO);

		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(spriteVertices), spriteVertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(spriteVertices), spriteindices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -2.75f));
	}

	void Sprite::Render()
	{
		mSpriteShader.use();
		projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)WNDwidth / (float)WNDheight, 0.1f, 100.0f);		

		glUniformMatrix4fv(glGetUniformLocation(mSpriteShader.ID, "proj"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(mSpriteShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glBindVertexArray(mVAO);

		glBindTexture(GL_TEXTURE_2D, mSpriteTexture.GetID());
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}

	void Sprite::Load(std::string imagePath)
	{
		mSpriteTexture.Load(imagePath);
	}

	void Sprite::SetPosition(double x, double y)
	{
		view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(x, y, -2.75f));
	}
}