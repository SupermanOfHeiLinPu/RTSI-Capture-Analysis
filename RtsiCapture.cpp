#include "RtsiCapture.hpp"
#include "EndianUtils.hpp"
#include <string>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>

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

RtsiCapture::RtsiCapture() {
    auto eth_list = get_eth_list();
    host_id_ = buildID(eth_list["enp5s0"], 30004);
}

std::string RtsiCapture::buildID(const std::string& ip, int port) {
    return ip + ":" + std::to_string(port);
}

std::string RtsiCapture::buildID(const TcpMessage& tm) {
    return buildID(tm.src_ip, tm.src_port);
}

void RtsiCapture::analysis(const TcpMessage& tm) {
    std::string client_id = buildID(tm);
    if (client_id == host_id_) {
        client_id = buildID(tm.dst_ip, tm.dst_port);
    }
    
    if (connection_.find(client_id) == connection_.end()) {
        auto con = std::make_shared<RtsiConnection>(host_id_);
        connection_.insert({client_id, con});
    }
    connection_[client_id]->analysis(tm);
}

void RtsiCapture::established(const TcpMessage& msg) {
    std::string client_id = buildID(msg);
    if (host_id_ == client_id) {
        return;
    }
    if (connection_.find(client_id) != connection_.end()) {
        connection_.erase(client_id);
    }
    auto con = std::make_shared<RtsiConnection>(host_id_);
    connection_.insert({client_id, con});
}

void RtsiCapture::close(const TcpMessage& msg) {
    std::string client_id = buildID(msg);
    if (host_id_ == client_id) {
        return;
    }
    if (connection_.find(client_id) != connection_.end()) {
        connection_.erase(client_id);
    }
}


RtsiConnection::RtsiConnection(const std::string& host_id) : host_id_(host_id) {

}

RtsiConnection::~RtsiConnection() {

}

void RtsiConnection::analysis(const TcpMessage& tm) {
    if(buffer_.size() > 0) {
        buffer_.insert(buffer_.end(), tm.data.begin(), tm.data.end());
        int parser_len = 0;
        if(!parser(buffer_, parser_len)) {
            // TODO
        }
        // remove parsered message
        if (parser_len > 0) {
            buffer_.erase(buffer_.begin(), buffer_.begin() + parser_len);
        }
    } else {
        int parser_len = 0;
        if(!parser(buffer_, parser_len)) {
            // TODO
        }
        // Adds the remaining unparsed packets to the buffer
        if (parser_len < tm.data.size()) {
            buffer_.insert(buffer_.end(), tm.data.begin() + parser_len, tm.data.end());
        }
    }
}

bool RtsiConnection::parser(const std::vector<uint8_t>& msg, int& parser_len) {
    parser_len = msg.size();
    return true;
}