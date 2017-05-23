#pragma once

#include "detail/config.h"

#include "detail/impl/http_parser.h"

struct http_request
{
    std::string method;
    std::string url;
    //std::string head;
    std::map<std::string, std::string> header;
    std::string body;
};

struct http_response
{
    std::string version;
    int status_code;
    std::map<std::string, std::string> header;
    std::string body;
};

namespace detail
{

template<typename Listener>
class http_parser
{
public:
    http_parser(Listener & listener)
        : listener_(listener)
    {
        init_parse();
    }

    size_t parse(const char * data, size_t size)
    {
        size_t parsed = http_parser_execute(&parser_, &settings_, data, size);
        return parsed;
    }

    void handle_request()
    {
        request_.method = http_method_str(http_method(parser_.method));
        listener_.handle_request(request_);
        clear_request();
    }

    int append_url(const char * buf, size_t len)
    {
        request_.url.append(buf, len);
        return 0;
    }

    int append_body(const char * buf, size_t len)
    {
        request_.body.append(buf, len);
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
        session->handle_request();
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
        http_parser_init(&parser_, HTTP_REQUEST);
        should_append_header_ = false;
    }

    void check_append_header()
    {
        if(should_append_header_)
        {
            request_.header.insert(std::make_pair<string, string>(move(tmp_filed_), move(tmp_value_)));
            should_append_header_ = false;
        }
    }

    void clear_request()
    {
        request_.method.clear();
        request_.url.clear();
        request_.header.clear();
        request_.body.clear();
    }

    Listener & listener_;

    http_parser_settings settings_;

    ::http_parser parser_;

    http_request request_;

    std::string tmp_filed_;
    std::string tmp_value_;

    bool should_append_header_;
};

} // namespace detail
