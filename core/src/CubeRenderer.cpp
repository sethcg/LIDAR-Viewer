
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
    
    std::vector<Cube> cubes;

    glm::mat4 cachedView;
    glm::mat4 cachedProjection;
    bool matricesInitialized = false;
    
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

    void InitCamera(Application::AppContext* appContext) {
        if (matricesInitialized) return;

        // COMPUTE BOUNDING BOX OF ALL CUBES
        glm::vec3 min(FLT_MAX), max(-FLT_MAX);
        for (const Cube& cube : cubes) {
            glm::vec3 pos = cube.position;
            float s = cube.scale * appContext->globalScale;
            min = glm::min(min, pos - glm::vec3(s));
            max = glm::max(max, pos + glm::vec3(s));
        }

        glm::vec3 center = 0.5f * (min + max);
        float extent = glm::max(max.x - min.x, max.y - min.y) * 0.5f;

        float margin = 10.0f; // extra space
        float viewSize = extent + margin;

        // STATIC TOP-DOWN CAMERA
        cachedView = glm::lookAt(
            glm::vec3(center.x, center.y, 200.0f),  // CAMERA HIGH ABOVE
            glm::vec3(center.x, center.y, 0.0f),    // LOOK AT THE CENTER OF THE CUBES
            glm::vec3(0.0f, 1.0f, 0.0f)             // UP VECTOR
        );

        cachedProjection = glm::ortho(-viewSize, viewSize, -viewSize, viewSize, 0.1f, 500.0f);
        matricesInitialized = true;
    }

    void Render(Application::AppContext* appContext) {
        glViewport(0, 0, appContext->width, appContext->height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        GLint globalColorLocation = glGetUniformLocation(program, "globalColor");
        glUniform3fv(globalColorLocation, 1, glm::value_ptr(appContext->globalColor));

        for(const Cube& cube : cubes) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), cube.position);
            model = glm::scale(model, glm::vec3(cube.scale * appContext->globalScale));

            glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(cachedView));
            glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(cachedProjection));

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