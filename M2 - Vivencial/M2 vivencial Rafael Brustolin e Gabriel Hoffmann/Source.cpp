//rafael brustolin e Gabriel Hoffmann

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
using namespace std;

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


const GLint WIDTH = 800, HEIGHT = 600;
glm::mat4 matrix = glm::mat4(1);

GLfloat medioX = 0;
GLfloat medioY = 0;

vector<GLfloat> vertice = { };

void adicionaPontoMedio() {
    vertice.push_back(medioX);
    vertice.push_back(medioY);
    vertice.push_back(0.0f);
    vertice.push_back(0.0f);
    vertice.push_back(0.0f);
    vertice.push_back(1.0f);
}

void calculaPontoMedio() {
    medioX = 0;
    medioY = 0;

    for (int i = 0; i < vertice.size(); i += 6) {
        medioX += vertice[i];
        medioY += vertice[i + 1];
    }
    medioX = (medioX / (vertice.size() / 6));
    medioY = (medioY / (vertice.size() / 6));
}

void adicionaUltimoPonto() {
    vertice.push_back(vertice[vertice.size() - 12]);
    vertice.push_back(vertice[vertice.size() - 12]);
    vertice.push_back(0.0f);
    vertice.push_back(0.0f);
    vertice.push_back(0.0f);
    vertice.push_back(1.0f);
}


void mouse(double mx, double my) {
    
    if (vertice.size() < 18) {
        vertice.push_back(mx);
        vertice.push_back(my);
        vertice.push_back(0.0f);
        vertice.push_back(0.0f);
        vertice.push_back(0.0f);
        vertice.push_back(1.0f);
    } else {
        calculaPontoMedio();
        adicionaPontoMedio();
        adicionaUltimoPonto();
        vertice.push_back(mx);
        vertice.push_back(my);
        vertice.push_back(0.0f);
        vertice.push_back(0.0f);
        vertice.push_back(0.0f);
        vertice.push_back(1.0f);
        calculaPontoMedio();
    }
    glBufferData(GL_ARRAY_BUFFER, vertice.size() * sizeof(GLfloat), &vertice[0], GL_STATIC_DRAW);
}



int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

#pragma region Basic Setup
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "M2 vivencial - Rafael Brustolin e Gabriel Hoffmann", nullptr, nullptr);



    if (window == nullptr) {
        std::cout << "Failed to create GLFW Window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        std::cout << "Failed no init GLEW." << std::endl;
        return EXIT_FAILURE;
    }

#pragma endregion

    const char* vertex_shader =
        "#version 410\n"
        "layout ( location = 0 ) in vec3 vPosition;"
        "layout ( location = 1 ) in vec3 vColor;"
        "uniform mat4 proj;"
        "uniform mat4 matrix;"
        "out vec3 color;"
        "void main() {"
        "    color = vColor;"
        "    gl_Position = proj * matrix * vec4 ( vPosition, 1.0);"
        "}";

    const char* fragment_shader =
        "#version 410\n"
        "in vec3 color;"
        "out vec4 frag_color;"
        "void main(){"
        "  frag_color = vec4(color, 1.0f);"
        "}";

    int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    int shader_programme = glCreateProgram();
    glAttachShader(shader_programme, fs);
    glAttachShader(shader_programme, vs);
    glLinkProgram(shader_programme);

    
    
    glm::mat4 proj = glm::ortho(0.0f, (float)WIDTH, (float)HEIGHT, 0.0f, -1.0f, 1.0f);


    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);



    int oldMouseState = 0;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        const int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (state == GLFW_RELEASE && oldMouseState == GLFW_PRESS) {
            double mx, my;
            glfwGetCursorPos(window, &mx, &my);
            mouse(mx, my);
        }

        oldMouseState = state;
#pragma region Input Handling

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }


#pragma endregion

        glClearColor(0.4f, 0.65f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int screenWidth, screenHeight;
        glfwGetWindowSize(window, &screenWidth, &screenHeight);
        glViewport(0, 0, screenWidth, screenHeight);

        // PASSAGEM DE PARÂMETROS PRA SHADERS
        glUseProgram(shader_programme);
        glUniformMatrix4fv(glGetUniformLocation(shader_programme, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(glGetUniformLocation(shader_programme, "matrix"), 1, GL_FALSE, glm::value_ptr(matrix));

        glBindVertexArray(VAO);
                //triangulos do desenho
        glDrawArrays(GL_TRIANGLES, 0, (vertice.size()/6));

        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}
