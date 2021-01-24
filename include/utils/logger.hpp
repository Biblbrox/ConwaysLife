#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <GL/glew.h>

namespace utils::log
{
    enum class Category
    {
        INTERNAL_ERROR,
        FILE_ERROR,
        UNEXPECTED_ERROR,
        INFO,
        INITIALIZATION_ERROR,
        SHADER_COMPILE_ERROR
    };

    constexpr const char* shader_log_file_name()
    {
        return "shader_log.log";
    }

    constexpr const char* program_log_file_name()
    {
        return "moonlander_log.log";
    }

    /**
         * Writes shader log to shader log file and standard output
         * @param shader
         */
    void printShaderLog(GLuint shader);

    /**
     * Writes program log to shader log file and standard output
     * @param program
     */
    void printProgramLog(GLuint program);

    class Logger
    {
    public:
        static void write(const std::string &file_name, Category category,
                          const std::string &msg);
    };
}

#endif //LOGGER_HPP
