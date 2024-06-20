#include "RtsiParserData.hpp"
#include "CommUtils.hpp"

void RtsiParserData::parser(const TcpMessage& tm, const std::vector<uint8_t>::const_iterator& msg) {
    std::string src_id = CommUtils::buildID(tm);
    if (src_id == host_id_) {
        parserOutRecipe(tm, msg);
    } else {
        parserInRecipe(tm, msg);
    }
}

std::string RtsiParserData::generateLog() {
    std::stringstream result;
    result << "\tRecipe output:\n";
    for (auto& recipe : orecipes_) {
        if (recipe.id_ > 0) {
            result << recipe.generateLog();
        }
    }
    result << std::endl;

    result << "\tRecipe input:\n";
    for (auto& recipe : irecipes_) {
        if (recipe.id_ > 0) {
            result << recipe.generateLog();
        }
    }
    return result.str();
}

void RtsiParserData::parserOutRecipe(const TcpMessage& tm, const std::vector<uint8_t>::const_iterator& msg) {
    if (msg[3] >= 255) {
        throw std::runtime_error("Error output recipe id: " + std::to_string(msg[3]));
        return;
    }
    
    orecipes_[msg[3]].parser(tm, msg);
}

void RtsiParserData::parserInRecipe(const TcpMessage& tm, const std::vector<uint8_t>::const_iterator& msg) {
    if (msg[3] >= 255) {
        throw std::runtime_error("Error input recipe id: " + std::to_string(msg[3]));
        return;
    }
    irecipes_[msg[3]].parser(tm, msg);
}
