#pragma once

#include "static_webpage_processor.h"
#include "bot_processor.h"

class http_dispatcher
{
public:
    http_dispatcher(const std::string & root)
        : default_processor_(root)
    {
    }

    template<typename Resposeable>
    void handle_request(http_request & request, Resposeable & responseable)
    {
        const char bot_url[] = "/telegram_bot/";
        if(strncmp(request.url.c_str(), bot_url, sizeof(bot_url) - 1))
        {
            bot_processor_.handle_request(request, responseable);
        }
        else
        {
            default_processor_.handle_request(request, responseable);
        }
    }

private:
    bot_processor bot_processor_;
    static_webpage_processor default_processor_;
};

