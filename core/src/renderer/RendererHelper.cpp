#include <fstream>
#include <sstream>
#include <string>

#include <glad/glad.h>
#include <SDL3/SDL.h>

#include <RendererHelper.hpp>

namespace Renderer {

    std::string LoadTextFile(const char* filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load file: %s", filepath);
            return "";
        }
        std::stringstream stream;
        stream << file.rdbuf();
        return stream.str();
    }

    GLuint CreateShader(const std::string& source, GLenum type) {
        GLuint shader = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            const char* shaderType = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s SHADER COMPILE ERROR: %s", shaderType, infoLog);
        }

        return shader;
    }

    GLuint CreateShaderProgram(GLuint vertexShader, GLuint fragmentShader) {
        GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        GLint success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(program, 512, nullptr, infoLog);
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SHADER PROGRAM LINK ERROR: %s", infoLog);
        }

        // DELETE SHADER AFTER LINKING
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return program;
    }

}
