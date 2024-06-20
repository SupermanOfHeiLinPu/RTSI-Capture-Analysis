#ifndef __RTSI_RECIPE_HPP__
#define __RTSI_RECIPE_HPP__

#include <string>
#include <ctime>
#include <vector>
#include <array>
#include <list>
#include <sstream>
#include "RtsiType.hpp"
#include "TcpCapture.hpp"

class RtsiRecipe {
private:
    std::string generateRawLog();
    std::string generateDataLog();

public:
    timeval last_time_;
    int id_;
    double frequency_;
    double actual_frequency_;
    std::vector<std::string> names_;
    std::vector<std::string> types_;
    std::list<std::vector<RtsiTypeVariant>> values_;
    bool is_raw_;

    RtsiRecipe() : is_raw_(true), id_(-1) {}

    void parser(const TcpMessage&, const std::vector<uint8_t>::const_iterator&);
    std::string generateLog();
};




#endif
