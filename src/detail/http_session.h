#pragma once

#include "detail/http_parser.h"

namespace detail
{

template<typename Listener, typename AsyncStreamSocket>
class http_session : public AsyncStreamSocket
{
    typedef Listener listener_type;

public:
    typedef std::function<bool(http_response & response)> Responseable;

    template<typename ...Args>
    http_session(Listener & listener, Args && ... args)
        : AsyncStreamSocket(std::forward<Args>(args)...)
        , listener_(listener)
    {
        DEBUG();
        http_parser_.set_request_handler([this](http_request & request)
        {
            handle_request(request);
        });
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
        listener_.handle_request(request,
            [self, this](http_response & response)
        {
            return this->response(response);
        });
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

    bool response(http_response & response)
    {
        return AsyncStreamSocket::send(to_string(response));
    }

private:
    listener_type & listener_;

    http_parser http_parser_;
};

} // namespace detail
