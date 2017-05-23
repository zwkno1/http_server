#pragma once

#include <string>
#include <map>
#include <sstream>

class server_config
{
    server_config()
    {
    }

public:
    static server_config & instance()
    {
        static server_config config;
        return config;
    }

    static void set(const std::string & key, const std::string & value)
    {
        instance().properties_[key] = value;
    }

    template<typename T>
    static T get(const std::string & key, const T & default_value = T())
    {
        auto iter = instance().properties_.find(key);
        if(iter != instance().properties_.end())
        {
            return get_value<T>(iter->second, default_value);
        }
        return default_value;
    }

    template<typename T>
    static T get_value(const std::string & str, const T & default_value)
    {
        T result = default_value;
        std::stringstream ss;
        ss << str;
        ss >> result;

        if(ss.eof())
        {
            return result;
        }
        else
        {
            return default_value;
        }
    }

private:
    typedef std::map<std::string, std::string> PropertyMap;

    PropertyMap properties_;
};

