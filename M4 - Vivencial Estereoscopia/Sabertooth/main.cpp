// Atividade Vivencial M4 - Gabriel Hoffmann e Rafael Brustolin

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
using namespace std;

const GLint WIDTH = 800, HEIGHT = 600;
glm::mat4 matrix = glm::mat4(1);
GLuint texture1, texture2;
int chosenFilter;

bool loadTexture(const char* file_name, GLuint* tex) {
    int x, y, n;
    int force_channels = 4;
    glEnable(GL_TEXTURE_2D);
    unsigned char* image_data = stbi_load(file_name, &x, &y, &n, force_channels);
    if (!image_data) {
        fprintf(stderr, "ERROR: could not load %s\n", file_name);
        return false;
    }

    glGenTextures(1, tex);
    glBindTexture(GL_TEXTURE_2D, *tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    GLfloat max_aniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
    // set the maximum!
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);
    return true;
}

void chooseAnaglyphFilter() {
    cout << "Digite o número de um filtro anaglifo:\n" << endl;
    cout << "1 - Anaglifo Verdadeiro" << endl;
    cout << "2 - Anaglifo Cinza" << endl;
    cout << "3 - Anaglifo Colorido" << endl;
    cin >> chosenFilter;
}

void chooseImages() {
    string path1;
    string path2;

    cout << "Digite o path da textura 1:" << endl;
    getline(cin >> ws, path1);
    cout << "Digite o path da textura 2:" << endl;
    getline(cin >> ws, path2);

    loadTexture(path1.c_str(), &texture1);
    loadTexture(path2.c_str(), &texture2);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "ORTHO + MOUSE + TEXTURE", nullptr, nullptr);

    if (window == nullptr) {
        std::cout << "Failed to create GLFW Window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to init GLEW." << std::endl;
        return EXIT_FAILURE;
    }

    const char* vertex_shader =
        "#version 410\n"
        "layout (location = 0) in vec3 vPosition;"
        "layout (location = 1) in vec2 vTexture;"
        "out vec2 text_map;"
        "void main() {"
        "    text_map = vTexture;"
        "    gl_Position = vec4(vPosition, 1.0);"
        "}";

    const char* fragment_shader =
        "#version 410\n"
        "in vec2 text_map;"
        "uniform sampler2D texture1;"
        "uniform sampler2D texture2;"
        "uniform int chosen_filter;"
        "out vec4 frag_color;"

        "vec4 calculateFinalColor(vec4 color_left, vec4 color_right) {"
        "   vec4 final_color;"
        "   if (chosen_filter == 1) {"
        "       final_color.r = 0.299 * color_left.r + 0.587 * color_left.g + 0.114 * color_left.b;"
        "       final_color.g = 0;"
        "       final_color.b = 0.299 * color_right.r + 0.587 * color_right.g + 0.114 * color_right.b;"
        "   }"
        "   if (chosen_filter == 2) {"
        "       final_color.r = 0.299 * color_left.r + 0.587 * color_left.g + 0.114 * color_left.b;"
        "       final_color.g = 0.299 * color_right.r + 0.587 * color_right.g + 0.114 * color_right.b;"
        "       final_color.b = 0.299 * color_right.r + 0.587 * color_right.g + 0.114 * color_right.b;"
        "   }"
        "   if (chosen_filter == 3) {"
        "       final_color.r = 1 * color_left.r;"
        "       final_color.g = 1 * color_right.g;"
        "       final_color.b = 1 * color_right.b;"
        "   }"
        "   final_color.a = max(color_left.a, color_right.a);"
        "   return final_color;"
        "}"

        "void main() {"
        "   vec4 color_left = texture(texture1, text_map);"
        "   vec4 color_right = texture(texture2, text_map);"
        "   vec4 final_color = calculateFinalColor(color_left, color_right);"
        "   frag_color = final_color;"
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

    GLfloat vertices[] = {
        // Positions            // Texture coordinates
        -1.0f, -1.0f, 0.0f,     0.0f, 1.0f,
        1.0f, -1.0f, 0.0f,      1.0f, 1.0f,
        1.0f, 1.0f, 0.0f,       1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,       1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,      0.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,     0.0f, 1.0f
    };

    glm::mat4 proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    chooseImages();
    chooseAnaglyphFilter();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int screenWidth, screenHeight;
        glfwGetWindowSize(window, &screenWidth, &screenHeight);
        glViewport(0, 0, screenWidth, screenHeight);

        glUseProgram(shader_programme);
        glUniform1i(glGetUniformLocation(shader_programme, "chosen_filter"), chosenFilter);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glUniform1i(glGetUniformLocation(shader_programme, "texture1"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glUniform1i(glGetUniformLocation(shader_programme, "texture2"), 1);
        GLint matrix_location = glGetUniformLocation(shader_programme, "matrix");
        glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(proj * matrix));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glDeleteTextures(1, &texture1);
    glDeleteTextures(1, &texture2);
    glDeleteShader(fs);
    glDeleteShader(vs);
    glDeleteProgram(shader_programme);

    glfwTerminate();
    return 0;
}