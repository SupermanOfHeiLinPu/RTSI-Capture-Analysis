#include "RtsiParser.hpp"
#include "CommUtils.hpp"
#include <sstream>


void RtsiParserProtocol::parser(const TcpMessage& tm, const std::vector<uint8_t>::const_iterator& msg) {
    std::string src_id = CommUtils::buildID(tm);
    if (src_id == host_id_) {
        s_time_.tv_sec = tm.time_sec;
        s_time_.tv_usec = tm.time_us;
        is_success_ = msg[3];
    } else {
        c_time_.tv_sec = tm.time_sec;
        c_time_.tv_usec = tm.time_us;
    }
}

std::string RtsiParserProtocol::generateLog() {
    std::stringstream result;
    result << "\tRequest protocol:\n";
    result << "\t\tLast client request time: " << c_time_.tv_sec << "." << c_time_.tv_usec << std::endl;
    result << "\t\tLast server respon time: " << s_time_.tv_sec << "." << s_time_.tv_usec << std::endl;
    result << "\t\tIs success: " << is_success_ << std::endl;
    return result.str();
}