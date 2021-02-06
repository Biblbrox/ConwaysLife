#ifndef STRING_HPP
#define STRING_HPP

#include <vector>
#include <string>

namespace utils::string
{
    std::vector<std::string>
    split(const std::string& str, const std::string& del);
}

#endif //STRING_HPP
