#include <vector>
#include <string>

#include "utils/string.hpp"

std::vector<std::string>
utils::string::split(const std::string& str, const std::string& del)
{
    std::string s(str);
    size_t pos = 0;
    std::string token;
    std::vector<std::string> res;
    while (s.starts_with(del)) s.erase(0, del.length());
    while ((pos = s.find(del)) != std::string::npos) {
        res.emplace_back(s.substr(0, pos));
        s.erase(0, pos + del.length());
    }
    res.emplace_back(s);

    return res;
}