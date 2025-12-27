#pragma once

#include <fstream>
#include <sstream>
#include <string>

#include <glad/glad.h>
#include <SDL3/SDL.h>

namespace Renderer {

    std::string LoadTextFile(const std::string& filepath);

    GLuint CreateShader(const std::string& source, GLenum type);

    bool ValidateShaderProgram(GLuint shaderProgram);

    GLuint CreateShaderProgramFromFiles(const std::string& vertexPath, const std::string& fragmentPath);

    GLuint CreateComputeShaderProgram(const std::string& computeShaderPath);

}