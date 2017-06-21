#pragma once

#include "detail/config.h"

#include "detail/impl/http_parser.h"

struct http_request
{
    int version_major;
    int version_minor;
    std::string method;
    std::string url;
    std::map<std::string, std::string> header;
    std::string body;
};

struct http_response
{
    int version_major;
    int version_minor;
    int status_code;
    std::map<std::string, std::string> header;
    std::string body;
};

const char * http_status_str(int code)
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

std::string to_string(const http_response & response)
{
    std::stringstream ss;
    ss << "HTTP/" << response.version_major << "." << response.version_minor << " " << response.status_code << " " << http_status_str(response.status_code) << "\r\n";
    for(const auto & i : response.header)
    {
        ss << i.first << ":"  << i.second << "\r\n";
    }
    ss << "\r\n";
    std::string result = ss.str();
    result.append(response.body.data(), response.body.size());
    return result;
}

namespace detail
{

class http_parser
{
public:
    http_parser()
    {
        init_parse();
    }

    template<typename Handler>
    void set_request_handler(Handler && handler)
    {
        request_handler_ = move(handler);
    }

    template<typename Handler>
    void set_response_handler(Handler && handler)
    {
        response_handler_ = move(handler);
    }

    size_t parse(const char * data, size_t size)
    {
        size_t parsed = http_parser_execute(&parser_, &settings_, data, size);
        return parsed;
    }

    void handle_message_complete()
    {
        if(parser_.type == HTTP_REQUEST)
        {
            if(request_handler_)
            {
                http_request request;
                request.version_major = parser_.http_major;
                request.version_minor = parser_.http_minor;
                request.method = http_method_str(http_method(parser_.method));
                request.url = std::move(url_);
                request.header = std::move(header_);
                request.body = std::move(body_);
                request_handler_(request);
            }
        }
        else
        {
            if(response_handler_)
            {
                http_response response;
                response.version_major = parser_.http_major;
                response.version_minor = parser_.http_minor;
                response.header = std::move(header_);
                response.body = std::move(body_);
                response.status_code = parser_.status_code;
                response_handler_(response);
            }
        }
        clear();

    }

    int append_url(const char * buf, size_t len)
    {
        url_.append(buf, len);
        return 0;
    }

    int append_body(const char * buf, size_t len)
    {
        body_.append(buf, len);
        return 0;
    }

    int append_header_field(const char * buf, size_t len)
    {
        check_append_header();
        tmp_filed_.append(buf, len);
        return 0;
    }

    int append_header_value(const char * buf, size_t len)
    {
        should_append_header_ = true;
        tmp_value_.append(buf, len);
        return 0;
    }

    int headers_complete()
    {
        check_append_header();
    }

    static int url_cb(::http_parser *p, const char *buf, size_t len)
    {
        http_parser * session = static_cast<http_parser *>(p->data);
        session->append_url(buf, len);
        return 0;
    }

    static int body_cb(::http_parser *p, const char *buf, size_t len)
    {
        http_parser * session = static_cast<http_parser *>(p->data);
        session->append_body(buf, len);
        return 0;
    }

    static int header_field_cb(::http_parser *p, const char *buf, size_t len)
    {
        http_parser * session = static_cast<http_parser *>(p->data);
        session->append_header_field(buf, len);
        return 0;
    }

    static int header_value_cb(::http_parser *p, const char *buf, size_t len)
    {
        http_parser * session = static_cast<http_parser *>(p->data);
        session->append_header_value(buf, len);
        return 0;
    }

    static int headers_complete(::http_parser *p)
    {
        http_parser * session = static_cast<http_parser *>(p->data);
        session->headers_complete();
        return 0;
    }

    static int message_complete_cb(::http_parser *p)
    {
        http_parser * session = static_cast<http_parser *>(p->data);
        session->handle_message_complete();
        return 0;
    }

private:
    void init_parse()
    {
        std::memset(&settings_, 0, sizeof(settings_));
        settings_.on_url = &http_parser::url_cb;
        settings_.on_body = &http_parser::body_cb;
        settings_.on_header_field = &http_parser::header_field_cb;
        settings_.on_header_value = &http_parser::header_value_cb;
        settings_.on_message_complete = &http_parser::message_complete_cb;
        parser_.data = this;
        http_parser_init(&parser_, HTTP_BOTH);
        clear();
    }

    void check_append_header()
    {
        if(should_append_header_)
        {
            header_.insert(std::make_pair<string, string>(move(tmp_filed_), move(tmp_value_)));
            should_append_header_ = false;
        }
    }

    void clear()
    {
        should_append_header_ = false;
        method_.clear();
        url_.clear();
        header_.clear();
        body_.clear();
        tmp_filed_.clear();
        tmp_value_.clear();
    }

    http_parser_settings settings_;

    ::http_parser parser_;

    std::string tmp_filed_;
    std::string tmp_value_;

    std::string method_;
    std::map<std::string, std::string> header_;
    std::string url_;
    std::string body_;

    bool should_append_header_;

    std::function<void(http_request &)> request_handler_;
    std::function<void(http_response &)> response_handler_;
};

} // namespace detail
