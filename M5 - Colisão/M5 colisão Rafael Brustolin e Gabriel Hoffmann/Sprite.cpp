#include "Sprite.h"

Sprite::Sprite(float vertices[])
{
	float vertices1[32];
	for (int i = 0; i < 32; i++) {
		vertices1[i] = vertices[i];
	}

	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	unsigned int VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	transform = glm::mat4(1); //matriz identidade
	texID = -1; 
	shader = NULL; 
	pos = glm::vec3(0.0f, 0.0f, 0.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
}

void Sprite::setTexture(int texID)
{
	this->texID = texID;
}

void Sprite::setTranslation(glm::vec3 displacements, bool reset)
{
	if (reset) transform = glm::mat4(1);
	transform = glm::translate(transform, displacements);
}

void Sprite::setScale(glm::vec3 scaleFactors, bool reset)
{
	if (reset) transform = glm::mat4(1);
	transform = glm::scale(transform, scaleFactors);
	scale = scaleFactors;
}

float Sprite::getPosZInicial()
{
	return posZInicial;
}

float Sprite::getPosX()
{
	return pos[0];
}

void Sprite::setPosX(float x)
{
	pos[0] = x;
}

float Sprite::getPosY()
{
	return pos[1];
}

void Sprite::setPosY(float y)
{
	pos[1] = y;
}

void Sprite::setPosXInicial(float x)
{
	posXInicial = x;
}

void Sprite::setPosYInicial(float y)
{
	posYInicial = y;
}

void Sprite::setPosZInicial(float z)
{
	posZInicial = z;
}

void Sprite::setOffsetX(float x)
{	
	offsetx = x;
}
void Sprite::setOffsetY(float y)
{
	offsety = y;
}
void Sprite::setRateX(float x)
{
	ratex = x;
}
void Sprite::setRateY(float y)
{
	ratey = y;
}

float Sprite::getOffsetX()
{
	return offsetx;
}

float Sprite::getRateX()
{
	return ratex;
}

void Sprite::draw()
{
	glBindTexture(GL_TEXTURE_2D, texID);
	glUniform1i(glGetUniformLocation(shader, "texture1"), 0);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Sprite::update()
{
	setTranslation(pos);
	setScale(scale, false);

	GLint transformLoc = glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
}
