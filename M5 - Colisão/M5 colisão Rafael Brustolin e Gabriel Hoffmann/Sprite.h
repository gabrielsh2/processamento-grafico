#pragma once

#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Sprite
{
public:
	Sprite(float vertices[]);
	~Sprite() {}
	void setTexture(int texID);
	void setShader(GLuint shader) { this->shader = shader; glUseProgram(shader); }
	
	void setPosition(glm::vec3 pos) { this->pos = pos; } 
	void setDimension(glm::vec3 scale) { this->scale = scale; }

	void setTranslation(glm::vec3 displacements, bool reset = true);
	void setScale(glm::vec3 scaleFactors, bool reset = true);

	float getPosZInicial();
	
	float getPosX();
	void setPosX(float x);

	float getPosY();
	void setPosY(float y);

	void setPosXInicial(float x);
	void setPosYInicial(float y);
	void setPosZInicial(float z);

	void setOffsetX(float x);
	void setOffsetY(float y);
	void setRateX(float x);
	void setRateY(float y);
	float getOffsetX();
	float getRateX();

	void draw();
	void update();

protected:
	GLuint VAO;
	glm::mat4 transform;
	unsigned int texID;
	GLuint shader;
	glm::vec3 pos, scale;
	float offsetx, offsety, ratex, ratey;
	float posXInicial = 0, posYInicial = 0, posZInicial = 0;
};

