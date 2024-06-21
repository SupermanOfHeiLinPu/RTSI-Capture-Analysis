#ifndef __RTSI_RECIPE_SETUP_HPP__
#define __RTSI_RECIPE_SETUP_HPP__

#include "RtsiParser.hpp"
#include "TcpCapture.hpp"
#include "RtsiRecipe.hpp"

class RtsiReciepSetup : public RtsiParser {
private:
    std::vector<std::string> parserString(const std::vector<uint8_t>::const_iterator& msg, int offset, int pkg_len, RtsiRecipe& recipe);
public:
    RtsiReciepSetup(const std::string& host_id) : RtsiParser(host_id) { }
    virtual ~RtsiReciepSetup() = default;

    [[deprecated("Use parserSetup() instead")]]
    virtual void parser(const TcpMessage&, const std::vector<uint8_t>::const_iterator&) { };

    void parserOutSetup(const TcpMessage& tm, 
                        const std::vector<uint8_t>::const_iterator& msg,
                        std::array<RtsiRecipe, 255>& recipes,
                        int recipe_count,
                        int pkg_len);

    void parserInSetup(const TcpMessage& tm, 
                        const std::vector<uint8_t>::const_iterator& msg,
                        std::array<RtsiRecipe, 255>& recipes,
                        int recipe_count,
                        int pkg_len);

    [[deprecated("Use RtsiParserData::generateLog instead")]]
    virtual std::string generateLog() {}

};





#endif
