#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <map>
#include <any>

class Config
{
public:
    template<class T>
    static void addVal(const std::string& key, T val)
    {
        auto const res = m_values.insert({key, val});
        if (!res.second)
            res.first->second = val;
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

private:
    static std::map<std::string, std::any> m_values;
};

#endif //CONFIG_HPP
