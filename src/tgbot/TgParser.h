#pragma once
#include "tgbot/TgTypeParser.h"
#include "detail/log.h"

class TgParser
{
public:
    std::vector<TgBot::Update::Ptr> parseUpdate(const std::string & updateString, bool longPolling = false)
    {
        boost::property_tree::ptree pt = parseString(updateString);
        if(longPolling)
        {
            if (pt.get<bool>("ok", false))
            {
                pt = pt.get_child("result");
            }
            else
            {
                Log::debug() << "parseUpdate error, long pooling not ok";
                return std::vector<TgBot::Update::Ptr>{};
            }
        }

        return parser_.parseJsonAndGetArray<TgBot::Update>(&TgBot::TgTypeParser::parseJsonAndGetUpdate, pt);
    }

private:
    boost::property_tree::ptree parseString(const std::string & str)
    {
        try
        {
            boost::property_tree::ptree result = parser_.parseJson(str);
            return result;
        }
        catch (boost::property_tree::ptree_error& e)
        {
           Log::debug() << "parseString error: " << e.what();
        }

        return boost::property_tree::ptree{};
    }

    TgBot::TgTypeParser parser_;
};
