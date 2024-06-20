#ifndef __RTSI_PARSER_DATA_HPP__
#define __RTSI_PARSER_DATA_HPP__

#include <array>
#include <string>
#include "RtsiRecipe.hpp"
#include "RtsiParser.hpp"
#include "TcpCapture.hpp"

class RtsiParserData : public RtsiParser {
private:
    std::array<RtsiRecipe, 255>& orecipes_;
    std::array<RtsiRecipe, 255>& irecipes_;

    void parserOutRecipe(const TcpMessage&, const std::vector<uint8_t>::const_iterator&);
    void parserInRecipe(const TcpMessage&, const std::vector<uint8_t>::const_iterator&);
public:
    RtsiParserData(const std::string& host_id, 
                   std::array<RtsiRecipe, 255>& orecipes,
                   std::array<RtsiRecipe, 255>& irecieps ) 
                   : RtsiParser(host_id),
                     orecipes_(orecipes), 
                     irecipes_(irecieps) { }
    virtual ~RtsiParserData() = default;
    
    virtual void parser(const TcpMessage&, const std::vector<uint8_t>::const_iterator&);

    virtual std::string generateLog();
};



#endif
