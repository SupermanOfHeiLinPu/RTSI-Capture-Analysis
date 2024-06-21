#include "RtsiReciepSetup.hpp"
#include "CommUtils.hpp"
#include "EndianUtils.hpp"
#include <string>

std::vector<std::string> RtsiReciepSetup::parserString(const std::vector<uint8_t>::const_iterator& msg, 
                                  int offset, 
                                  int pkg_len, 
                                  RtsiRecipe& recipe) {
    std::string types;
    for (size_t i = offset; i < pkg_len; i++) {
        types += msg[i];
    }
    return CommUtils::splitString(types, ",");
}

void RtsiReciepSetup::parserOutSetup(const TcpMessage& tm, 
                                     const std::vector<uint8_t>::const_iterator& msg,
                                     std::array<RtsiRecipe, 255>& recipes,
                                     int recipe_count,
                                     int pkg_len) {

    std::string src_id = CommUtils::buildID(tm);
    if (src_id == host_id_) {
        // 第4字节是配方的ID
        int r_id = *(msg + 3);
        recipes[r_id].id_ = r_id;
        recipes[r_id].types_ = parserString(msg, 4, pkg_len, recipes[r_id]);
        recipes[r_id].is_raw_ = false;
    } else {
        EndianUtils::unpack(msg + 3, recipes[recipe_count].frequency_);
        recipes[recipe_count].names_ = parserString(msg, 11, pkg_len, recipes[recipe_count]);
    }
    
    
}



void RtsiReciepSetup::parserInSetup(const TcpMessage& tm, 
                                    const std::vector<uint8_t>::const_iterator& msg,
                                    std::array<RtsiRecipe, 255>& recipes,
                                    int recipe_count,
                                    int pkg_len) {

    std::string src_id = CommUtils::buildID(tm);
    if (src_id == host_id_) {
        // 第4字节是配方的ID
        int r_id = *(msg + 3);
        recipes[r_id].id_ = r_id;
        recipes[r_id].types_ = parserString(msg, 4, pkg_len, recipes[r_id]);
        recipes[r_id].is_raw_ = false;

    } else {
        recipes[recipe_count].names_ = parserString(msg, 3, pkg_len, recipes[recipe_count]);
    }
}
