#ifndef __RTSI_CAPTURE_HPP__
#define __RTSI_CAPTURE_HPP__

#include "TcpCapture.hpp"
#include <unordered_map>
#include <string>
#include <memory>

class RtsiConnection {
private:
    std::string host_id_;
public:
    RtsiConnection(const std::string& host_id_);
    ~RtsiConnection();
    void analysis(const TcpMessage&);
};



class RtsiCapture :public TcpCapture {
private:
    std::string host_id_;
    std::unordered_map<std::string, std::shared_ptr<RtsiConnection>> connection_;
public:
    RtsiCapture();
    virtual void analysis(const TcpMessage&);
    virtual void established(const TcpMessage&);
    virtual void close(const TcpMessage&);

};







#endif