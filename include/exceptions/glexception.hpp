#ifndef GLEXCEPTION_HPP
#define GLEXCEPTION_HPP

#include <string>

#include "basegameexception.hpp"
#include "utils/logger.hpp"

using utils::log::Category;

/**
 * Exception class that can be throw when happened some error
 * with gl functions.
 */
class GLException: public BaseGameException
{
public:
    explicit GLException(std::string msg,
                         std::string fileLog = "moonlander_log.log",
                         Category category = Category::INTERNAL_ERROR)
            : BaseGameException(std::move(msg), std::move(fileLog), category) {}
};


#endif //MOONLANDER_GLEXCEPTION_HPP
