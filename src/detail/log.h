#pragma once

#include <sstream>

class Log
{
public:
    ~Log()
    {
        std::cout << prefix_ << stream_.rdbuf() << std::endl;
    }

    static inline Log debug() { return Log("[debug] "); }
    static inline Log info()  { return Log("[info ] "); }
    static inline Log warn()  { return Log("[warn ] "); }
    static inline Log error() { return Log("[error] "); }

    template<class T>
    Log & operator <<(const T & t)
    {
        stream_ << t;
        return *this;
    }

private:
    Log(const std::string & prefix)
        : prefix_(prefix)
    {
    }

    Log(Log &&);

    std::stringstream stream_;
    std::string prefix_;
};

#define DEBUG()   Log::debug() << "[ " __FILE__ << ":" << __LINE__<< ":" << __FUNCTION__ << " ]"
#define INFO ()   Log::info()  << "[ " __FILE__ << ":" << __LINE__<< ":" << __FUNCTION__ << " ]"
#define WARN ()   Log::warn()  << "[ " __FILE__ << ":" << __LINE__<< ":" << __FUNCTION__ << " ]"
#define ERROR()   Log::error() << "[ " __FILE__ << ":" << __LINE__<< ":" << __FUNCTION__ << " ]"

