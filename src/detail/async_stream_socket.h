#pragma once

#include "config.h"

namespace detail
{

/*
 * SocketSocket concept:
 *     socket_type;
 *     start_async();
 *     stop_async();
 *     socket_type & socket();
*/
template<typename StreanSocket, size_t ReadBufferSize = 64*1024, size_t WriteBufferSize = 4*1024*1024>
class async_stream_socket: public enable_shared_from_this<async_stream_socket<StreanSocket> >
{
public:
    typedef StreanSocket stream_socket_type;

    async_stream_socket(StreanSocket & owner)
        : socket_(owner)
        , stopped_(false)
    {
        DEBUG();
    }

    virtual ~async_stream_socket()
    {
        DEBUG();
    }

    bool send(const std::string & data)
    {
        if(stopped_)
            return false;

        if(write_buffer_.size() + data.size() > WriteBufferSize)
        {
            return false;
        }

        bool start_send_op = write_buffer_.empty();
        write_buffer_ += data;
        if(start_send_op)
            do_send();
        return true;
    }

    void stop()
    {
        if(!stopped_)
        {
            stopped_ = true;

            //close socket, cancel all async operations
            socket_.close();
        }
    }

protected:
    void do_receive()
    {
        if(stopped_)
            return;

        // use this-> to avoid complie err
        auto self = this->shared_from_this();
        socket_.socket().async_read_some(asio::buffer(read_buffer_), [this, self](const error_code& err, size_t size)
        {
            if(err)
            {
                DEBUG() << "async read error: " << err.message();
                stop();
                return;
            }

            handle_receive(read_buffer_.data(), size);

            do_receive();
        });
    }

    virtual void handle_receive(const char * data, size_t size) = 0;

private:
    void do_send()
    {
        if(stopped_)
            return;

        if(write_buffer_.empty())
            return;

        // use this-> to avoid complie err
        auto self = this->shared_from_this();
        socket_.socket().async_write_some(asio::buffer(write_buffer_), [this, self](const error_code & err, size_t bytes)
        {
            if(err)
            {
                DEBUG() << "async write error: " << err.message();
                stop();
                return;
            }
            write_buffer_.erase(0, bytes);
            do_send();
        });
    }

    stream_socket_type & socket_;

    array<char, ReadBufferSize> read_buffer_;

    std::string write_buffer_;

    bool stopped_;
};

} // namespace detail
