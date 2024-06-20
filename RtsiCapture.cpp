#include "RtsiCapture.hpp"
#include "EndianUtils.hpp"
#include "CommUtils.hpp"
#include <string>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <sstream>

static std::unordered_map<std::string, std::string> get_eth_list() {
    struct ifaddrs *ifa = NULL, *ifList;
    std::unordered_map<std::string, std::string> result;
    if (getifaddrs(&ifList) < 0) {
        return {};
    }
    for (ifa = ifList; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if(ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *sin = (struct sockaddr_in *)ifa->ifa_addr;
            result.insert({ifa->ifa_name, inet_ntoa(sin->sin_addr)});
        }
    }
    freeifaddrs(ifList);
    return result;
}

RtsiCapture::RtsiCapture(const std::string& eth) {
    auto eth_list = get_eth_list();
    host_id_ = CommUtils::buildID(eth_list[eth], 30004);
}

void RtsiCapture::analysis(const TcpMessage& tm) {
    std::string client_id = CommUtils::buildID(tm);

    if (client_id == host_id_) {
        client_id = CommUtils::buildID(tm.dst_ip, tm.dst_port);
    }
    
    if (connection_.find(client_id) == connection_.end()) {
        auto con = std::make_shared<RtsiConnection>(host_id_, client_id);
        connection_.insert({client_id, con});
    }
    connection_[client_id]->analysis(tm);
}

void RtsiCapture::established(const TcpMessage& msg) {
    std::string client_id = CommUtils::buildID(msg);
    if (host_id_ == client_id) {
        return;
    }
    if (connection_.find(client_id) != connection_.end()) {
        connection_.erase(client_id);
    }
    auto con = std::make_shared<RtsiConnection>(host_id_, client_id);
    connection_.insert({client_id, con});
}

void RtsiCapture::close(const TcpMessage& msg) {
    std::string client_id = CommUtils::buildID(msg);
    if (host_id_ == client_id) {
        return;
    }
    if (connection_.find(client_id) != connection_.end()) {
        // TODO:保存日志
        std::cout << "Client disconnect: " << msg.time_sec << "." << msg.time_us << std::endl;
        std::cout << connection_[client_id]->generateLog() << std::endl;
        connection_.erase(client_id);
    }
}


RtsiConnection::RtsiConnection(const std::string& host_id, const std::string& client_id) : 
    host_id_(host_id),
    client_id_(client_id),
    protocol_parser_(host_id),
    control_version_parser_(host_id),
    start_parser_(host_id),
    pause_parser_(host_id),
    data_parser_(host_id, orecipes_, irecipes_) {

}

RtsiConnection::~RtsiConnection() {

}

void RtsiConnection::analysis(const TcpMessage& tm) {
    if(buffer_.size() > 0) {
        buffer_.insert(buffer_.end(), tm.data.begin(), tm.data.end());
        int parser_len = 0;
        if(!parser(tm, buffer_, parser_len)) {
            // TODO
        }
        // remove parsered message
        if (parser_len > 0) {
            buffer_.erase(buffer_.begin(), buffer_.begin() + parser_len);
        }
    } else {
        int parser_len = 0;
        if(!parser(tm, tm.data, parser_len)) {
            // TODO
        }
        // Adds the remaining unparsed packets to the buffer
        if (parser_len < tm.data.size()) {
            buffer_.insert(buffer_.end(), tm.data.begin() + parser_len, tm.data.end());
        }
    }
}

bool RtsiConnection::parser(const TcpMessage& tm, const std::vector<uint8_t>& msg, int& parsered_len) {
    uint16_t package_len = 0;
    parsered_len = 0;
    if (msg.size() < 3) {
        return true;
    }
    do {
        EndianUtils::unpack(msg.begin() + parsered_len, package_len);
        RtsiPackageType pt = (RtsiPackageType)msg[2];
        if (package_len > (msg.size() - parsered_len)) {
            break;
        }
        
        if (pt == RtsiPackageType::REQUEST_PROTOCOL_VERSION) {
            protocol_parser_.parser(tm, msg.begin() + parsered_len);
            protocol_parser_.setRecvFlag();
        } else if (pt == RtsiPackageType::GET_ELITE_CONTROL_VERSION) {
            control_version_parser_.parser(tm, msg.begin() + parsered_len);
            control_version_parser_.setRecvFlag();
        } else if (pt == RtsiPackageType::TEXT_MESSAGE) {
            // TODO:
        } else if (pt == RtsiPackageType::DATA_PACKAGE) {
            data_parser_.parser(tm, msg.begin() + parsered_len);
        } else if (pt == RtsiPackageType::CONTROL_PACKAGE_SETUP_OUTPUTS) {

        } else if (pt == RtsiPackageType::CONTROL_PACKAGE_SETUP_INPUTS) {

        } else if (pt == RtsiPackageType::CONTROL_PACKAGE_START) {
            start_parser_.parser(tm, msg.begin() + parsered_len);
            start_parser_.setRecvFlag();
        }  else if (pt == RtsiPackageType::CONTROL_PACKAGE_PAUSE) {
            pause_parser_.parser(tm, msg.begin() + parsered_len);
            pause_parser_.setRecvFlag();
        } else {
            // TODO
        }
        parsered_len += package_len;
    } while ((parsered_len + 3) <= msg.size());
    return true;
}

std::string RtsiConnection::generateLog() {
    std::stringstream result;
    result << "Client ID: " << client_id_ << std::endl;
    result << protocol_parser_.generateLog();
    result << control_version_parser_.generateLog();
    result << start_parser_.generateLog();
    result << pause_parser_.generateLog();
    result << data_parser_.generateLog();
    return result.str();
}