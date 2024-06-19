#ifndef __COMM_UTILS_HPP__
#define __COMM_UTILS_HPP__

#include "TcpCapture.hpp"
#include <string>

namespace CommUtils
{

static inline std::string buildID(const std::string& ip, int port) {
    return ip + ":" + std::to_string(port);
}

static inline std::string buildID(const TcpMessage& tm) {
    return buildID(tm.src_ip, tm.src_port);
}


} // namespace CommUtils


#endif
