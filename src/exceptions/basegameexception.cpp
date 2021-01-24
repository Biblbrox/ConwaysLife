#include <string>

#include "exceptions/basegameexception.hpp"

BaseGameException::BaseGameException(std::string msg, std::string fileLog,
                                     Category category)
        : m_message(std::move(msg)),
          m_fileLog(std::move(fileLog)),
          m_category(category)
{
}

const char* BaseGameException::what() const noexcept
{
    return m_message.c_str();
}

const char* BaseGameException::fileLog() const noexcept
{
    return m_fileLog.c_str();
}

Category BaseGameException::categoryError() const noexcept
{
    return m_category;
}
