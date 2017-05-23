#include <iostream>

#include "server_config.h"
#include "detail/https_server.h"
#include "detail/http_server.h"
#include "http_processor.h"

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

    http_processor processor(server_config::get<std::string>("root", "./"));

    try
    {
        asio::io_context context(1);
        http_server<http_processor> server(processor, context, tcp::endpoint(ip::address_v4::any(), server_config::get("http_port", 80)));
        https_server<http_processor> server2(processor, context, tcp::endpoint(ip::address_v4::any(), server_config::get("https_port", 443)));
        context.run();
    }
    catch(const error_code& err)
    {
        std::cout << err.message() << std::endl;
    }
}
