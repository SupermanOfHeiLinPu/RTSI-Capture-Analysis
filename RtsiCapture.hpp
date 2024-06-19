#ifndef __RTSI_CAPTURE_HPP__
#define __RTSI_CAPTURE_HPP__

#include "TcpCapture.hpp"
#include <unordered_map>
#include <string>
#include <memory>

class RtsiConnection {
private:
    std::string host_id_;
    bool version_check_;
    std::vector<uint8_t> buffer_;

    bool parser(const std::vector<uint8_t>&, int& parser_len);
public:
    RtsiConnection(const std::string& host_id_);
    ~RtsiConnection();
    void analysis(const TcpMessage&);
};



class RtsiCapture :public TcpCapture {
private:
    std::string host_id_;
    std::unordered_map<std::string, std::shared_ptr<RtsiConnection>> connection_;
    std::string buildID(const std::string& ip, int port);
    std::string buildID(const TcpMessage&);
public:
    RtsiCapture();
    virtual void analysis(const TcpMessage&);
    virtual void established(const TcpMessage&);
    virtual void close(const TcpMessage&);

};







#endif