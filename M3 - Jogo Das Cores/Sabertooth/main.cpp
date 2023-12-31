// M3 - Jogo das Cores - Rafael Brustolin e Gabriel Hoffmann

#include <GL/glew.h>    // inclui lib auxiliar GLEW e a versão mais recente da OpenGL
#include <GLFW/glfw3.h> // GLFW biblioteca para interface com SO (janela, mouse, teclado, ...)
#include <stdio.h>      // biblioteca padrão C para I/O
#include <vector>
#include <iostream>
#include <random>

int RECTANGLES_NUMBER = 109, SCREEN_WIDTH = 640, SCREEN_HEIGHT = 480, COLOR_SIMILARITY_PERCENTAGE = 20, ROUNDS = 4;
float xSize, ySize;
int oldMouseState = GLFW_RELEASE, lines, columns, currentRound = 1, score = 0;
std::vector<GLfloat> vertices;
std::vector<GLfloat> colors;
std::vector<bool> visibility;

bool isSimilarColor(float distanceBetweenColors) {
    float maxDistance = (COLOR_SIMILARITY_PERCENTAGE / static_cast<float>(100)) * (sqrt(3.0f));

    return distanceBetweenColors <= maxDistance;
}

void handleRoundScore(int eliminatedCount) {
    int scoreToAdd = eliminatedCount * (10 / currentRound);
    score += scoreToAdd;
    std::cout << eliminatedCount << " retangulos eliminados!" << std::endl;
    std::cout << "Pontos feitos na rodada: " << scoreToAdd << std::endl;
    std::cout << "Pontuacao total: " << score << std::endl;
    std::cout << std::endl;
}

void eliminateSimilarRectangles(float red, float green, float blue) {
    int eliminatedCount = 0;

    for (int i = 0; i < RECTANGLES_NUMBER; i++)
    {
        float redDistance = red - colors[i * 3];
        float greenDistance = green - colors[i * 3 + 1];
        float blueDistance = blue - colors[i * 3 + 2];
        float distance = sqrt(redDistance * redDistance + greenDistance * greenDistance + blueDistance * blueDistance);

        if (isSimilarColor(distance)) {
            eliminatedCount++;
            visibility[i] = false;
        }
    }

    handleRoundScore(eliminatedCount);
}

float getRandomValue() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    return dis(gen);
}

void setVertices() {
    float sqrtAux = sqrt(RECTANGLES_NUMBER);
    lines = trunc(sqrtAux);
    columns = ceil(static_cast<double>(RECTANGLES_NUMBER) / lines);
    ySize = 2.0f / lines;
    xSize = 2.0f / columns;

    float currentX = -1.0f;
    float currentY = 1.0f;
    for (int currentRectangle = 0; currentRectangle < RECTANGLES_NUMBER; currentRectangle++) {
        vertices.insert(vertices.end(), {
            currentX, currentY,
            currentX + xSize,  currentY,
            currentX + xSize,  currentY - ySize,
            currentX + xSize,  currentY - ySize,
            currentX,  currentY - ySize,
            currentX,  currentY
        });

        currentX += xSize;

        if (currentX >= 1.0f) {
            currentX = -1.0f;
            currentY -= ySize;
        }
    }
}

void setColors() {
    for (int i = 0; i < RECTANGLES_NUMBER; i++)
    {
        colors.push_back(getRandomValue());
        colors.push_back(getRandomValue());
        colors.push_back(getRandomValue());
    }
}

void setVisibility() {
    for (int i = 0; i < RECTANGLES_NUMBER; i++)
    {
        visibility.push_back(true);
    }
}

void handleMouseClick(double mx, double my) {
    float xCoordinate = (mx * 2.0f / SCREEN_WIDTH) - 1.0f;
    float yCoordinate = ((my * 2.0f / SCREEN_HEIGHT) - 1.0f) * -1.0f;

    float columnPosition;
    for (int i = 0; i < columns; i++)
    {
        float currentColumnInitialX = i * xSize - 1.0f;
        float currentColumnFinalX = (i + 1) * xSize - 1.0f;

        if (xCoordinate >= currentColumnInitialX && xCoordinate <= currentColumnFinalX) {
            columnPosition = i;
        }
    }

    float linePosition;
    for (int i = 0; i < lines; i++)
    {
        float currentLineInitialY = (i * ySize - 1.0f) * -1.0f;
        float currentLineFinalY = ((i + 1) * ySize - 1.0f) * -1.0f;

        if (yCoordinate <= currentLineInitialY && yCoordinate >= currentLineFinalY) {
            linePosition = i;
        }
    }

    int rectanglePosition = (linePosition * columns) + columnPosition;

    if (RECTANGLES_NUMBER <= rectanglePosition || !visibility[rectanglePosition]) {
        std::cout << "Clique nao corresponde a um retangulo" << std::endl;
        std::cout << std::endl;
        return;
    }

    float red = colors[rectanglePosition * 3];
    float green = colors[rectanglePosition * 3 + 1];
    float blue = colors[rectanglePosition * 3 + 2];

    eliminateSimilarRectangles(red, green, blue);

    currentRound++;
    if (currentRound <= ROUNDS) {
        std::cout << "Rodada: " << currentRound << std::endl;
    }
    else {
        std::cout << "Fim de Jogo!" << std::endl;
    }

    std::cout << std::endl;
}

int main() {
    // 1 - Inicialização da GLFW
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }
    // 1.1 - Necessário para Apple OS X
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2 - Criação do contexto gráfico (window)
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Buffers - Dados e Layout", NULL, NULL);
    if (!window) {
        fprintf(stderr, "*** ERRO: não foi possível abrir janela com a GLFW\n");
        // 2.0 - Se não foi possível iniciar GLFW, então termina / remove lib GLFW da memória.
        glfwTerminate();
        return 1;
    }
    // 2.1 - Torna janela atual como o contexto gráfico atual para desenho
    glfwMakeContextCurrent(window);

    // 3 - Inicia manipulador da extensão GLEW 
    glewExperimental = GL_TRUE;
    glewInit();

    // 4 - Objetivo do exemplo: detectar a versão atual do OpenGL e motor de renderização.
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    printf("\n** Verificação da OpenGL ************\n");
    printf("\tRenderer: %s\n", renderer);
    printf("\tVersão suportada da OpenGL %s\n\n", version);

    /**************
     * 5 - AQUI VEM TODO O CÓDIGO DA APP GRÁFICA, PRINCIPALMENTE, O MAIN LOOP
     **************/

    setVertices();
    setColors();
    setVisibility();

     // 5.1 - Geometria da cena

    GLuint vboVert; // identificador de vértices
    glGenBuffers(1, &vboVert);
    glBindBuffer(GL_ARRAY_BUFFER, vboVert);
    // 5.1.1 - Comando que passa dados de vértices da memória da CPU -> memória da GPU 
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);


    // 5.2 - Objeto de propriedades / layout
    // como variável foi criada dentro do método main, este é o seu escopo!
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // 5.2.1 - Vínculo entre áreas de dados e layout de leitura de memória na GPU
    // 5.2.1.1 - Vinculando layout de leitura dos vértices
    glBindBuffer(GL_ARRAY_BUFFER, vboVert); // identifica vbo atual
    //   habilitado primeiro atributo do vbo (bind atual)
    glEnableVertexAttribArray(0);
    //   associação do vbo atual com primeiro atributo
    //   0 (primeiro 0) identifica que o primeiro atributo está sendo definido
    //   3, GL_FLOAT identifica que dados são vec3 e estão a cada 3 float, 
    //   começando no primeiro byte (último 0).
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    // 5.3 - Definição dos shaders para renderização
    // 5.3.1 - Vertex shader é o estagio de entrada, é ele quem recebe os dados da GPU:
    const char* vertex_shader =
        "#version 410\n"
        "layout(location=0) in vec2 vp;"    // layout 0 foi utilizado para receber vec3 do vértice
        "uniform vec3 rectColor;"
        "out vec3 color;"
        "void main () {"
        "   color = rectColor;"
        "	gl_Position = vec4 (vp, 0.0, 1.0);"  //objetivo do VS, posicionar o vértice vp no sistema de referência do universo
        "}";

    // 5.3.2 - é o estágio de saída do pipeline que podemos intervir
    const char* fragment_shader =
        "#version 410\n"
        "in vec3 color;"                    // recebe a cor saída do estágio anterior VS
        "out vec4 frag_color;"
        "void main () {"
        "	 frag_color = vec4 (color, 1.0);" // objetivo do FS, determinar a cor do fragmento
        "}";

    // 5.4 - Compilação e "linkagem" dos shaders num shader programm
    // identifica vs e o associa com vertex_shader
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);

    int params = -1;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &params);
    if (GL_TRUE != params)
    {
        fprintf(stderr, "ERROR: GL shader index %i did not compile\n", vs);
        // print_shader_info_log(vs);
        glfwTerminate();
        return 1; // or exit or something
    }

    // identifica fs e o associa com fragment_shader
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &params);
    if (GL_TRUE != params)
    {
        fprintf(stderr, "ERROR: GL shader index %i did not compile\n", fs);
        // print_shader_info_log(fs);
        glfwTerminate();
        return 1; // or exit or something
    }

    // identifica do programa, adiciona partes e faz "linkagem"
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, fs);
    glAttachShader(shader_program, vs);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &params);
    if (GL_TRUE != params)
    {
        fprintf(stderr, "ERROR: could not link shader programme GL index %i\n",
            shader_program);
        // print_programme_info_log( shader_programm);
        glfwTerminate();
        return 1;
    }

    // 5.4 - Finalmente, loop de desenho. Note que até o momento pipeline não foi utilizado!
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    std::cout << "Rodada: " << currentRound << std::endl;
    std::cout << std::endl;
    while (!glfwWindowShouldClose(window)) {
        const int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (state == GLFW_RELEASE && oldMouseState == GLFW_PRESS && currentRound <= ROUNDS) {
            double mx, my;
            glfwGetCursorPos(window, &mx, &my);
            handleMouseClick(mx, my);
        }

        oldMouseState = state;

        glClear(GL_COLOR_BUFFER_BIT);

        // Define shader_programme como o shader a ser utilizado
        glUseProgram(shader_program);

        // Define vao como verte array atual 
        glBindVertexArray(vao);

        for (int i = 0; i < RECTANGLES_NUMBER; i++)
        {
            if (visibility[i]) {
                glUniform3f(glGetUniformLocation(shader_program, "rectColor"), colors[i * 3], colors[i * 3 + 1], colors[i * 3 + 2]);
            }
            else {
                glUniform3f(glGetUniformLocation(shader_program, "rectColor"), 0.0f, 0.0f, 0.0f);
            }

            glDrawArrays(GL_TRIANGLES, i * 6, 6);

            glUniform3f(glGetUniformLocation(shader_program, "rectColor"), 0.0f, 0.0f, 0.0f);
            glDrawArrays(GL_LINE_LOOP, i * 6, 6);
        }

        // 5.4.2 - Este comando faz controle double buffering, obtendo imagem do framebuffer 
        //         gerado pela OpenGL para renderização no contexto gráfico (window).
        glfwSwapBuffers(window);

        // Processa eventos da GLFW
        glfwPollEvents();
    }

    // 6 - Ao final, terminar / remover GLFW da memória.
    glfwTerminate();
    return 0;
}
