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
        if(std::equal(bot_path.begin(), bot_path.end(), request.path.begin()))
        {
            bot_processor_.handle_request(request, responseable);
        }
        else
        {
            default_processor_.handle_request(request, responseable);
        }
    }

private:
    const std::string bot_path = "/telegram_bot/";

    bot_processor bot_processor_;
    static_webpage_processor default_processor_;
};

