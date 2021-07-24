#include "Sprite.h"

#include <OpenGL/GLUtils.h>

const char* vSpriteShaderCode = { "#version 330 core\n"
		"layout(location = 0) in vec3 aPos;\n"
		"layout(location = 2) in vec3 aColor;\n"
		"layout(location = 2) in vec2 aTexCoord;\n"
		"\n"
		"out vec2 TexCoord;\n"
		"out vec3 ourColor;\n"
		"uniform mat4 view;\n"
		"uniform mat4 proj;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position = proj * view * vec4(aPos, 1.0);\n"
		"	TexCoord = aTexCoord;\n"
		"	ourColor = aColor;\n"
		"}\n"
};

const char* fSpriteShaderCode = { "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec2 TexCoord;\n"
		"uniform sampler2D ourTexture;\n"
		"void main()\n"
		"{\n"
		"	vec4 texColor = texture(ourTexture, TexCoord);\n"
		"	if(texColor.a < 0.1)\n"
		"		discard;\n"
		"	FragColor = texColor;\n"
		"}\n"
};

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

extern unsigned int windowWidth, windowHeight;

namespace rb
{
	void Sprite::Init()
	{
		mSpriteShader.load(vSpriteShaderCode, fSpriteShaderCode);

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
	}

	void Sprite::Render()
	{
		mSpriteShader.use();
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)1600 / (float)900, 0.1f, 100.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

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
		double oz;
		glu::ClientToGL(x, y, &mPosX, &mPosY, &oz);

		float vertices[] = {
			   -0.85f + mPosX, -1.3f + mPosY, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
				0.85f + mPosX, -1.3f + mPosY, 0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
				0.85f + mPosX,  0.85f + mPosY, 0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
			   -0.85f + mPosX,  0.85f + mPosY, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f
		};

		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertices), spriteindices, GL_STATIC_DRAW);
	}
}