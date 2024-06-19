#ifndef __RTSI_PARSER_HPP__
#define __RTSI_PARSER_HPP__

#include <string>
#include <ctime>
#include <vector>
#include "TcpCapture.hpp"

class RtsiParser {
private:

protected:
    std::string host_id_;
public:
    RtsiParser(const std::string& host_id) : host_id_(host_id) {

    }
    virtual ~RtsiParser() = default;

    virtual void parser(const TcpMessage&, const std::vector<uint8_t>::const_iterator&) = 0;
    virtual std::string generateLog() = 0;

};

class RtsiParserProtocol : public RtsiParser {
private:
    timeval c_time_;
    timeval s_time_;
    bool is_success_;
public:
    RtsiParserProtocol(const std::string& host_id) : RtsiParser(host_id) { }
    virtual ~RtsiParserProtocol() = default;
    virtual void parser(const TcpMessage&, const std::vector<uint8_t>::const_iterator&);
    virtual std::string generateLog();
};

    



#endif
