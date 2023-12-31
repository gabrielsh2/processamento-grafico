#include "Sprite.h"

Sprite::Sprite(float vertices[])
{
	float vertices1[16];
	for (int i = 0; i < 16; i++) {
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
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	r = -1;
	c = -1;
	transform = glm::mat4(1); //matriz identidade
	texID = -1;
	shader = NULL;
	pos = glm::vec3(0.0f, 0.0f, 0.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
}
void Sprite::setRC(int r, int c)
{
	this->r = r;
	this->c = c;
}

int Sprite::getR()
{
	return r;
}
int Sprite::getC()
{
	return c;
}

void Sprite::setTexture(int texID)
{
	this->texID = texID;
}

void Sprite::setTranslation(glm::vec3 displacements)
{
	transform = glm::mat4(1);
	transform = glm::translate(transform, displacements);
}
void Sprite::setRotation(float angle)
{
	transform = glm::rotate(transform, angle, glm::vec3(0.0f, 0.0f, 1.0f));
}

void Sprite::setScale(glm::vec3 scaleFactors)
{
	transform = glm::scale(transform, scaleFactors);
	scale = scaleFactors;
}
void Sprite::setAngle(float angle)
{
	this->angle = angle;
}

float Sprite::getAngle()
{
	return this->angle;
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
float Sprite::getOffsetY()
{
	return offsety;
}
float Sprite::getRateX()
{
	return ratex;
}

void Sprite::draw()
{
	glBindTexture(GL_TEXTURE_2D, texID);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
}


void Sprite::update()
{
	if (r >= 0 && c > 0) {
		pos[1] = (-0.066666 / 2) * (c - r);
		pos[0] = 0.033333 * (r - (pos[1] / 0.066666));
		
		pos[0] *= 800;
		pos[1] *= 200;
		pos[1] += 200;
		//std::cout << pos[0] << " " << pos[1];
	}

	setTranslation(pos);
	setRotation(angle);
	setScale(scale);

	glUniform1f(glGetUniformLocation(shader, "offsetx"), this->offsetx);
	glUniform1f(glGetUniformLocation(shader, "offsety"), this->offsety);
	glUniform1f(glGetUniformLocation(shader, "tx"), 0);
	glUniform1f(glGetUniformLocation(shader, "ty"), 0);

	GLint transformLoc = glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
}
