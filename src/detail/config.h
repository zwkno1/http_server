#pragma once

#include <memory>

#ifdef USE_BOOST_FILESYSTEM
#include <boost/filesystem.hpp>
#else
#include <experimental/filesystem>
#endif

#include <boost/optional.hpp>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "detail/log.h"

using boost::system::error_code;
using boost::optional;
using std::enable_shared_from_this;
using std::make_shared;
using std::array;
using std::move;
using std::shared_ptr;
using std::string;

namespace asio = boost::asio;

#ifdef USE_BOOST_FILESYSTEM
namespace  filesystem = boost::filesystem;
#else
namespace  filesystem = std::experimental::filesystem;
#endif

namespace ip = asio::ip;
namespace ssl = asio::ssl;
using asio::ip::tcp;
