#ifndef __RTSI_PARSER_HPP__
#define __RTSI_PARSER_HPP__

#include <string>
#include <ctime>
#include <vector>
#include <array>
#include <list>
#include <sstream>
#include <boost/variant.hpp>
#include "TcpCapture.hpp"
#include "RtsiRecipe.hpp"

class RtsiParser {
private:

protected:
    std::string host_id_;
    timeval c_time_;
    timeval s_time_;
    bool is_recv_;
public:
    RtsiParser(const std::string& host_id) : host_id_(host_id) {

    }
    virtual ~RtsiParser() = default;

    virtual void parser(const TcpMessage&, const std::vector<uint8_t>::const_iterator&) = 0;
    virtual std::string generateLog() = 0;
    void setRecvFlag() { is_recv_ = true; };

};

class RtsiParserProtocol : public RtsiParser {
private:
    bool is_success_;
public:
    RtsiParserProtocol(const std::string& host_id) : RtsiParser(host_id) { }
    virtual ~RtsiParserProtocol() = default;
    virtual void parser(const TcpMessage&, const std::vector<uint8_t>::const_iterator&);
    virtual std::string generateLog();
};

class RtsiParserControlVersion : public RtsiParser {
private:
    uint32_t major_ = 0;
    uint32_t minor_ = 0;
    uint32_t bugfix_ = 0;
    uint32_t build_ = 0;
    
public:
    RtsiParserControlVersion(const std::string& host_id) : RtsiParser(host_id) { }
    virtual ~RtsiParserControlVersion() = default;
    virtual void parser(const TcpMessage&, const std::vector<uint8_t>::const_iterator&);
    virtual std::string generateLog();
};

class RtsiParserStart : public RtsiParser {
private:
    bool is_success_;
    
public:
    RtsiParserStart(const std::string& host_id) : RtsiParser(host_id) { }
    virtual ~RtsiParserStart() = default;
    virtual void parser(const TcpMessage&, const std::vector<uint8_t>::const_iterator&);
    virtual std::string generateLog();
};

class RtsiParserPause : public RtsiParser {
private:
    bool is_success_;
    
public:
    RtsiParserPause(const std::string& host_id) : RtsiParser(host_id) { }
    virtual ~RtsiParserPause() = default;
    virtual void parser(const TcpMessage&, const std::vector<uint8_t>::const_iterator&);
    virtual std::string generateLog();
};



#endif
