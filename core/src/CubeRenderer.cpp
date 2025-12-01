
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <string>

#include <AppContext.hpp>
#include <CubeRenderer.hpp>

namespace CubeRenderer {

    GLuint program = 0;             // SHADER PROGRAM
    GLuint vao = 0;                 // VERTEX ARRAY OBJECT
    GLuint vbo = 0;                 // VERTEX BUFFER OBJECT
    
    float angle = 0.0f;

    std::vector<Cube> cubes;

    float vertices[] = {
        // BACK FACE (RED)
        -0.5f,-0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,
        0.5f, 0.5f,-0.5f,
        0.5f, 0.5f,-0.5f,
        -0.5f, 0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f,

        // FRONT FACE (GREEN)
        -0.5f,-0.5f, 0.5f,
        0.5f,-0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f,-0.5f, 0.5f,

        // LEFT FACE (BLUE)
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f,
        -0.5f,-0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,

        // RIGHT FACE (YELLOW)
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,
        0.5f,-0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,

        // BOTTOM FACE (MAGENTA)
        -0.5f,-0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,
        0.5f,-0.5f, 0.5f,
        0.5f,-0.5f, 0.5f,
        -0.5f,-0.5f, 0.5f,
        -0.5f,-0.5f,-0.5f,

        // TOP FACE (CYAN)
        -0.5f, 0.5f,-0.5f,
        0.5f, 0.5f,-0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f,-0.5f,
    };

    
    std::string LoadTextFile(const char* path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Failed to load shader file: %s\n", path);
            return "";
        }
        std::stringstream stream;
        stream << file.rdbuf();
        return stream.str();
    }

    void CheckProgram(GLuint program) {
        GLint success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if(!success) {
            char log[1024];
            glGetProgramInfoLog(program, 512, nullptr, log);
            SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Program link error: %s\n", log);
        }
    }

    GLuint CompileShader(GLenum type, const char* src) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success) {
            char info[512];
            glGetShaderInfoLog(shader, 512, nullptr, info);
            SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Shader compile error: %s\n", info);
        }
        return shader;
    }

    GLuint CreateShaderProgram() {
        std::string vertexSource = LoadTextFile("../assets/shaders/cube.vert");
        std::string fragmentSource = LoadTextFile("../assets/shaders/cube.frag");

        GLuint vertex = CompileShader(GL_VERTEX_SHADER, vertexSource.c_str());
        GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource.c_str());

        GLuint program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);
        CheckProgram(program);

        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return program;
    }

    void Init() {
        program = CreateShaderProgram();

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    void Render(Application::AppContext* appContext) {
        angle += 0.01f * appContext->rotationSpeed;

        glViewport(0, 0, appContext->width, appContext->height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) appContext->width / appContext->height, 0.1f, 100.0f);

        GLint globalColorLocation = glGetUniformLocation(program, "globalColor");
        glUniform3fv(globalColorLocation, 1, glm::value_ptr(appContext->globalColor));

        for(const Cube& cube : cubes) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), cube.position);
            model = glm::rotate(model, angle, glm::vec3(0, 1, 0));
            model = glm::scale(model, glm::vec3(cube.scale * appContext->globalScale));

            glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

            // SET UNIFORM COLOR
            GLint colorLocation = glGetUniformLocation(program, "cubeColor");
            glUniform3fv(colorLocation, 1, glm::value_ptr(cube.color));

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    void Add(const Cube& cube) { 
        cubes.push_back(cube); 
    }

    void Clear() { 
        cubes.clear(); 
    }
}