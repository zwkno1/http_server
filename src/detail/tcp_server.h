#pragma once

#include "config.h"

namespace detail
{

template<typename Listener>
class tcp_server
{
public:
    tcp_server(asio::io_service & service, const tcp::endpoint & endpoint, Listener & listener)
        : service_(service)
        , acceptor_(service, endpoint)
        , socket_(service)
        , listener_(listener)
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(socket_, [this](const error_code & err)
        {
            if(!err)
            {
                listener_.handle_connect(socket_);
            }
            else
            {
                DEBUG() << "accept error: " << err.message();
            }
            do_accept();
        });
    }

    asio::io_service & service_;
    tcp::acceptor acceptor_;
    tcp::socket socket_;

    Listener & listener_;
};

} // namespace detail
