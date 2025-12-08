#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Cube.hpp>
#include <CubeRenderer.hpp>
#include <RendererHelper.hpp>
#include <Point.hpp>

namespace CubeRenderer {

    static float globalScale = 0.5f;
    static glm::vec3 globalColor = glm::vec3(1.0f);

    static std::vector<Data::Cube> cubes;

    static GLuint vao = 0;
    static GLuint vbo = 0;
    static GLuint instanceVBO = 0;
    static GLuint instanceColorVBO = 0;
    static GLuint shaderProgram = 0;

    static GLint uViewProjectionLocation = -1;
    static GLint uGlobalColorLocation = -1;
    static GLint uGlobalScaleLocation = -1;

    static size_t instanceCount = 0;
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

    void UpdateBufferSize(int maxInstanceCount) {
        instanceCount = 0;

        instanceModels.resize(maxInstanceCount, glm::mat4(1.0f));
        instanceColors.resize(maxInstanceCount, glm::vec3(1.0f));

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, maxInstanceCount * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, instanceColorVBO);
        glBufferData(GL_ARRAY_BUFFER, maxInstanceCount * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
    }

    void Render(glm::mat4 view, glm::mat4 projection) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (instanceCount == 0) return;

        glUseProgram(shaderProgram);
        glBindVertexArray(vao);

        glUniform3fv(uGlobalColorLocation, 1, glm::value_ptr(globalColor));
        glUniform1f(uGlobalScaleLocation, globalScale);

        glm::mat4 viewProjection = projection * view;
        glUniformMatrix4fv(uViewProjectionLocation, 1, GL_FALSE, glm::value_ptr(viewProjection));

        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, (GLsizei)instanceCount);

        glBindVertexArray(0);
        glUseProgram(0);
    }

    void AddCubes(const std::vector<Data::Point>& points) {
        size_t pointCount = points.size();
        if (pointCount == 0) return;

        // RESERVE ADDITIONAL SPACE FOR CUBES/INSTANCE DATA
        cubes.reserve(cubes.size() + pointCount);
        if (instanceModels.size() < instanceCount + pointCount) {
            instanceModels.resize(instanceCount + pointCount);
        }
        if (instanceColors.size() < instanceCount + pointCount) {
            instanceColors.resize(instanceCount + pointCount);
        }

        glm::mat4 modelMatrix(1.0f);
        for (size_t i = 0; i < pointCount; ++i) {
            const Data::Point& point = points[i];

            // PRECOMPUTE CUBE POSITION AND COLOR
            glm::vec3 position = glm::vec3(point.x, point.y, point.z);
            glm::vec3 color = Data::ColorMap(point.normalized);
            
            // ADD CUBE
            cubes.emplace_back(position, color);

            // DIRECTLY SET TRANSLATION IN THE MATRIX MODEL (EFFICIENT)
            modelMatrix[3] = glm::vec4(position, 1.0f);
            instanceModels[instanceCount] = modelMatrix;
            instanceColors[instanceCount] = color;
            instanceCount++;
        }

        // UPDATE GPU BUFFERS FOR ALL INSTANCES
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, instanceCount * sizeof(glm::mat4), instanceModels.data());

        glBindBuffer(GL_ARRAY_BUFFER, instanceColorVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, instanceCount * sizeof(glm::vec3), instanceColors.data());
    }

    void UpdateCube(size_t index, const Data::Point& point) {
        if (index >= instanceCount) return; // INDEX OUT OF BOUNDS

        glm::vec3 position = glm::vec3(point.x, point.y, point.z);
        glm::vec3 color = Data::ColorMap(point.normalized);
        cubes[index] = Data::Cube(position, color);

        // UPDATE INSTANCE ARRAYS
        instanceModels[index] = glm::translate(glm::mat4(1.0f), position);
        instanceColors[index] = color;

        // UPDATE GPU BUFFERS FOR SINGLE INSTANCE
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(glm::mat4), sizeof(glm::mat4), &instanceModels[index]);

        glBindBuffer(GL_ARRAY_BUFFER, instanceColorVBO);
        glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(glm::vec3), sizeof(glm::vec3), &instanceColors[index]);
    }

    void Clear() {
        cubes.clear();
        instanceModels.clear();
        instanceColors.clear();
        instanceCount = 0;
    }

    void Shutdown() {
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &instanceVBO);
        glDeleteBuffers(1, &instanceColorVBO);
        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(shaderProgram);
    }

    // ACCESSOR METHODS
    const std::vector<Data::Cube>& GetCubes() { return cubes; };

    float& GetGlobalScale() { return globalScale; };
    glm::vec3& GetGlobalColor() { return globalColor; };

}
