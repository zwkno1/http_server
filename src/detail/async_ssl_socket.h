#pragma once

#include "detail/async_stream_socket.h"

namespace detail
{

class async_ssl_socket : public async_stream_socket<async_ssl_socket>
{
public:
    typedef ssl::stream<tcp::socket> socket_type;

    async_ssl_socket(tcp::socket & socket, ssl::context & context)
        : async_stream_socket(*this)
        , socket_(socket.get_io_service(), context)
    {
        socket_.set_verify_mode(ssl::verify_peer);
        socket_.lowest_layer() = move(socket);
        DEBUG();
    }

    socket_type & socket()
    {
        return socket_;
    }

    ~async_ssl_socket()
    {
        DEBUG();
    }

    void start()
    {
        do_handshake();
    }

    void close()
    {
        socket_.lowest_layer().close();
    }

    void do_handshake()
    {
        auto self = shared_from_this();

        socket_.async_handshake(ssl::stream_base::server, [this, self](const error_code & err)
        {
            if(err)
            {
                DEBUG() << "async handshake error: " << err.message();
                stop();
                return;
            }
            do_receive();
        });
    }
private:
    socket_type socket_;
};

} // namespace detail
