#pragma once

#include "detail/config.h"
#include "detail/http_parser.h"
#include "content_type.h"
#include "server_config.h"
#include <map>
#include <string>
#include <cstring>
#include <fstream>

class content_service
{
public:
    content_service(const std::string & root)
        : root_(root)
        , content_type_(content_type_map)
    {
    }

    optional<std::pair<string, string> > get(const string & url)
    {
        optional<std::pair<string, string>> result;

        filesystem::path path(root_);
        path /= url;

        if(filesystem::is_directory(path))
        {
            path /= "index.html";
        }
        else if(!filesystem::exists(path))
        {
             path += ".html";
        }

        if(filesystem::is_regular_file(path))
        {
            auto iter = cache_.find(path.string());
            if(iter == cache_.end())
            {
                DEBUG() << "cache miss: " << url << ":" << path.string();
                std::ifstream ifs(path.string(), std::ios_base::binary);
                if(ifs)
                {
                    std::string content_value;
                    ifs.seekg(0, std::ios::end);
                    size_t size = ifs.tellg();
                    ifs.seekg(0, std::ios::beg);
                    content_value.resize(size);
                    ifs.read(&content_value[0], content_value.size());
                    string content_type = get_content_type(path.string());
                    auto content = std::make_pair<string, string>(move(content_type), move(content_value));
                    DEBUG() << "cache add: " << url << ":" << path.string() << ":" << content.first << ":" << content.second.size();
                    cache_[path.string()] = content;
                    result= move(content);
                }
            }
            else
            {
                DEBUG() << "cache hit: " << url << ":" << path.string() << ":" << iter->second.first << ":" << iter->second.second.size();
                result = iter->second;
            }
        }

        return result;
    }
private:
    std::string get_content_type(const std::string & filename)
    {
        std::string suffix = ".*";
        for(int i = filename.size(); i >= 0; --i)
        {
            if(filename[i] == '.')
            {
                 suffix = filename.substr(i);
                 break;
            }
            else if(filename[i] == '\\' || filename[i] == '/')
            {
                  break;
            }
        }
        DEBUG() << "suffix: " << filename << ":" << suffix;
        return content_type_[suffix];
    }

private:
    string root_;
    std::map<string, std::pair<string, string> > cache_;
    std::map<string, string> content_type_;
};

class static_webpage_processor
{
public:
    static_webpage_processor(const std::string & root)
        : content_service_(root)
    {
    }

    template<typename Resposeable>
    void handle_request(http_request & request, Resposeable & responseable)
    {
        DEBUG() << request.method;
        DEBUG() << request.url;
        for(auto & i : request.header)
        {
            DEBUG() << i.first << " : " << i.second;
        }
        DEBUG() << request.body;

        auto result = content_service_.get(request.url);
        if(!result)
        {
            result = std::make_pair<string, string>("text/html", "<html><head><title>Ooops</title></head><body><p>你迷路啦</p></body></html>");
        }

        http_response response;
        response.status_code = 200;
        response.version_major = 1;
        response.version_minor = 1;
        response.header["Connection"] = "Keep-Alive";
        response.header["Content-Type"] = result->first;
        response.header["Content-Length"] = std::to_string(result->second.size());
        response.body = result->second;

        responseable(response);
    }

private:
    content_service content_service_;
};

