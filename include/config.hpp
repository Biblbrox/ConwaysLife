#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <map>
#include <any>
#include <fstream>
#include <iostream>
#include <glm/vec4.hpp>

#include "utils/string.hpp"

static std::string as_string(const std::any& val, const std::string& type)
{
    if (type == "int") {
        return std::to_string(std::any_cast<int>(val));
    } else if (type == "float") {
        return std::to_string(std::any_cast<float>(val));
    } else if (type == "double") {
        return std::to_string(std::any_cast<double>(val));
    } else if (type == "char") {
        return std::to_string(std::any_cast<char>(val));
    } else if (type == "bool") {
        return std::to_string(std::any_cast<bool>(val));
    } else if (type == "long") {
        return std::to_string(std::any_cast<long>(val));
    } else if (type == "string") {
        return std::any_cast<std::string>(val);
    } else if (type == "const char*") {
        return std::any_cast<const char*>(val);
    } else if (type == "long long") {
        return std::to_string(std::any_cast<long long>(val));
    } else if (type == "vec4") {
        auto value = std::any_cast<glm::vec4>(val);
        return std::to_string(value.x) + "," + std::to_string(value.y)
               + "," + std::to_string(value.z) + "," + std::to_string(value.w);
    }

    return "";
}

class Config
{
public:
    template<class T>
    static void addVal(const std::string& key, T val, const std::string& type)
    {
        auto const res = m_values.insert({key, val});
        auto const type_res = m_types.insert({key, type});
        if (!res.second) {
            res.first->second = val;
            type_res.first->second = type;
        }
    }

    static bool hasKey(const std::string& key)
    {
        return m_values.contains(key);
    }

    template<class T>
    static T& getVal(const std::string& key)
    {
        return std::any_cast<T&>(m_values[key]);
    }

    static void save(const std::string& file)
    {
        std::ofstream out(file);
        for (const auto& [key, val]: m_values) {
            out << key << ":" << m_types[key] <<
                ":" << as_string(val, m_types[key]) << "\n";
        }
        out.close();
    }

    static void load(const std::string& file)
    {
        using utils::string::split;
        std::ifstream in(file);
        std::string line;
        while ((std::getline(in, line))) {
            auto parts = split(line, ":");
            std::string key = parts[0];
            std::string type = parts[1];
            if (type == "int") {
                int val = std::stoi(parts[2]);
                addVal(key, val, "int");
            } else if (type == "float") {
                float val = std::stof(parts[2]);
                addVal(key, val, "float");
            } else if (type == "double") {
                double val = std::stod(parts[2]);
                addVal(key, val, "double");
            } else if (type == "char") {
                char val = std::stoi(parts[2]);
                addVal(key, val, "char");
            } else if (type == "bool") {
                bool val = std::stoi(parts[2]) == 1;
                addVal(key, val, "bool");
            } else if (type == "long") {
                long val = std::stol(parts[2]);
                addVal(key, val, "long");
            } else if (type == "long long") {
                long long val = std::stoll(parts[2]);
                addVal(key, val, "long long");
            } else if (type == "string") {
                addVal(key, parts[2], "string");
            } else if (type == "const char*") {
                addVal(key, parts[2], "const char*");
            } else if (type == "vec4") {
                auto vec = split(parts[2], ",");
                glm::vec4 val = {
                        std::stof(vec[0]), std::stof(vec[1]),
                        std::stof(vec[2]), std::stof(vec[3])
                };
                addVal(key, val, "vec4");
            }
        }
    }

private:
    static std::map<std::string, std::any> m_values;
    static std::map<std::string, std::string> m_types;
};

#endif //CONFIG_HPP
