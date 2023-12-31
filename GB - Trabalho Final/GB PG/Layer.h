#include "Sprite.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#pragma once


using namespace std;

class Layer
{
public:

	void updateCamada();
	void updateCamadaTileMap();
	void updateCamadaSemDraw();
	void adicionarObjeto(float posX, float posY, float posZ, float dimX, float dimY, float offsetx, float offsety, float ratex, float ratey, GLuint shader, float vertice[], float angle);
	void setDeslocamento(float deslocamento);
	float getDeslocamento();

	float deslocamento = 0;

	Sprite* obj;
	vector <Sprite*> objects;

};