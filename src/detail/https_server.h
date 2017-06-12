#pragma once

#include "detail/tcp_server.h"
#include "detail/http_session.h"
#include "detail/async_ssl_socket.h"
#include "server_config.h"

template<typename Listener>
class https_server
{
public:
    typedef detail::http_session<https_server<Listener>, detail::async_ssl_socket> https_session;

    https_server(Listener & listener, asio::io_service & service, const tcp::endpoint & endpoint)
        : impl_(service, endpoint, *this)
        , context_(ssl::context::sslv23)
        , listener_(listener)
    {
        init_context();
    }

    void handle_connect(tcp::socket & socket)
    {
        make_shared<https_session>(*this, socket, context_)->start();
    }

    template<typename Responesable>
    void handle_request(http_request & request, Responesable responseable)
    {
        listener_.handle_request(request, responseable);
    }

private:
    void init_context()
    {
        context_ = ssl::context(ssl::context::sslv23);
        context_.set_options(ssl::context::default_workarounds
                             | ssl::context::no_sslv2
                             | ssl::context::single_dh_use);

        context_.set_password_callback([this](size_t size, ssl::context_base::password_purpose purpose)
        {
            DEBUG() << "ssl password callback >> size: " << size << ", purpose: " << purpose;
            return server_config::get<std::string>("private_password");
        });
        context_.use_certificate_file(server_config::get<std::string>("public_key"), ssl::context::pem);
        context_.use_private_key_file(server_config::get<std::string>("private_key"), ssl::context::pem);
        context_.set_options(SSL_OP_NO_TICKET);
    }

    detail::tcp_server<https_server> impl_;

    ssl::context context_;

    Listener & listener_;
};
