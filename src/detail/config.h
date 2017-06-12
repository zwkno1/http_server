#pragma once

#include <memory>
#include <optional>
#include <boost/filesystem.hpp>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "detail/log.h"

using boost::system::error_code;
using std::enable_shared_from_this;
using std::make_shared;
using std::array;
using std::move;
using std::shared_ptr;
using std::optional;
using std::string;

namespace asio = boost::asio;
namespace  filesystem = boost::filesystem;
namespace ip = asio::ip;
namespace ssl = asio::ssl;
using asio::ip::tcp;
