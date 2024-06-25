#ifndef __RTSI_CAPTURE_HPP__
#define __RTSI_CAPTURE_HPP__

#include "TcpCapture.hpp"
#include "RtsiParser.hpp"
#include "RtsiParserData.hpp"
#include "RtsiRecipe.hpp"
#include "RtsiReciepSetup.hpp"
#include <unordered_map>
#include <string>
#include <memory>
#include <array>
#include <fstream>
#include <mutex>
#include <functional>

/**
 * @brief Rtsi package type
 * 
 */
enum class RtsiPackageType : uint8_t {
    REQUEST_PROTOCOL_VERSION = 86,       // ascii V
    GET_ELITE_CONTROL_VERSION = 118,     // ascii v
    TEXT_MESSAGE = 77,                   // ascii M
    DATA_PACKAGE = 85,                   // ascii U
    CONTROL_PACKAGE_SETUP_OUTPUTS = 79,  // ascii O
    CONTROL_PACKAGE_SETUP_INPUTS = 73,   // ascii I
    CONTROL_PACKAGE_START = 83,          // ascii S
    CONTROL_PACKAGE_PAUSE = 80           // ascii P
};

class RtsiConnection {
private:
    std::string client_id_;
    std::string host_id_;
    std::vector<uint8_t> buffer_;
    RtsiParserProtocol protocol_parser_;
    RtsiParserControlVersion control_version_parser_;
    RtsiParserPause pause_parser_;
    RtsiParserStart start_parser_;
    RtsiParserData data_parser_;
    RtsiReciepSetup setup_parser_;
    int orecipe_count_;
    int irecipe_count_;
    std::array<RtsiRecipe, 255> orecipes_;
    std::array<RtsiRecipe, 255> irecipes_;
    bool parser(const TcpMessage&, const std::vector<uint8_t>&, int& parser_len);

public:
    RtsiConnection(const std::string& host_id, const std::string& client_id);
    ~RtsiConnection();
    void analysis(const TcpMessage&);
    std::string generateLog();
};



class RtsiCapture :public TcpCapture {
private:
    std::string host_id_;
    std::unordered_map<std::string, std::shared_ptr<RtsiConnection>> connection_;
    std::mutex mutex_;
public:
    RtsiCapture(const std::string& eth);
    virtual void analysis(const TcpMessage&);
    virtual void established(const TcpMessage&);
    virtual void close(const TcpMessage&);
    
    void saveConnectionsToFile();
};







#endif