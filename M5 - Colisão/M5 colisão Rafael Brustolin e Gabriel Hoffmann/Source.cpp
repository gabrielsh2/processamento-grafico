#include "Layer.h"
#include "Shader.h"
#include "Sprite.h" 
#include "Layer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

using namespace std;

static bool keys[1024];
static GLuint width, height;

const int layer0 = 4;
const int layer1 = 3;
const int layer2 = 2;
const int layer3 = 1;
const int layer4 = 0;

const int personagem = 5;
const int pedra = 6;

enum Movimento { esquerda, parado, direita, quebra };
static Movimento mov = parado;

int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600, frameAtual = 0;
float PARALLAX_RATE = 0.00005f, fw = 0.125f, offsetx = 0, previous, previous_pulo, sprite_speed = 0.1;
double current_seconds;
bool colidindo = false;

Shader* shader;

Layer* layers = new Layer[7];
vector <Sprite*> objects;

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
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		mov = direita;
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		mov = esquerda;
	}
	else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		mov = quebra;
	}
	else
	{
		mov = parado;
	}
}

void addShader(string vFilename, string fFilename)
{
	shader = new Shader(vFilename.c_str(), fFilename.c_str());
}

void setPosicoes() {
	float verticesCena[] = {
		// positions          // colors           // texture coords
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0, // top right
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0  // top left 
	};
	float verticesPersonagem[] = {
		// positions          // colors           // texture coords
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   .125f, 1.0, // top right
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   .125f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.006f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.006f, 1.0  // top left 
	};

	// Definindo posições de cada Sprite
	layers[layer4].adicionarObjeto(400.0f, 300.0f, -40.0, 800.0f, 600.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, shader->ID, verticesCena);
	layers[layer3].adicionarObjeto(400.0f, 300.0f, -35.0, 800.0f, 600.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, shader->ID, verticesCena);
	layers[layer2].adicionarObjeto(400.0f, 300.0f, -30.0, 800.0f, 600.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, shader->ID, verticesCena);
	layers[layer1].adicionarObjeto(400.0f, 300.0f, -25.0, 800.0f, 600.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, shader->ID, verticesCena);
	layers[layer0].adicionarObjeto(400.0f, 300.0f, -20.0, 800.0f, 600.0f, 0.0f, 0.0f, 0.0f, 3.0f, 0.0f, shader->ID, verticesCena);
	layers[personagem].adicionarObjeto(80.0f, 210.0f, -1.0f, 100.0f, 200.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, shader->ID, verticesPersonagem);
	layers[personagem].setDeslocamento(0.2f);
	layers[pedra].adicionarObjeto(800.0f, 150.0f, -1.0f, 100.0f, 100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, shader->ID, verticesCena);
	layers[pedra].setDeslocamento(0.12f);
}

void setTexturas() {
	// Adicionando Textura para cada Sprite
	unsigned int texID = loadTexture("./textures/fundo.png");
	layers[layer4].objects[0]->setTexture(texID);

	texID = loadTexture("./textures/sol.png");
	layers[layer3].objects[0]->setTexture(texID);

	texID = loadTexture("./textures/cidade.png");
	layers[layer2].objects[0]->setTexture(texID);

	texID = loadTexture("./textures/arvores.png");
	layers[layer1].objects[0]->setTexture(texID);

	texID = loadTexture("./textures/chao.png");
	layers[layer0].objects[0]->setTexture(texID);

	texID = loadTexture("./textures/homem.png");
	layers[personagem].objects[0]->setTexture(texID);

	texID = loadTexture("./textures/pedra.png");
	layers[pedra].objects[0]->setTexture(texID);
}

void setWindowProjection() {
	glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -100.0f, 100.0f);
	GLint projLoc = glGetUniformLocation(shader->ID, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void movimentaPersonagem() {
	switch (mov)
	{
	case esquerda:
		layers[personagem].objects[0]->setPosX(layers[personagem].objects[0]->getPosX() - layers[personagem].getDeslocamento());
		sprite_speed = 0.1;
		break;
	case direita:
		if (colidindo == false) {
			layers[personagem].objects[0]->setPosX(layers[personagem].objects[0]->getPosX() + layers[personagem].getDeslocamento());
			sprite_speed = 0.06;
		}
		break;
	case quebra:
		if (colidindo == true) {
			layers[pedra].objects[0]->setPosX(900.0f);
			layers[pedra].setDeslocamento(0.12f);
			layers[layer0].objects[0]->setRateX(3.0);
			layers[layer1].objects[0]->setRateX(2.0);
			layers[layer2].objects[0]->setRateX(1.0);
			colidindo = false;
		}
		break;
	default:
		sprite_speed = 0.08;
		break;
	}

	layers[personagem].objects[0]->setPosition(glm::vec3(layers[personagem].objects[0]->getPosX(), layers[personagem].objects[0]->getPosY(), layers[personagem].objects[0]->getPosZInicial()));
}

void movimentaPedra() {
	if (colidindo == false) {
		layers[pedra].objects[0]->setPosX(layers[pedra].objects[0]->getPosX() - layers[pedra].getDeslocamento());
		layers[pedra].objects[0]->setPosition(glm::vec3(layers[pedra].objects[0]->getPosX(), layers[pedra].objects[0]->getPosY(), layers[pedra].objects[0]->getPosZInicial()));
	}
	else {
		layers[pedra].setDeslocamento(0.0f);
	}
}

void verificaColisao() {
	if ((layers[personagem].objects[0]->getPosX() + 31 >= layers[pedra].objects[0]->getPosX() - 32.5f) && (layers[personagem].objects[0]->getPosX() < layers[pedra].objects[0]->getPosX()))
	{
		colidindo = true;
		layers[layer0].objects[0]->setRateX(0);
		layers[layer1].objects[0]->setRateX(0);
		layers[layer2].objects[0]->setRateX(0);
	}
	else {
		colidindo = false;
	}
}

void drawSprites() {
	for (int i = 0; i < 7; i++)
	{
		if (i == personagem) {
			if (((current_seconds - previous) > (sprite_speed)) && colidindo == false)
			{
				previous = current_seconds;

				if (frameAtual == 7) {
					frameAtual = 0;
				}
				else {
					frameAtual++;
				}

				offsetx = fw * (float)frameAtual;
				layers[personagem].objects[0]->setOffsetX(offsetx);
			}
		}
		else {
			layers[i].objects[0]->setOffsetX(layers[i].objects[0]->getOffsetX() + (PARALLAX_RATE * layers[i].objects[0]->getRateX()));
		}
		glUniform1f(glGetUniformLocation(shader->ID, "offsetx"), layers[i].objects[0]->getOffsetX());
		layers[i].updateCamada();
	}
}

int main()
{
	// 1 - Inicialização da GLFW
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	// 2 - Criação do contexto gráfico (window)
	GLFWwindow*  window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "M5 - Camadas + colisao", nullptr, nullptr);
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

	setPosicoes();
	setTexturas();
	setWindowProjection();
	
	previous = glfwGetTime();
	previous_pulo = glfwGetTime();

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glClearColor(0.89f, 0.89f, 0.87f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		current_seconds = glfwGetTime();

		if (keys[GLFW_KEY_ESCAPE])
			glfwSetWindowShouldClose(window, GL_TRUE);

		movimentaPersonagem();
		movimentaPedra();
		verificaColisao();

		drawSprites();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
}
