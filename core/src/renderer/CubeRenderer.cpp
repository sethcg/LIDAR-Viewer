#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <CubeRenderer.hpp>
#include <RendererHelper.hpp>

namespace CubeRenderer {

    std::vector<Cube> cubes;

    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint instanceVBO = 0;
    GLuint instanceColorVBO = 0;
    GLuint shaderProgram = 0;

    glm::mat4 view;
    glm::mat4 projection;

    GLint uViewProjectionLocation = -1;
    GLint uGlobalColorLocation = -1;
    GLint uGlobalScaleLocation = -1;

    std::vector<glm::mat4> instanceModels;
    std::vector<glm::vec3> instanceColors;

    void Init(Application::AppContext* appContext) {
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

    void InitCamera(Application::AppContext* appContext) {
        if (cubes.empty()) {
            view = glm::lookAt(glm::vec3(0,0,5), glm::vec3(0,0,0), glm::vec3(0,1,0));
            projection = glm::perspective(glm::radians(45.0f), float(appContext->width) / appContext->height, 0.1f, 100.0f);
            return;
        }

        glm::vec3 min(FLT_MAX), max(-FLT_MAX);
        for (const Cube& cube : cubes) {
            glm::vec3 position = cube.position;
            float scale = cube.scale * appContext->globalState->scale;
            min = glm::min(min, position - glm::vec3(scale));
            max = glm::max(max, position + glm::vec3(scale));
        }

        glm::vec3 center = 0.5f * (min + max);
        float extent = glm::max(max.x - min.x, max.y - min.y) * 0.5f;
        float fov = 45.0f;
        float dist = extent / tan(glm::radians(fov) * 0.5f);

        glm::vec3 camPos = center + glm::vec3(0, -dist * 1.3f, dist * 0.9f);
        glm::vec3 camTarget = center;

        view = glm::lookAt(camPos, camTarget, glm::vec3(0,0,1));
        projection = glm::perspective(glm::radians(fov), float(appContext->width) / appContext->height, 0.1f, 5000.0f);
    }

    void UpdateInstanceBuffers(Application::AppContext* appContext) {
        if (cubes.empty()) return;
        if(!appContext->globalState->changed) return;

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
        appContext->globalState->changed = false;
    }

    void Render(Application::AppContext* appContext) {
        Application::GlobalState* globalState = appContext->globalState;

        glViewport(0, 0, appContext->width, appContext->height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (cubes.empty()) return;

        // CALLED IN MAIN WHEN STATE CHANGES
        // UpdateInstanceBuffers();
        
        glUseProgram(shaderProgram);
        glBindVertexArray(vao);

        glUniform3fv(uGlobalColorLocation, 1, glm::value_ptr(globalState->color));
        glUniform1f(uGlobalScaleLocation, globalState->scale);

        glm::mat4 viewProjection = projection * view;
        glUniformMatrix4fv(uViewProjectionLocation, 1, GL_FALSE, glm::value_ptr(viewProjection));

        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, (GLsizei)cubes.size());

        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Shutdown(Application::AppContext* appContext) {
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

}
