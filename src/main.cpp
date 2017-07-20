#include <iostream>
#include <chrono>

#include "server_config.h"
#include "detail/https_server.h"
#include "detail/http_server.h"
#include "http_dispatcher.h"
#include "session_factory.h"
#include "detail/uri.h"

int main(int argc, char *argv[])
{
    if(argc < 7)
    {
        std::cout << "Usage: ./http_server ./ ./certs/public.pem ./certs/private.pem 123456 80 443" << std::endl;
        return -1;
    }

    server_config::set("root", argv[1]);
    server_config::set("public_key", argv[2]);
    server_config::set("private_key", argv[3]);
    server_config::set("private_password", argv[4]);
    server_config::set("http_port", argv[5]);
    server_config::set("https_port", argv[6]);

    http_dispatcher dispatcher(server_config::get<std::string>("root", "./"));

    try
    {
        asio::io_service service(1);
        http_server<http_dispatcher> server(dispatcher, service, tcp::endpoint(ip::address_v4::any(), server_config::get("http_port", 80)));
        https_server<http_dispatcher> server2(dispatcher, service, tcp::endpoint(ip::address_v4::any(), server_config::get("https_port", 443)));
        service.run();
    }
    catch(const error_code& err)
    {
        std::cout << err.message() << std::endl;
    }
}
