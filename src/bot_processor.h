#pragma once

#include "detail/http_parser.h"
#include "tgbot/TgParser.h"

class bot_processor
{
public:
    template<typename Resposeable>
    void handle_request(http_request & request, Resposeable & responseable)
    {
        std::vector<TgBot::Update::Ptr> result =  parser_.parseUpdate(request.body);

        http_response response;

        if(!result.empty())
        {
            for(TgBot::Update::Ptr & update : result)
            {
                if(update->message)
                {
                    Log::debug() << "recive bot message: " << update->message->text;
                }
            }
            response.status_code = 200;
        }
        else
        {
            response.status_code = 400;
        }

        responseable(response);
    }
private:
    TgParser parser_;
};

