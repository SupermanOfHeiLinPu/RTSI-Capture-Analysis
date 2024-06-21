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

/**
 * @brief Split the string by the given delimiter.
 *
 * @param input
 * @param delimiter
 * @return std::vector<std::string> string list
 */
static std::vector<std::string> splitString(const std::string& input, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = input.find(delimiter);

    while (end != std::string::npos) {
        tokens.push_back(input.substr(start, end - start));
        start = end + delimiter.length();
        end = input.find(delimiter, start);
    }

    tokens.push_back(input.substr(start, std::string::npos));

    return tokens;
}

} // namespace CommUtils


#endif
