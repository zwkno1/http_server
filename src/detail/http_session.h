#pragma once

#include "detail/http_parser.h"

namespace detail
{

template<typename Listener, typename AsyncStreamSocket>
class http_session : public AsyncStreamSocket
{
    typedef Listener listener_type;

    typedef detail::http_parser<http_session<Listener, AsyncStreamSocket> > http_parser_type;

public:
    template<typename ...Args>
    http_session(Listener & listener, Args && ... args)
        : AsyncStreamSocket(std::forward<Args>(args)...)
        , listener_(listener)
        , http_parser_(*this)
    {
        DEBUG();
    }

    void start()
    {
        AsyncStreamSocket::start();
    }

    ~http_session()
    {
        DEBUG();
    }

    void handle_request(http_request & request)
    {
        auto self = this->shared_from_this();
        listener_.handle_request(self, request);
    }

    void handle_receive(const char * data, size_t size) final
    {
        DEBUG();
        size_t parsed = http_parser_.parse(data, size);
        if(parsed != size)
        {
            DEBUG() << "http parse error";
            this->stop();
        }
    }

    bool response(const http_response & response)
    {
        std::string result;
        result += "HTTP/"+ response.version + " " + getStatusString(response.status_code) + "\r\n";
        for(const auto & i : response.header)
        {
            result += i.first + ":" + i.second + "\r\n";
        }
        result.append(response.body.data(), response.body.size());
        return AsyncStreamSocket::send(result.data(), result.size());
    }

    const char * getStatusString(int code)
    {
        switch(code)
        {
        case 100:	return "Continue";
        case 101:	return "Switching Protocols";
        case 200:	return "OK";
        case 201:	return "Created";
        case 202:	return "Accepted";
        case 203:	return "Non-Authoritative Information";
        case 204:	return "No Content";
        case 205:	return "Reset Content";
        case 206:	return "Partial Content";
        case 300:	return "Multiple Choices";
        case 301:	return "Moved Permanently";
        case 302:	return "Found";
        case 303:	return "See Other";
        case 304:	return "Not Modified";
        case 305:	return "Use Proxy";
        case 306:	return "Unused";
        case 307:	return "Temporary Redirect";
        case 400:	return "Bad Request";
        case 401:	return "Unauthorized";
        case 402:	return "Payment Required";
        case 403:	return "Forbidden";
        case 404:	return "Not Found";
        case 405:	return "Method Not Allowed";
        case 406:	return "Not Acceptable";
        case 407:	return "Proxy Authentication Required";
        case 408:	return "Request Time-out";
        case 409:	return "Conflict";
        case 410:	return "Gone";
        case 411:	return "Length Required";
        case 412:	return "Precondition Failed";
        case 413:	return "Request Entity Too Large";
        case 414:	return "Request-URI Too Large";
        case 415:	return "Unsupported Media Type";
        case 416:	return "Requested range not satisfiable";
        case 417:	return "Expectation Failed";
        case 500:	return "Internal Server Error";
        case 501:	return "Not Implemented";
        case 502:	return "Bad Gateway";
        case 503:	return "Service Unavailable";
        case 504:	return "Gateway Time-out";
        case 505:	return "HTTP Version not supported";
        }
    }

private:
    listener_type & listener_;

    http_parser_type http_parser_;
};

} // namespace detail
