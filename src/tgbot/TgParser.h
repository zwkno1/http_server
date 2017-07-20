#pragma once
#include "tgbot/TgTypeParser.h"
#include "detail/log.h"

class TgParser
{
public:
    std::vector<TgBot::Update::Ptr> parseUpdate(const std::string & updateString, bool longPolling = false)
    {
        try
        {
            boost::property_tree::ptree pt = parseString(updateString);
            if(longPolling)
            {
                if (pt.get<bool>("ok", false))
                {
                    pt = pt.get_child("result");
                    return parser_.parseJsonAndGetArray<TgBot::Update>(&TgBot::TgTypeParser::parseJsonAndGetUpdate, pt);
                }
                else
                {
                    Log::debug() << "parseUpdate error, long pooling not ok";
                }
            }
            else
            {
                return std::vector<TgBot::Update::Ptr>{ parser_.parseJsonAndGetUpdate(pt) };
            }
        }
        catch (boost::property_tree::ptree_error& e)
        {
            Log::debug() << "parseUpdate error: " << e.what();
        }

        return std::vector<TgBot::Update::Ptr>{ };
    }

private:
    boost::property_tree::ptree parseString(const std::string & str)
    {
        boost::property_tree::ptree result = parser_.parseJson(str);
        return result;
    }

    TgBot::TgTypeParser parser_;
};
