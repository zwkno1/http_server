#pragma once
#include "tgbot/TgTypeParser.h"
#include "detail/log.h"

class TgParser
{
public:
    std::vector<TgBot::Update::Ptr> parseUpdate(const std::string & updateString)
    {
        boost::property_tree::ptree pt = parseString(updateString);
        return parser_.parseJsonAndGetArray<TgBot::Update>(&TgBot::TgTypeParser::parseJsonAndGetUpdate, pt);
    }

private:
    boost::property_tree::ptree parseString(const std::string & str)
    {
        boost::property_tree::ptree result = parser_.parseJson(str);
        try
        {
            if (result.get<bool>("ok", false))
            {
                return result.get_child("result");
            }
            else
            {
                Log::debug() << "parseString error: " << result.get("description", "");
            }
        }
        catch (boost::property_tree::ptree_error& e)
        {
           Log::debug() << "parseString error: " << e.what();
        }

        return boost::property_tree::ptree{};
    }

    TgBot::TgTypeParser parser_;
};
