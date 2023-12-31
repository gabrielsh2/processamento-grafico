//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <string.h>
#include <time.h>
#define GL_LOG_FILE "gl.log"
#include <iostream>
#include <vector>
#include "TileMap.h"
#include "DiamondView.h"
#include "ltMath.h"
#include <fstream>

#include <thread>

#include "Layer.h"
#include "Shader.h"
#include "Sprite.h"
#include "maths_funcs.h"
/* Command line build:
  g++ -framework Cocoa -framework OpenGL -framework IOKit -o demoIsom gl_utils.cpp maths_funcs.cpp stb_image.cpp _isometrico.cpp  -I include -I/sw/include -I/usr/local/include -I ../common/include ../common/osx_64/libGLEW.a ../common/osx_64/libglfw3.a
 */

using namespace std;

float xi = -1.0f;
float xf = 1.0f;
float yi = -1.0f;
float yf = 1.0f;
float w = xf - xi;
float h = yf - yi;
float tw, th, tw2, th2;
int tileSetCols = 8, tileSetRows = 21;
float tileW, tileW2;
float tileH, tileH2;
int cx = -1, cy = -1;

const int chao = 0;
const int carro = 1;
const int alvo = 2;
const int pneu = 3;
const int arrow = 4;
const int espinho = 5;

int SCREEN_WIDTH = 1600, SCREEN_HEIGHT = 800, INTEGRIDADE_INICIAL = 100, frameAtual = 0;
float fh = 0.005f, offsety = 0.5, previous, sprite_speed = 0.05;
bool colidindo = false;
bool cima = true;
double current_seconds;

float orthoX = 0;
float orthoY = 0;

unsigned int VAO_CHAO;


TilemapView* tview = new DiamondView();
TileMap* tmap = NULL;

enum Movimento { esquerda, parado, direita, frente, tras };
static Movimento mov = parado;

Shader* shader;

Layer* layers = new Layer[6];
vector <Sprite*> objects;

int CARRO_C_INICIAL = 30, CARRO_R_INICIAL = 0;
int ALVO_C_INICIAL = 1, ALVO_R_INICIAL = 29;

int carrinhoC = CARRO_C_INICIAL;
int carrinhoR = CARRO_R_INICIAL;

bool aux_pressionado = false;
bool aux_pressionado_anterior = false;

int integridade = INTEGRIDADE_INICIAL;


//definir funções
void redefinirParametros();
void updateArrowAngle();
void updateOrtho();

vector<int> lamaList = { 0, 1, 2, 3, 8, 9 ,10, 11, 16, 17, 18, 19, 24, 25, 26, 27,
				  32, 33, 34, 35, 40, 41, 42, 43, 48, 49, 50, 51, 56, 57, 58, 59,
				  64, 65, 66, 67, 72, 73, 74, 75, 80, 81, 82, 83, 88, 89, 90, 91,
				  96, 97, 98, 99, 104, 105, 106, 107, 116, 117, 118, 119, 124,
				  125, 126, 127, 132, 133, 134, 135, 140, 141, 142, 143, 148,
				  149, 150, 151, 156, 157, 158, 159, 164, 165, 166, 167, 79, 92, 93, 94, 95
};

string nivel[4][2] = { {"old orange truck.png", "tilemap1.txt"}, { "new red truck.png", "tilemap2.txt" }, { "black SUV.png", "tilemap3.txt" }, { "medalha.png", "tilemap3.txt" } };

int nivelAtual = 0;



TileMap* readMap(string filename) {
	ifstream arq(filename);
	int w, h;
	arq >> w >> h;
	TileMap* tmap = new TileMap(w, h, 0);
	for (int r = tmap->getWidth() - 1; r >= 0; r--) {
		for (int c = 0; c < w; c++) {
			int tid;
			arq >> tid;
			//cout << tid << " ";
			tmap->setTile(c, r, tid - 1);
		}
		//cout << endl;
	}
	arq.close();
	return tmap;
}

int loadTexture(string fpath)
{
	unsigned int texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	int width, height, nrChannels;

	unsigned char* data = stbi_load(fpath.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);

	return texture;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		mov = direita;
		aux_pressionado = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		mov = esquerda;
		aux_pressionado = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		mov = frente;
		aux_pressionado = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		mov = tras;
		aux_pressionado = true;
	}
	else {
		mov = parado;
		aux_pressionado_anterior = false;
	}
}

void setTexturas() {
	// Adicionando Textura para cada Sprite
		//unsigned int texID = loadTexture(nivel[nivelAtual][0]);
	unsigned int texID = loadTexture("./textures/" + nivel[nivelAtual][0]);
	layers[carro].objects[0]->setTexture(texID);
	texID = loadTexture("./textures/" + nivel[nivelAtual + 1][0]);
	layers[alvo].objects[0]->setTexture(texID);
	texID = loadTexture("./textures/" + nivel[3][0]);
	layers[alvo].objects[1]->setTexture(texID);
	texID = loadTexture("./textures/wheel.png");
	for (int i = 0; i < layers[pneu].objects.size(); i++)
	{
		layers[pneu].objects[i]->setTexture(texID);
	}
	texID = loadTexture("./textures/espinho.png");
	for (int i = 0; i < layers[espinho].objects.size(); i++)
	{
		layers[espinho].objects[i]->setTexture(texID);
	}

	texID = loadTexture("./textures/arrow.png");
	layers[arrow].objects[0]->setTexture(texID);
}

void criaTmap() {
	//cout << "Tentando criar tmap" << endl;
	tmap = readMap("./tilemap/" + nivel[nivelAtual][1]);
	tw = w / (float)tmap->getWidth();
	th = tw / 2.0f;
	tw2 = th;
	th2 = th / 2.0f;
	tileW = 1.0f / (float)tileSetCols;
	tileW2 = tileW / 2.0f;
	tileH = 1.0f / (float)tileSetRows;
	tileH2 = tileH / 2.0f;

	/*cout << "tw=" << tw << " th=" << th << " tw2=" << tw2 << " th2=" << th2
		<< " tileW=" << tileW << " tileH=" << tileH
		<< " tileW2=" << tileW2 << " tileH2=" << tileH2
		<< endl;*/
	GLuint tid;
	tid = loadTexture("./textures/tiles/all terrain.png");

	tmap->setTid(tid);
	//cout << "Tmap inicializado" << endl;
	for (int c = 0; c < tmap->getWidth(); c++) {
		for (int r = 0; r < tmap->getHeight(); r++) {
			unsigned char t_id = tmap->getTile(c, r);
			//cout << ((int)t_id) << " ";
		}
		//cout << endl;
	}
	//cout << "\n\n";
}
void gerarItens() {
	float vertices[] = {
		// positions  // texture coords
		0.5f,  0.5f, 1.0f, 1.0f, // top right
		0.5f, -0.5f,  1.0f, 0, // bottom right
		-0.5f, -0.5f, 0, 0, // bottom left
		-0.5f,  0.5f,  0, 1.0f  // top left 
	};

	cout << "gerar aleatorio" << endl;
	//pneus
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	vector<int> colItens, rowItens;
	cout << "gerar posicoes itens" << endl;

	for (int i = 0; i < 20; ++i) {
		colItens.push_back(1 + (std::rand() % 30));
		cout << "numeros rand col" << colItens[i] << endl;

		rowItens.push_back(std::rand() % 30);
		cout << "numeros rand row" << rowItens[i] << endl;

		for (int j = 0; j < i; ++j) {
			cout << "pneu " << j << endl;

			bool mesmaPosicaoCarro = colItens[j] == CARRO_C_INICIAL && rowItens[j] == CARRO_R_INICIAL;
			bool mesmaPosicaoAlvo = colItens[j] == ALVO_C_INICIAL && rowItens[j] == ALVO_R_INICIAL;
			bool posicaoRepetida = colItens[j] == colItens[i] && rowItens[j] == rowItens[i];

			cout << "pneu posicao col " << colItens[j] << " " << colItens[i] << endl;
			cout << "pneu posicao row " << rowItens[j] << " " << rowItens[i] << endl;
			cout << "1 " << mesmaPosicaoCarro << "2 " << mesmaPosicaoAlvo << "3 " << posicaoRepetida;


			if (posicaoRepetida ||
				mesmaPosicaoCarro ||
				mesmaPosicaoAlvo
				) {
				colItens.erase(colItens.begin() + j);
				rowItens.erase(rowItens.begin() + j);
				--i;
				break;
			}
		}
	}

	cout << "adicionar objeto" << endl;

	for (int i = 0; i < rowItens.size(); i++)
	{
		layers[pneu].adicionarObjeto(400.0f, 200.0f, 2.0f, 22.0f, 22.0f, 0.0f, 0.0f, 0.0f, 0.0f, shader->ID, vertices, 0);
		layers[pneu].objects[i]->setRC(rowItens[i], colItens[i]);
	}

	cout << "clear para o espinho" << endl;

	//espinho
	colItens.clear();
	rowItens.clear();
	cout << "loop para o espinho" << endl;

	for (int i = 0; i < 50; ++i) {

		colItens.push_back(1 + (std::rand() % 30));
		cout << "numeros rand col" << colItens[i] <<  endl;

		rowItens.push_back(std::rand() % 30);
		cout << "numeros rand row" << rowItens[i] << endl;

		cout << "loop no array" << endl;

		for (int j = 0; j < i; ++j) {
			cout << "espinho " << j << endl;
			bool mesmaPosicaoCarro = colItens[j] == CARRO_C_INICIAL && rowItens[j] == CARRO_R_INICIAL;
			bool mesmaPosicaoAlvo = colItens[j] == ALVO_C_INICIAL && rowItens[j] == ALVO_R_INICIAL;
			bool posicaoRepetida = colItens[j] == colItens[i] && rowItens[j] == rowItens[i];
			cout << "espinho posicao col " << colItens[j] << " " << colItens[i] << endl;
			cout << "espinho posicao row " << rowItens[j] << " " << rowItens[i] << endl;
			cout << "1 " << mesmaPosicaoCarro << "2 " << mesmaPosicaoAlvo << "3 " <<posicaoRepetida;

			if (posicaoRepetida ||
				mesmaPosicaoCarro ||
				mesmaPosicaoAlvo
				) {
				colItens.erase(colItens.begin() + j);
				rowItens.erase(rowItens.begin() + j);
				--i;
				break;
			}
		}
	}

	cout << "adiciona o espinho" << endl;
	for (int i = 0; i < rowItens.size(); i++)
	{
		layers[espinho].adicionarObjeto(400.0f, 200.0f, 3.0f, 17.0f, 25.0f, 0.0f, 0.0f, 0.0f, 0.0f, shader->ID, vertices, 0);
		layers[espinho].objects[i]->setRC(rowItens[i], colItens[i]);
	}
	cout << "saiu da adicao espinho" << endl;
}
void redefinirParametros() {
	cout << "entrando tmap" << endl;

	criaTmap();
	cout << "saindo tmap" << endl;
	cout << "entrando itens camada" << endl;

	layers[pneu].objects.clear();
	layers[pneu].updateCamada();
	layers[espinho].objects.clear();
	layers[espinho].updateCamada();
	cout << "saindo itens camada" << endl;
	cout << "entrando gerar itens" << endl;

	gerarItens();
	cout << "saindo gerar itens" << endl;

	setTexturas();

	layers[carro].objects[0]->setRC(0, 30);

	if (nivelAtual == 2) {
		layers[alvo].objects[1]->setRC(ALVO_R_INICIAL, ALVO_C_INICIAL);
		layers[alvo].objects[0]->setRC(ALVO_R_INICIAL + 50, ALVO_C_INICIAL + 50);
	}
	else {
		layers[alvo].objects[0]->setRC(29, 1);
		layers[alvo].objects[1]->setRC(ALVO_R_INICIAL + 50, ALVO_C_INICIAL + 50);
	}



	carrinhoC = 30;
	carrinhoR = 0;

	offsety = 0.5;
	layers[carro].objects[0]->setOffsetY(offsety);
	glUniform1f(glGetUniformLocation(shader->ID, "offsety"), layers[carro].objects[0]->getOffsetY());

	layers[carro].updateCamada();
	layers[alvo].updateCamada();
	layers[chao].updateCamadaSemDraw();
	integridade = INTEGRIDADE_INICIAL;

	cout << "entrando flecha" << endl;

	updateArrowAngle();
	cout << "saindo flecha" << endl;
	cout << "entrando ortho" << endl;

	updateOrtho();
	cout << "saindo ortho" << endl;

}

void restartGame(string motivo) {
	cout << motivo << endl;
	redefinirParametros();
}

void checkDamage() {
	//cout << "Entrou tomando dano" << endl;

	int carroCurrentC = layers[carro].objects[0]->getC();
	int carroCurrentR = layers[carro].objects[0]->getR();

	bool colunaForaDoLimite = carroCurrentC < 1 || carroCurrentC > 30;
	bool linhaForaDoLimite = carroCurrentR < 0 || carroCurrentR > 29;

	if (colunaForaDoLimite || linhaForaDoLimite) {
		restartGame("Seu carro caiu na agua! A fase sera reiniciada.");
	}
	else {
		for (int a = 0; a < lamaList.size(); a++) {
			if (tmap->getTile(carroCurrentC - 1, carroCurrentR) == lamaList[a]) {
				//cout << "posicao" << a << endl;
				//cout << "tile" << tmap->getTile(carroCurrentC - 1, carroCurrentR) << endl;

				//cout << "lamaList" << lamaList[a] << endl;
				//cout << "current c e r" << carroCurrentC - 1 << " " << carroCurrentR << endl;

				integridade -= 10;
				cout << "Lama da pesada! Sua integridade caiu." << endl;
				cout << "Integridade atual: " << integridade << endl;
			}
		}
		if (integridade <= 0) {
			restartGame("Seu carro está detonado! A fase sera reiniciada.");
		}
	}
}

unsigned int criaChao() {
	// set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
	float vertices[] = {
		// positions   // texture coords
		xi    , yi + th2, 0.0f, tileH2,   // left
		xi + tw2, yi    , tileW2, 0.0f,   // bottom
		xi + tw , yi + th2, tileW, tileH2,  // right
		xi + tw2, yi + th , tileW2, tileH,  // top
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		3, 1, 2  // second triangle
	};

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	return VAO;
}

void setPosicoes() {
	float verticesCarro[] = {
		// positions  // texture coords
		0.5f,  0.5f, .333f, 1.0, // top right
		0.5f, -0.5f,  0.33f, 0.75f, // bottom right
		-0.5f, -0.5f, 0.0f, 0.75f, // bottom left
		-0.5f,  0.5f, 0.0f, 1.0  // top left 
	};
	float verticesChao[] = {
		// positions   // texture coords
		xi      , yi + th2, 0.0f, tileH2,   // left
		xi + tw2, yi    , tileW2, 0.0f,   // bottom
		xi + tw , yi + th2, tileW, tileH2,  // right
		xi + tw2, yi + th , tileW2, tileH,  // top
	};

	float verticesGeral[] = {
		// positions  // texture coords
		0.5f,  0.5f, 1.0f, 1.0f, // top right
		0.5f, -0.5f,  1.0f, 0, // bottom right
		-0.5f, -0.5f, 0, 0, // bottom left
		-0.5f,  0.5f,  0, 1.0f  // top left 
	};

	// Definindo posições de cada Sprite

	layers[carro].adicionarObjeto(400, 200, 2.0f, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, 0.0f, shader->ID, verticesCarro, 0);
	layers[carro].objects[0]->setRC(CARRO_R_INICIAL, CARRO_C_INICIAL);

	layers[alvo].adicionarObjeto(400, 200, 2.0f, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, 0.0f, shader->ID, verticesCarro, 0);
	layers[alvo].objects[0]->setRC(ALVO_R_INICIAL, ALVO_C_INICIAL);

	layers[alvo].adicionarObjeto(400, 200, 2.0f, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, 0.0f, shader->ID, verticesGeral, 0);
	layers[alvo].objects[1]->setRC(CARRO_R_INICIAL + 50, CARRO_C_INICIAL + 50);

	//layers[pneu].adicionarObjeto(400.0f, 200.0f, 2.0f, 22.0f, 22.0f, 0.0f, 0.0f, 0.0f, 0.0f, shader->ID, verticesPneu, 0);

	layers[arrow].adicionarObjeto(400.0f, 200.0f, 3.0f, 22.0f, 22.0f, 0.0f, 0.0f, 0.0f, 0.0f, shader->ID, verticesGeral, 0);

	layers[chao].adicionarObjeto(400.0f, 200.0f, 1.0f, 400.0f, 400.0f, 0.0f, 0.0f, 0.0f, 0.0f, shader->ID, verticesChao, 0);

	gerarItens();
}

void nextLevel(string motivo) {
	cout << motivo << endl;
	nivelAtual++;
	if (nivelAtual < 3) {
		cout << "Nivel Atual: " << nivelAtual << endl;
	}
	else {
		nivelAtual = 0;
		cout << "Nivel Atual: " << nivelAtual << endl;
	}
	redefinirParametros();
}


void checkVitoria() {
	if (layers[alvo].objects[0]->getC() == layers[carro].objects[0]->getC() && layers[alvo].objects[0]->getR() == layers[carro].objects[0]->getR()) {
		nextLevel("Voce para a próxima fase!");
	}
	else if (layers[alvo].objects[1]->getC() == layers[carro].objects[0]->getC() && layers[alvo].objects[1]->getR() == layers[carro].objects[0]->getR()) {
		nextLevel("Voce venceu o jogo! O jogo iria iniciar na fase zero.");
	}
}

void setWindowProjection() {
	glm::mat4 projection = glm::ortho((orthoX - 100.0f), (orthoX + 100.0f), (orthoY - 20), (orthoY + 80.0f), -100.0f, 100.0f);
	//cout << orthoX << " " << orthoY;
	GLint projLoc = glGetUniformLocation(shader->ID, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}
void updateOrtho() {
	orthoX = layers[carro].objects[0]->getPosX();
	orthoY = layers[carro].objects[0]->getPosY();
	layers[arrow].objects[0]->setPosition(glm::vec3(orthoX, orthoY + 70.0, 10.0f));
	setWindowProjection();
	//cout << "entrou aqui";
}

void updateArrowAngle() {
	layers[arrow].objects[0]->setAngle((((layers[carro].objects[0]->getPosX() - 400)) / 400) + 1.6);
	//cout << layers[alvo].objects[0]->getPosX();
}
void verificaColisaoItem() {
	int indexColisao = -1;



	for (int i = 0; i < layers[pneu].objects.size(); i++)
	{
		if (layers[pneu].objects[i]->getC() == layers[carro].objects[0]->getC() &&
			layers[pneu].objects[i]->getR() == layers[carro].objects[0]->getR())
		{
			indexColisao = i;
		}
	}



	if (indexColisao >= 0) {
		integridade += 10;
		cout << "Achou um item de reparo! Receba um incremento de integridade." << endl;
		cout << "Integridade Atual: " << integridade << endl;



		layers[pneu].objects.erase(layers[pneu].objects.begin() + indexColisao);
		layers[pneu].updateCamada();
	}

	indexColisao = -1;



	for (int i = 0; i < layers[espinho].objects.size(); i++)
	{
		if (layers[espinho].objects[i]->getC() == layers[carro].objects[0]->getC() &&
			layers[espinho].objects[i]->getR() == layers[carro].objects[0]->getR())
		{
			indexColisao = i;
		}
	}



	if (indexColisao >= 0) {
		integridade -= 10;
		cout << "Espinho! Sua integridade caiu." << endl;
		cout << "Integridade Atual: " << integridade << endl;
		//layers[espinho].objects.erase(layers[espinho].objects.begin() + indexColisao);
		//layers[espinho].updateCamada();
		if (integridade <= 0) {
			restartGame("Seu carro está detonado! A fase sera reiniciada.");
		}
	}
}
void movimentaCarrinho() {
	if (aux_pressionado == true && aux_pressionado_anterior == false) {
		if (mov == direita) {
			carrinhoC++;
			layers[carro].objects[0]->setRC(carrinhoR, carrinhoC);
			layers[carro].updateCamadaSemDraw();
			aux_pressionado_anterior = true;
			updateOrtho();
			layers[carro].updateCamada();
			updateArrowAngle();
			offsety = 0.75;
			verificaColisaoItem();
			checkDamage();
			checkVitoria();
		}
		else if (mov == esquerda) {
			carrinhoC--;
			layers[carro].objects[0]->setRC(carrinhoR, carrinhoC);
			layers[carro].updateCamadaSemDraw();
			aux_pressionado_anterior = true;
			updateOrtho();
			layers[carro].updateCamada();
			updateArrowAngle();
			offsety = 0.25;
			verificaColisaoItem();
			checkDamage();
			checkVitoria();
		}
		else if (mov == frente) {
			carrinhoR++;
			layers[carro].objects[0]->setRC(carrinhoR, carrinhoC);
			layers[carro].updateCamadaSemDraw();
			aux_pressionado_anterior = true;
			updateOrtho();
			layers[carro].updateCamada();
			updateArrowAngle();
			offsety = 0.5;
			verificaColisaoItem();
			checkDamage();
			checkVitoria();
		}
		else if (mov == tras) {
			carrinhoR--;
			layers[carro].objects[0]->setRC(carrinhoR, carrinhoC);
			layers[carro].updateCamadaSemDraw();
			aux_pressionado_anterior = true;
			updateOrtho();
			layers[carro].updateCamada();
			updateArrowAngle();
			offsety = 0.0;
			verificaColisaoItem();
			checkDamage();
			checkVitoria();

		}
	}
}

void addShader(string vFilename, string fFilename)
{
	shader = new Shader(vFilename.c_str(), fFilename.c_str());
}

void SRD2SRU(double& mx, double& my, float& x, float& y) {
	x = xi + (mx / SCREEN_WIDTH) * w;
	y = yi + (1 - (my / SCREEN_HEIGHT)) * h;
}

void mouse(double& mx, double& my) {

	// cout << "DEBUG => mouse click" << endl;

	// 1) Definição do tile candidato ao clique
	float y = 0;
	float x = 0;
	//cout << mx << " , " << my << "\n";
	SRD2SRU(mx, my, x, y);
	//cout << x << " , " << y << "\n";
	x = x / 4;
	y = y / 4;


	int c, r;
	tview->computeMouseMap(c, r, tw, th, x, y);


	c += layers[carro].objects[0]->getC() - 31;
	r += layers[carro].objects[0]->getR() + 30;



	//cout << "cr " << c << " , " << r << "\n";
	// cout << "\tDEBUG => r: " << r << " c: " << c << endl;

	// 2) Verificar se o ponto pertence ao tile indicado:

	// 2.1) Normalização do clique:
	float x0, y0;
	tview->computeDrawPosition(c, r, tw, th, x0, y0);
	//x0 += xi;


	float point[] = { x, y };

	// 2.2) Verifica se o ponto está dentro do triângulo da esquerda ou da direita do losangulo (metades)
	//      Implementação via cálculo de área dos triangulos: area(ABC) == area(ABp)+area(ACp)+area(BCp)
	// triangulo ABC:
	float* abc = new float[6];

	// 2.2.1) Define metade da esquerda ou da direita
	bool left = x < (x0 + tw / 2.0f);
	// cout << "\tDEBUG => mx: " << x << " midx: " << (x0 + tw/2.0f) << endl; 

	if (left) { // left
		abc[0] = x0;           abc[1] = y0 + th / 2.0f;
		abc[2] = x0 + tw / 2.0f; abc[3] = y0 + th;
		abc[4] = x0 + tw / 2.0f; abc[5] = y0;
		// cout << "DEBUG => TRG LFT [(x,y),...] = ([" << abc[0] << "," << abc[1] << "], "
		// << "[" << abc[2] << "," << abc[3] << "], "
		// << "[" << abc[4] << "," << abc[5] << "])" << endl;
	}
	else { // right
		abc[0] = x0 + tw / 2.0f; abc[1] = y0;
		abc[2] = x0 + tw / 2.0f; abc[3] = y0 + th;
		abc[4] = x0 + tw;      abc[5] = y0 + th / 2.0f;
		// cout << "DEBUG => TRG RGHT [(x,y),...] = ([" << abc[0] << "," << abc[1] << "], "
		// << "[" << abc[2] << "," << abc[3] << "], "
		// << "[" << abc[4] << "," << abc[5] << "])" << endl;
	}

	// 2.3) Calcular colisão do ponto com o triangulo
	bool collide = triangleCollidePoint2D(abc, point);

	if (!collide) {
		// 2.4) Em caso "erro" de cálculo, deve ser feito o tileWalking para tile certo!
		//cout << "tileWalking " << endl;
		if (left) {
			tview->computeTileWalking(c, r, DIRECTION_WEST);
		}
		else {
			tview->computeTileWalking(c, r, DIRECTION_EAST);
		}
	}

	if ((c < 0) || (c >= tmap->getWidth()) || (r < 0) || (r >= tmap->getHeight())) {
		//cout << "wrong click position: " << c << ", " << r << endl;
		return; // posição inválida!
	}

	//cout << "SELECIONADO c=" << c << "," << r << endl;
	cx = c; cy = r;
}

void drawSprites() {

	if (((current_seconds - previous) > (sprite_speed)))
	{
		previous = current_seconds;
		if (cima == true) {
			offsety = offsety + fh;
			cima = false;
		}
		else if (cima == false) {
			offsety = offsety - fh;
			cima = true;
		}
	}
	layers[carro].objects[0]->setOffsetY(offsety);
	glUniform1f(glGetUniformLocation(shader->ID, "offsety"), layers[carro].objects[0]->getOffsetY());


	layers[pneu].updateCamada();
	layers[espinho].updateCamada();
	layers[alvo].updateCamada();
	layers[carro].updateCamada();
	layers[arrow].updateCamada();
	layers[chao].updateCamadaSemDraw();
}


void desenhaTileMap(unsigned int VAO) {
	glUseProgram(shader->ID);

	glBindVertexArray(VAO);
	float x, y;
	int r = 0, c = 0;
	for (int c = 0; c < tmap->getWidth(); c++) {
		for (int r = 0; r < tmap->getHeight(); r++) {
			int t_id = (int)tmap->getTile(c, r);
			//cout << ((int)t_id) << " ";
			int u = t_id % tileSetCols;
			int v = t_id / tileSetCols;

			tview->computeDrawPosition(c, r, tw, th, x, y);

			glUniform1f(glGetUniformLocation(shader->ID, "offsetx"), u * tileW);
			glUniform1f(glGetUniformLocation(shader->ID, "offsety"), v * tileH);
			glUniform1f(glGetUniformLocation(shader->ID, "tx"), x);
			glUniform1f(glGetUniformLocation(shader->ID, "ty"), y + 1.0);
			glUniform1f(glGetUniformLocation(shader->ID, "layer_z"), tmap->getZ());
			glUniform1f(glGetUniformLocation(shader->ID, "weight"), (c == cx) && (r == cy) ? 0.5 : 0.0);


			// bind Texture
			// glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tmap->getTileSet());

			glUniform1i(glGetUniformLocation(shader->ID, "sprite"), 0);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}
	glUniform1f(glGetUniformLocation(shader->ID, "weight"), 0.0);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

int main()
{
	// 1 - Inicialização da GLFW
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	// 2 - Criação do contexto gráfico (window)
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GB Rafael Brustolin e Gabriel Hoffmann", nullptr, nullptr);
	if (!window) {
		fprintf(stderr, "*** ERRO: não foi possível abrir janela com a GLFW\n");
		// 2.0 - Se não foi possível iniciar GLFW, então termina / remove lib GLFW da memória.
		glfwTerminate();
		return 1;
	}

	// 2.1 - Torna janela atual como o contexto gráfico atual para desenho
	glfwMakeContextCurrent(window);

	// Callback do teclado
	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	addShader("./shaders/vertex.vs", "./shaders/fragment.frag");

	criaTmap();
	VAO_CHAO = criaChao();
	setPosicoes();
	setTexturas();
	drawSprites();
	updateOrtho();
	updateArrowAngle();
	previous = glfwGetTime();

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	cout << "Bem-vindo ao Lama Rush, um jogo cheio de emocoes onde voce precisara usar da logica,\npaciencia e uma pitada de sorte para chegar ate o final. Seu objetivo e chegar ate o outro\nlado da ilha, sem cair na agua, e sem entrar demais na lama. Entrar na lama ou atingir um\nespinho desconta 10 na sua integridade, se voce atingir um pneu, sua integridade e acrescida\nem 10. Sua integridade inicial e de 100, e a cada fase um carro melhor sera recebido. Boa sorte!" << endl;
	while (!glfwWindowShouldClose(window))
	{

		glfwPollEvents();

		current_seconds = glfwGetTime();

		// wipe the drawing surface clear
		glClearColor(0.0f, 0.3f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		desenhaTileMap(VAO_CHAO);

		movimentaCarrinho();

		drawSprites();



		double mx, my;
		glfwGetCursorPos(window, &mx, &my);

		const int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

		if (state == GLFW_PRESS) {
			mouse(mx, my);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		// put the stuff we've been drawing onto the display
		glfwSwapBuffers(window);
	}
	// close GL context and any other GLFW resources
	glfwTerminate();
	delete tmap;
	return 0;
}
