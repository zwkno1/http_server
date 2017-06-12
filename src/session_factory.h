#include "detail/config.h"
#include "detail/async_ssl_socket.h"
#include "detail/async_tcp_socket.h"
#include "detail/http_session.h"

#if 0
typedef detail::http_session<detail::async_tcp_socket> http_session;


class bot_worker : public enable_shared_from_this<bot_worker>
{
public:
    bot_worker(asio::io_service & service, std::list<http_request> & requests, const std::string & host, const std::string & port)
        : service_(service)
        , resolver_(service)
        , query_(host, port)
        , socket_(service)
        , ssl_context_(ssl::context::sslv23)
        , stream_(service, ssl_context_)
        , requests_(requests)
        , timer_(service)
    {
        ssl_context_.load_verify_file("ca.pem");
        http_parser_.set_response_handler([this](http_response & response)
        {
            handle_response(response);
        });
    }

    void start()
    {
        auto self = shared_from_this();
        resolver_.async_resolve(query_, [this, self](error_code err, tcp::resolver::iterator iter)
        {
            if(!err)
            {
                if(iter == tcp::resolver::iterator())
                {
                    DEBUG() << "resolver error";
                    return;
                }
                socket_.async_connect(*iter, [this, self](error_code err)
                {
                    if(!err)
                    {
                        auto session = make_shared<http_session>(socket_);
                        //stream_.lowest_layer() = std::move(socket_);
                        //stream_.async_handshake(ssl::stream_base::client, [this, self](error_code err)
                        //{
                        //    if(!err)
                        //    {
                        //        do_work();
                        //    }
                        //    else
                        //    {
                        //        DEBUG() << err.message();
                        //        return;
                        //    }
                        //});
                    }
                    else
                    {
                        DEBUG() << err.message();
                        return;
                    }

                });
            }
            else
            {
                DEBUG() << err.message();
                return;
            }
        });
    }

    void do_work()
    {
        auto self = shared_from_this();
        if(requests_.empty())
        {
            timer_.expires_from_now(std::chrono::milliseconds(10));
            timer_.async_wait([this, self](error_code err)
            {
                if(!err)
                {
                    do_work();
                }
                else
                {
                    DEBUG() << err.message();
                    return;
                }
            });
        }
        else
        {
            request_ = requests_.front();
            requests_.pop_front();
            //
        }
    }

private:
    void handle_receive(const char * data, size_t size)
    {
        DEBUG();
        size_t parsed = http_parser_.parse(data, size);
        if(parsed != size)
        {
            DEBUG() << "http parse error";
        }
    }

    void handle_response(http_response & response)
    {
    }

    asio::io_service & service_;
    tcp::resolver resolver_;
    tcp::resolver::query query_;
    tcp::socket socket_;
    ssl::context ssl_context_;
    ssl::stream<tcp::socket> stream_;

    detail::http_parser http_parser_;
    http_request request_;

    std::list<http_request> & requests_;
    asio::steady_timer timer_;
};

#endif
