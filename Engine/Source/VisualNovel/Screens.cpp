#include "Screens.h"

namespace rb
{
	void Screen::Init()
	{
		const char* vertexShaderSource = "#version 330 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
			"}\0";

		const char* fragmentShaderSource = "#version 330 core\n"
			"out vec4 FragColor;\n"
			"void main()\n"
			"{\n"
			"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
			"}\n\0";

		sh.load(vertexShaderSource, fragmentShaderSource);

        float vertices[] = {
             0.1f,  0.05f, 0.0f,  // верхняя правая
             0.1f, -0.05f, 0.0f,  // нижняя правая
            -0.1f, -0.05f, 0.0f,  // нижняя левая
            -0.1f,  0.05f, 0.0f   // верхняя левая
            };
        unsigned int indices[] = {  // помните, что мы начинаем с 0!
            0, 1, 3,  // первый треугольник
            1, 2, 3   // второй треугольник
        };

        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);
        glGenBuffers(1, &mEBO);

        // Сначала связываем объект вершинного массива, затем связываем и устанавливаем вершинный буфер(ы), и затем конфигурируем вершинный атрибут(ы)
        glBindVertexArray(mVAO);

        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Обратите внимание, что данное действие разрешено, вызов glVertexAttribPointer() зарегистрировал VBO как привязанный вершинный буферный объект для вершинного атрибута, так что после этого мы можем спокойно выполнить отвязку
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // помните: не отвязывайте EBO, пока VАО активен, поскольку связанного объект буфера элемента хранится в VАО; сохраняйте привязку EBO.
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // Вы можете отменить привязку VАО после этого, чтобы другие вызовы VАО случайно не изменили этот VAO (но подобное довольно редко случается).
        // Модификация других VAO требует вызов glBindVertexArray(), поэтому мы обычно не снимаем привязку VAO (или VBO), когда это не требуется напрямую
        glBindVertexArray(0);
	}
	void Screen::Render()
	{
		sh.use();
		glBindVertexArray(mVAO); // поскольку у нас есть только один VАО, то нет необходимости связывать его каждый раз, но мы сделаем это, чтобы всё было немного более организованно
		// glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
}