#pragma once

#include "detail/async_stream_socket.h"

namespace detail
{

class async_tcp_socket : public async_stream_socket<async_tcp_socket>
{
public:
    typedef tcp::socket socket_type;

    async_tcp_socket(socket_type & socket)
        : async_stream_socket(*this)
        , socket_(move(socket))
    {
        DEBUG();
    }

    ~async_tcp_socket()
    {
        DEBUG();
    }

    socket_type & socket()
    {
        return socket_;
    }

    void start()
    {
        do_receive();
    }

    void close()
    {
        socket_.close();
    }

private:
    socket_type socket_;
};

} // namespace detail
