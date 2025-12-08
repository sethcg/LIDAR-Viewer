#pragma once

#include <fstream>
#include <sstream>
#include <string>

#include <glad/glad.h>
#include <SDL3/SDL.h>

namespace RendererHelper {

    std::string LoadTextFile(const char* filepath);

    GLuint CreateShader(const std::string& source, GLenum type);

    GLuint CreateShaderProgram(GLuint vertexShader, GLuint fragmentShader);

}