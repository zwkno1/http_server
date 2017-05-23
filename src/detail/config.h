#pragma once

//#include <boost/filesystem.hpp>
#include <memory>
#include <optional>
#include <experimental/filesystem>

#include <asio.hpp>
#include <asio/ssl.hpp>

#include "detail/log.h"

using std::error_code;
using std::enable_shared_from_this;
using std::make_shared;
using std::array;
using std::move;
using std::shared_ptr;
using std::optional;
using std::string;

//namespace asio = boost::asio;
//namespace  filesystem = boost::filesystem;
namespace  filesystem = std::experimental::filesystem;
namespace ip = asio::ip;
namespace ssl = asio::ssl;
using asio::ip::tcp;
