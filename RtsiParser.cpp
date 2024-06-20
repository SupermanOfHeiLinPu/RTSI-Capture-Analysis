#include "RtsiParser.hpp"
#include "CommUtils.hpp"
#include "EndianUtils.hpp"
#include <sstream>
#include <iomanip>


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
    if (!is_recv_) {
        result << "\t\tNever received" << std::endl;
        return result.str();
    }
    
    result << "\t\tLast client request time: " << c_time_.tv_sec << "." << c_time_.tv_usec << std::endl;
    result << "\t\tLast server respon time: " << s_time_.tv_sec << "." << s_time_.tv_usec << std::endl;
    result << "\t\tIs success: " << is_success_ << std::endl;
    return result.str();
}


void RtsiParserControlVersion::parser(const TcpMessage& tm, const std::vector<uint8_t>::const_iterator& msg) {
    std::string src_id = CommUtils::buildID(tm);
    if (src_id == host_id_) {
        s_time_.tv_sec = tm.time_sec;
        s_time_.tv_usec = tm.time_us;
        std::vector<uint8_t>::const_iterator move = msg + 3;
        EndianUtils::unpack(move, major_);
        move += sizeof(uint32_t);

        EndianUtils::unpack(move, minor_);
        move += sizeof(uint32_t);

        EndianUtils::unpack(move, bugfix_);
        move += sizeof(uint32_t);

        EndianUtils::unpack(move, build_);
    } else {
        c_time_.tv_sec = tm.time_sec;
        c_time_.tv_usec = tm.time_us;
    }
}

std::string RtsiParserControlVersion::generateLog() {
    std::stringstream result;
    result << "\tRequest controller version:\n";
    if (!is_recv_) {
        result << "\t\tNever received" << std::endl;
        return result.str();
    }
    result << "\t\tLast client request time: " << c_time_.tv_sec << "." << c_time_.tv_usec << std::endl;
    result << "\t\tLast server respon time: " << s_time_.tv_sec << "." << s_time_.tv_usec << std::endl;
    result << "\t\tversion: " << major_ << "." << minor_ << "." << bugfix_ << "." << build_ << std::endl;
    return result.str();
}


void RtsiParserStart::parser(const TcpMessage& tm, const std::vector<uint8_t>::const_iterator& msg) {
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

std::string RtsiParserStart::generateLog() {
    std::stringstream result;
    result << "\tRequest start:\n";
    if (!is_recv_) {
        result << "\t\tNever received" << std::endl;
        return result.str();
    }
    result << "\t\tLast client request time: " << c_time_.tv_sec << "." << c_time_.tv_usec << std::endl;
    result << "\t\tLast server respon time: " << s_time_.tv_sec << "." << s_time_.tv_usec << std::endl;
    result << "\t\tIs success: " << is_success_ << std::endl;
    return result.str();
}

void RtsiParserPause::parser(const TcpMessage& tm, const std::vector<uint8_t>::const_iterator& msg) {
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

std::string RtsiParserPause::generateLog() {
    std::stringstream result;
    result << "\tRequest pause:\n";
    if (!is_recv_) {
        result << "\t\tNever received" << std::endl;
        return result.str();
    }
    result << "\t\tLast client request time: " << c_time_.tv_sec << "." << c_time_.tv_usec << std::endl;
    result << "\t\tLast server respon time: " << s_time_.tv_sec << "." << s_time_.tv_usec << std::endl;
    result << "\t\tIs success: " << is_success_ << std::endl;
    return result.str();
}
