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
    GLuint shaderProgram = 0;

    glm::mat4 view;
    glm::mat4 projection;

    GLint uMVPLocation = -1;
    GLint cubeColorLocation = -1;
    GLint globalColorLocation = -1;

    void Init(Application::AppContext* appContext) {
        std::string vertexSource   = RendererHelper::LoadTextFile("../assets/shaders/cube/cube.vert");
        std::string fragmentSource = RendererHelper::LoadTextFile("../assets/shaders/cube/cube.frag");

        GLuint vertexShader   = RendererHelper::CreateShader(vertexSource, GL_VERTEX_SHADER);
        GLuint fragmentShader = RendererHelper::CreateShader(fragmentSource, GL_FRAGMENT_SHADER);

        shaderProgram = RendererHelper::CreateShaderProgram(vertexShader, fragmentShader);

        uMVPLocation = glGetUniformLocation(shaderProgram, "uMVP");
        cubeColorLocation = glGetUniformLocation(shaderProgram, "cubeColor");
        globalColorLocation = glGetUniformLocation(shaderProgram, "globalColor");

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindVertexArray(0);

        glEnable(GL_DEPTH_TEST);
    }

    void InitCamera(Application::AppContext* appContext) {
        // COMPUTE BOUNDING BOX OF ALL CUBES
        if (cubes.empty()) {
            view = glm::lookAt(glm::vec3(0,0,5), glm::vec3(0,0,0), glm::vec3(0,1,0));
            projection = glm::perspective(glm::radians(45.0f), float(appContext->width) / appContext->height, 0.1f, 100.0f);
            return;
        }

        glm::vec3 min(FLT_MAX), max(-FLT_MAX);
        for (const Cube& cube : cubes) {
            glm::vec3 p = cube.position;
            float s = cube.scale * appContext->globalScale;
            min = glm::min(min, p - glm::vec3(s));
            max = glm::max(max, p + glm::vec3(s));
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

    void Render(Application::AppContext* appContext) {
        glViewport(0, 0, appContext->width, appContext->height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(vao);

        glUniform3fv(globalColorLocation, 1, glm::value_ptr(appContext->globalColor));

        for (const Cube& cube : cubes) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), cube.position);
            model = glm::scale(model, glm::vec3(cube.scale * appContext->globalScale));
            glm::mat4 mvp = projection * view * model;

            glUniformMatrix4fv(uMVPLocation, 1, GL_FALSE, glm::value_ptr(mvp));
            glUniform3fv(cubeColorLocation, 1, glm::value_ptr(cube.color));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Shutdown(Application::AppContext* appContext) {
        glDeleteBuffers(1, &vbo);
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
