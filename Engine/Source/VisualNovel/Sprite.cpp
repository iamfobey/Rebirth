#include "Sprite.h"

const char* vSpriteShaderCode = { "#version 330 core\n"
		"layout(location = 0) in vec3 aPos;\n"
		"layout(location = 2) in vec3 aColor;\n"
		"layout(location = 2) in vec2 aTexCoord;\n"
		"\n"
		"out vec2 TexCoord;\n"
		"out vec3 ourColor;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position = vec4(aPos, 1.0);\n"
		"	TexCoord = aTexCoord;\n"
		"	ourColor = aColor;\n"
		"}\n"
};

const char* fSpiteShaderCode = { "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec2 TexCoord;\n"
		"\n"
		"uniform sampler2D ourTexture;\n"
		"uniform float alpha = 0.0;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	vec4 texColor = texture(ourTexture, TexCoord);\n"
		"	if(texColor.a < 0.1)\n"
		"		discard;\n"
		"	FragColor = texColor;\n"
		"}\n"
};

namespace rb
{
	void Sprite::Init()
	{
		float spritevertices[] = {
			   -0.45f, -1.0f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
				0.45f, -1.0f, 0.0f,  1.0f, 1.0f, 1.0f,  1.2f, 0.0f,
				0.45f,  1.0f, 0.0f,  1.0f, 1.0f, 1.0f,  1.2f, 1.2f,
			   -0.45f,  1.0f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.2f
		};

		unsigned int spriteindices[] = {
					0, 1, 2,
					2, 3, 0
		};

		mSpriteShader.load(vSpriteShaderCode, fSpiteShaderCode);

		glGenVertexArrays(1, &mVAO);
		glGenBuffers(1, &mVBO);
		glGenBuffers(1, &mEBO);

		glBindVertexArray(mVAO);

		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(spritevertices), spritevertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(spritevertices), spriteindices, GL_STATIC_DRAW);

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

		glBindVertexArray(mVAO);

		glBindTexture(GL_TEXTURE_2D, mSpriteTexture.GetID());
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}

	void Sprite::Load(std::string imagePath)
	{
		mSpriteTexture.Load(imagePath);
	}
}