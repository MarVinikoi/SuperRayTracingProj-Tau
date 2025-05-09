#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>


void g_RenderLine(float startx, float starty, float endx, float endy, std::vector<float> color) {

	float vertices[] = {
		startx, starty, 0.0f, color.at(0), color.at(1), color.at(2),
		endx, endy, 0.0f, color.at(0), color.at(1), color.at(2)
	};

    // std::cout << "X: " <<vertices[0] << "|  Y: " << vertices[1] << std::endl;
	// std::cout << "R: " <<vertices[3] << "|  G: " << vertices[4] << "|  B: " << vertices[5] << std::endl;

	GLuint VAOline, VBOline;

	glGenVertexArrays(1, &VAOline);
	glGenBuffers(1, &VBOline);

	glBindVertexArray(VAOline);
	glBindBuffer(GL_ARRAY_BUFFER, VBOline);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_LINES, 0, 2);

	glDeleteVertexArrays(1, &VAOline);
	glDeleteBuffers(1, &VBOline);
}