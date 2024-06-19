#ifndef __TCP_CAPTURE_HPP__
#define __TCP_CAPTURE_HPP__

#include <string>
#include <vector>
#include <cstdint>
#include <ctime>

class TcpMessage {
public:
    time_t time_sec;
    suseconds_t time_us;
    int src_port;
    int dst_port;
    std::string src_ip;
    std::string dst_ip;
    std::string src_mac;
    std::vector<uint8_t> data;
};

class TcpCapture {
public:
    virtual void analysis(const TcpMessage&) = 0;
    virtual void established(const TcpMessage&) = 0;
    virtual void close(const TcpMessage&) = 0;
};


void registerTcpMessage(TcpCapture* tm);

int startTcpCapture(const std::string& dev, const std::string& filter);

void printMessage(const TcpMessage& tmd);

#endif
