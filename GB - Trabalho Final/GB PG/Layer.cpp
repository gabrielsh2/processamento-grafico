#include "Layer.h"

void Layer::updateCamada()
{
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->update();
		objects[i]->draw();
	}
}
void Layer::updateCamadaSemDraw()
{
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->update();
	}
}

void Layer::adicionarObjeto(float posX, float posY, float posZ, float dimX, float dimY, float offsetx, float offsety, float ratex, float ratey, GLuint shader, float vertices[], float angle)
{
	obj = new Sprite(vertices);
	obj->setPosition(glm::vec3(posX, posY, posZ));
	obj->setDimension(glm::vec3(dimX, dimY, 0.0));
	obj->setShader(shader);
	objects.push_back(obj);
	obj->setPosXInicial(posX);
	obj->setPosYInicial(posY);
	obj->setPosY(posY);
	obj->setPosX(posX);
	//obj->setPosZ(posZ);
	obj->setOffsetX(offsetx);
	obj->setOffsetY(offsety);
	obj->setRateX(ratex);
	obj->setRateY(ratey);
	obj->setAngle(angle);
}

void Layer::setDeslocamento(float deslocamento)
{
	this->deslocamento = deslocamento;
}

float Layer::getDeslocamento()
{
	return deslocamento;
}
