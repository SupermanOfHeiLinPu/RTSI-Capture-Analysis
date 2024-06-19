#include "RtsiCapture.hpp"
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
    host_id_ = eth_list["enp5s0"] + "30004";
}

void RtsiCapture::analysis(const TcpMessage& msg) {
    std::string src_id = msg.src_ip + std::to_string(msg.src_port);
    if (connection_.find(src_id) == connection_.end()) {
        auto con = std::make_shared<RtsiConnection>(host_id_);
        connection_.insert({src_id, con});
    }
    connection_[src_id]->analysis(msg);
}

void RtsiCapture::established(const TcpMessage& msg) {
    std::string src_id = msg.src_ip + std::to_string(msg.src_port);
    if (host_id_ == src_id) {
        return;
    }
    if (connection_.find(src_id) != connection_.end()) {
        connection_.erase(src_id);
    }
    auto con = std::make_shared<RtsiConnection>(host_id_);
    connection_.insert({src_id, con});
}

void RtsiCapture::close(const TcpMessage& msg) {
    std::string src_id = msg.src_ip + std::to_string(msg.src_port);
    if (host_id_ == src_id) {
        return;
    }
    if (connection_.find(src_id) != connection_.end()) {
        connection_.erase(src_id);
    }
}


RtsiConnection::RtsiConnection(const std::string& host_id) : host_id_(host_id) {

}

RtsiConnection::~RtsiConnection() {

}

void RtsiConnection::analysis(const TcpMessage& tm) {
}
