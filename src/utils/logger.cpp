#include <fstream>
#include <iostream>
#include <boost/format.hpp>

#include "utils/logger.hpp"

using utils::log::Logger;
using utils::log::program_log_file_name;
using utils::log::shader_log_file_name;
using utils::log::Category;
using boost::format;

void
utils::log::Logger::write(const std::string &file_name, Category category,
                          const std::string &msg)
{
    std::ofstream file(file_name);
    std::string category_str;
    switch (category) {
        case Category::INTERNAL_ERROR:
            category_str = "Internal error: ";
            break;
        case Category::FILE_ERROR:
            category_str = "File IO error: ";
            break;
        case Category::UNEXPECTED_ERROR:
            category_str = "Unexpected error: ";
            break;
        case Category::INFO:
            category_str = "Info: ";
            break;
        case Category::SHADER_COMPILE_ERROR:
            category_str = "Shader compile error";
            break;
        default:
            category_str = "Internal error: ";
            break;
    }
    std::string message = category_str + msg;
    file.write(message.c_str(), message.size());
    file.close();
    if (category == Category::INFO)
        std::cout << message << "\n";
    else
        std::cerr << message << "\n";
}

void utils::log::printShaderLog(GLuint shader)
{
    if (glIsShader(shader)) {
        int infoLength = 0;
        int maxLength = infoLength;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
        char *log_str = new char[maxLength];

        glGetShaderInfoLog(shader, maxLength, &infoLength, log_str);
        if (infoLength > 0)
            Logger::write(shader_log_file_name(),
                          Category::SHADER_COMPILE_ERROR,
                          (format("Shader log:\n\n%s")
                           % log_str).str());

        delete[] log_str;
    } else {
        std::cerr << (format("Name %d is not a shader\n")
                      % shader).str() << std::endl;
    }
}

void utils::log::printProgramLog(GLuint program)
{
    if (glIsProgram(program)) {
        int infoLength = 0;
        int maxLength = infoLength;

        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        char *log_str = new char[maxLength];

        glGetProgramInfoLog(program, maxLength, &infoLength, log_str);
        if (infoLength > 0)
            Logger::write(shader_log_file_name(),
                          Category::INTERNAL_ERROR,
                          (format("Shader program log:\n\n%s")
                           % log_str).str());

        delete[] log_str;
    } else {
        Logger::write(shader_log_file_name(),Category::INTERNAL_ERROR,
                      (format("Name %1% is not a program\n")
                       % program).str());
    }
}