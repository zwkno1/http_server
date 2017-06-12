#pragma once

#include <string>
#include <boost/regex.hpp>
#include "detail/config.h"

namespace detail
{

class uri
{
public:
    /**
     * Parse a Uri from a string.  Throws std::invalid_argument on parse error.
     */
    explicit uri(const std::string & str)
        : has_authority_(false), port_(0)
    {
        static const boost::regex uri_regex(
                    "([a-zA-Z][a-zA-Z0-9+.-]*):"  // scheme:
                    "([^?#]*)"                    // authority and path
                    "(?:\\?([^#]*))?"             // ?query
                    "(?:#(.*))?");                // #fragment
        static const boost::regex authority_and_path_regex("//([^/]*)(/.*)?");

        boost::smatch match;
        if (!boost::regex_match(str, match, uri_regex))
        {
            throw std::invalid_argument("invalid URI ");
        }

        scheme_ = submatch(match, 1);
        //toLower(scheme_);
        std::transform(scheme_.begin(), scheme_.end(), scheme_.begin(),[](int c)
        {
            return tolower(c);
        });

        std::string authority_and_path(submatch(match, 2));
        boost::smatch authority_and_path_match;
        if (!boost::regex_match(authority_and_path,
                                authority_and_path_match, authority_and_path_regex))
        {
            // Does not start with //, doesn't have authority
            has_authority_ = false;
            path_ = authority_and_path;
        }
        else
        {
            static const boost::regex authority_regex(
                        "(?:([^@:]*)(?::([^@]*))?@)?"  // username, password
                        "(\\[[^\\]]*\\]|[^\\[:]*)"     // host (IP-literal (e.g. '['+IPv6+']',
                        // dotted-IPv4, or named host)
                        "(?::(\\d*))?");               // port

            auto authority = authority_and_path_match[1];
            boost::smatch authority_match;
            if (!boost::regex_match(authority.first,
                                    authority.second,
                                    authority_match,
                                    authority_regex))
            {
                throw std::invalid_argument("invalid URI authority");
            }

            std::string port(submatch(authority_match, 4));
            if (!port.empty())
            {
                port_ = std::stoul(port);
            }

            has_authority_ = true;
            username_ = submatch(authority_match, 1);
            password_ = submatch(authority_match, 2);
            host_ = submatch(authority_match, 3);
            path_ = submatch(authority_and_path_match, 2);
        }

        query_ = submatch(match, 3);
        fragment_ = submatch(match, 4);
    }


    const std::string& scheme() const { return scheme_; }
    const std::string& username() const { return username_; }
    const std::string& password() const { return password_; }
    /**
     * Get host part of URI. If host is an IPv6 address, square brackets will be
     * returned, for example: "[::1]".
     */
    const std::string& host() const { return host_; }
    /**
     * Get host part of URI. If host is an IPv6 address, square brackets will not
     * be returned, for exmaple "::1"; otherwise it returns the same thing as
     * host().
     *
     * hostname() is what one needs to call if passing the host to any other tool
     * or API that connects to that host/port; e.g. getaddrinfo() only understands
     * IPv6 host without square brackets
     */
    std::string hostname() const
    {
        if (host_.size() > 0 && host_[0] == '[')
        {
            // If it starts with '[', then it should end with ']', this is ensured by
            // regex
            return host_.substr(1, host_.size() - 2);
        }
        return host_;
    }

    uint16_t port() const { return port_; }
    const std::string& path() const { return path_; }
    const std::string& query() const { return query_; }
    const std::string& fragment() const { return fragment_; }

    std::string authority() const;

    std::string str() const
    {
        std::string result;
        result.reserve(1024);
        if (has_authority_)
        {
            result += scheme_ + "://";
            if (!password_.empty())
            {
                result += username_ + ":" + password_ + "@";
            }
            else if (!username_.empty())
            {
                result += username_ + "@";
            }
            result += host_;
            if (port_ != 0)
            {
                result += ":" + port_;
            }
        }
        else
        {
            result += scheme_ + ":";
        }
        result += path_;
        if (!query_.empty())
        {
            result += "?" + query_;
        }

        if (!fragment_.empty())
        {
            result += "#" + fragment_;
        }
        return result;
    }

    void set_port(uint16_t port)
    {
        has_authority_ = true;
        port_ = port;
    }

    /**
     * Get query parameters as key-value pairs.
     * e.g. for URI containing query string:  key1=foo&key2=&key3&=bar&=bar=
     * In returned list, there are 3 entries:
     *     "key1" => "foo"
     *     "key2" => ""
     *     "key3" => ""
     * Parts "=bar" and "=bar=" are ignored, as they are not valid query
     * parameters. "=bar" is missing parameter name, while "=bar=" has more than
     * one equal signs, we don't know which one is the delimiter for key and
     * value.
     *
     * Note, this method is not thread safe, it might update internal state, but
     * only the first call to this method update the state. After the first call
     * is finished, subsequent calls to this method are thread safe.
     *
     * @return  query parameter key-value pairs in a vector, each element is a
     *          pair of which the first element is parameter name and the second
     *          one is parameter value
     */
    const std::vector<std::pair<std::string, std::string>>& get_query_params()
    {
        if (!query_.empty() && query_params_.empty())
        {
            // Parse query string
            static const boost::regex query_param_regex(
                        "(^|&)" /*start of query or start of parameter "&"*/
                        "([^=&]*)=?" /*parameter name and "=" if value is expected*/
                        "([^=&]*)" /*parameter value*/
                        "(?=(&|$))" /*forward reference, next should be end of query or start of next parameter*/);
            boost::sregex_iterator iter(query_.begin(), query_.end(), query_param_regex);
            for (; iter != boost::sregex_iterator(); ++iter)
            {
                if (iter->length(2) == 0)
                {
                    // key is empty, ignore it
                    continue;
                }

                // parameter name , parameter value
                query_params_.emplace_back(submatch(*iter, 2), submatch(*iter, 3));
            }
        }
        return query_params_;
    }

private:
    template<typename Match>
    std::string submatch(const Match & m, int idx) const
    {
        auto& sub = m[idx];
        return std::string(sub.first, sub.second);
    }

    std::string scheme_;
    std::string username_;
    std::string password_;
    std::string host_;
    bool has_authority_;
    uint16_t port_;
    std::string path_;
    std::string query_;
    std::string fragment_;
    std::vector<std::pair<std::string, std::string>> query_params_;
};

}  // namespace detail
