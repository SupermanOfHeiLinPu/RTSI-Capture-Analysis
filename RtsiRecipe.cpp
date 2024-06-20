#include "RtsiRecipe.hpp"
#include "EndianUtils.hpp"
#include <iomanip>


void RtsiRecipe::parser(const TcpMessage& tm, const std::vector<uint8_t>::const_iterator& msg) {
    id_ = msg[3];
    double last_second = (double)last_time_.tv_sec + (last_time_.tv_usec / 1000000.0);
    double current_second = (double)tm.time_sec + (tm.time_us / 1000000.0);
    actual_frequency_ = 1 / (current_second - last_second);
    last_time_.tv_sec = tm.time_sec;
    last_time_.tv_usec = tm.time_us;
    
    int16_t pkg_len;
    EndianUtils::unpack(msg, pkg_len);

    if (values_.size() > 100) {
        values_.pop_front();
    }
    
    if (is_raw_) {
        std::vector<RtsiTypeVariant> value_buf;
        RtsiTypeVariant temp;
        for (size_t i = 4; i < pkg_len; i++) {
            temp = msg[i];
            value_buf.push_back(std::move(temp));
        }
        values_.push_back(std::move(value_buf));
    } else {
        // TODO
    }
    
}

std::string RtsiRecipe::generateRawLog() {
    std::stringstream result;
    result << "\t\tNot config recipe. Raw: " << std::endl;
    int count = 0;
    for(auto& raw_data_list : values_) {
        count++;
        result << "\t\t\tHistory " << count <<": ";
        for (int i = 0; i < raw_data_list.size(); i++) {
            if (i % 16 == 0) {
                result << "\n\t\t\t";
            }
            result << std::setfill('0') << std::setw(2) 
                   << std::hex << (unsigned int)boost::get<uint8_t>(raw_data_list[i]) 
                   << " " << std::dec;
        }
        result << std::endl;
    }
    return result.str();
}

std::string RtsiRecipe::generateDataLog() {
    // TODO
}


std::string RtsiRecipe::generateLog() {
    std::stringstream result;
    result << "\t\tID: " << id_ << std::endl;
    result << "\t\tTarge frequency: " << frequency_ << std::endl;
    result << "\t\tActual frequency: " << actual_frequency_ << std::endl;
    result << "\t\tLast timestamp: " << last_time_.tv_sec << "." << last_time_.tv_usec << std::endl;
    if (is_raw_) {
        result << generateRawLog();
    } else {
        result << generateDataLog();
    }
    return result.str();
}
