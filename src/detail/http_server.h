#pragma once

#include "detail/tcp_server.h"
#include "detail/http_session.h"
#include "detail/async_tcp_socket.h"

template<typename Listener>
class http_server
{
public:
    typedef detail::http_session<http_server<Listener>, detail::async_tcp_socket> http_session;

    http_server(Listener & listener, asio::io_service & service, const tcp::endpoint & endpoint)
        : impl_(service, endpoint, *this)
        , listener_(listener)
    {
    }

    void handle_connect(tcp::socket & socket)
    {
        make_shared<http_session>(*this, socket)->start();
    }

    template<typename Responesable>
    void handle_request(http_request & request, Responesable responseable)
    {
        listener_.handle_request(request, responseable);
    }

private:
    detail::tcp_server<http_server> impl_;

    Listener & listener_;
};
