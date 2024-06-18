#ifndef __TCP_MESSAGE_HPP__
#define __TCP_MESSAGE_HPP__

#include <string>
#include <vector>
#include <cstdint>

class TcpMessageData {
public:
    int src_port;
    int dst_port;
    std::string src_ip;
    std::string dst_ip;
    std::string src_mac;
    std::vector<uint8_t> data;
};

class TcpMessage {
public:
    virtual void analysis(const TcpMessageData&) = 0;
    virtual void established(const TcpMessageData&) = 0;
    virtual void close(const TcpMessageData&) = 0;
};


void registerTcpMessage(TcpMessage* tm);




#endif
