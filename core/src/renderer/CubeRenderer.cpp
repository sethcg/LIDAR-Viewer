#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <CubeRenderer.hpp>
#include <RendererHelper.hpp>

namespace CubeRenderer {

    static bool stateChanged = true;
    static float globalScale = 0.5f;
    static glm::vec3 globalColor = glm::vec3(1.0f);

    static std::vector<Cube> cubes;

    static GLuint vao = 0;
    static GLuint vbo = 0;
    static GLuint instanceVBO = 0;
    static GLuint instanceColorVBO = 0;
    static GLuint shaderProgram = 0;

    static GLint uViewProjectionLocation = -1;
    static GLint uGlobalColorLocation = -1;
    static GLint uGlobalScaleLocation = -1;

    static std::vector<glm::mat4> instanceModels;
    static std::vector<glm::vec3> instanceColors;

    void Init() {
        std::string vertexSource   = RendererHelper::LoadTextFile("../assets/shaders/cube/cube.vert");
        std::string fragmentSource = RendererHelper::LoadTextFile("../assets/shaders/cube/cube.frag");

        GLuint vertexShader   = RendererHelper::CreateShader(vertexSource, GL_VERTEX_SHADER);
        GLuint fragmentShader = RendererHelper::CreateShader(fragmentSource, GL_FRAGMENT_SHADER);

        shaderProgram = RendererHelper::CreateShaderProgram(vertexShader, fragmentShader);

        uViewProjectionLocation = glGetUniformLocation(shaderProgram, "uViewProjection");
        uGlobalColorLocation = glGetUniformLocation(shaderProgram, "uGlobalColor");
        uGlobalScaleLocation = glGetUniformLocation(shaderProgram, "uGlobalScale");

        // CUBE VERTEX DATA
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &instanceVBO);
        glGenBuffers(1, &instanceColorVBO);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // VERTEX POSITIONS
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        // INSTANCE MODEL MATRIX (4 vec4 ATTRIBUTES)
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        for (int i = 0; i < 4; ++i) {
            glEnableVertexAttribArray(1 + i);
            glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
            glVertexAttribDivisor(1 + i, 1); // ADVANCE PER INSTANCE
        }

        // INSTANCE COLOR
        glBindBuffer(GL_ARRAY_BUFFER, instanceColorVBO);
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glVertexAttribDivisor(5, 1); // ADVANCE PER INSTANCE

        glBindVertexArray(0);

        glEnable(GL_DEPTH_TEST);
    }

    void UpdateInstanceBuffers() {
        if (cubes.empty()) return;
        if(!stateChanged) return;

        std::vector<glm::mat4> models(cubes.size());
        std::vector<glm::vec3> colors(cubes.size());

        for (size_t i = 0; i < cubes.size(); ++i) {
            models[i] = glm::translate(glm::mat4(1.0f), cubes[i].position);
            colors[i] = cubes[i].color;
        }

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, models.size() * sizeof(glm::mat4), models.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, instanceColorVBO);
        glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);

        // CHANGE STATE (AVOIDS UPDATING BUFFERS EACH RENDER FRAME)
        stateChanged = false;
    }

    void Render(Application::AppContext* appContext) {
        glViewport(0, 0, appContext->width, appContext->height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (cubes.empty()) return;

        // CALLED IN MAIN WHEN STATE CHANGES
        // UpdateInstanceBuffers();
        
        glUseProgram(shaderProgram);
        glBindVertexArray(vao);

        glUniform3fv(uGlobalColorLocation, 1, glm::value_ptr(globalColor));
        glUniform1f(uGlobalScaleLocation, globalScale);

        glm::mat4 viewProjection = Camera::GetProjection() * Camera::GetView();
        glUniformMatrix4fv(uViewProjectionLocation, 1, GL_FALSE, glm::value_ptr(viewProjection));

        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, (GLsizei)cubes.size());

        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Shutdown() {
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &instanceVBO);
        glDeleteBuffers(1, &instanceColorVBO);
        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(shaderProgram);
    }

    void Add(const Cube& cube) {
        cubes.push_back(cube);
    }

    void Clear() {
        cubes.clear();
    }

    // ACCESSOR METHODS
    const std::vector<Cube>& GetCubes() { return cubes; };

    const void SetStateChanged(bool state) { stateChanged = state; };

    float& GetGlobalScale() { return globalScale; };
    glm::vec3& GetGlobalColor() { return globalColor; };

}
