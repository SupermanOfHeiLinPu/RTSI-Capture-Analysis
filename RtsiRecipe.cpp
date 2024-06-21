#include "RtsiRecipe.hpp"
#include "EndianUtils.hpp"
#include <iomanip>


void RtsiRecipe::parser(const TcpMessage& tm, const std::vector<uint8_t>::const_iterator& msg, bool only_one ) {
    id_ = msg[3];
    double last_second = (double)last_time_.tv_sec + (last_time_.tv_usec / 1000000.0);
    double current_second = (double)tm.time_sec + (tm.time_us / 1000000.0);
    actual_frequency_ = 1 / (current_second - last_second);
    last_time_.tv_sec = tm.time_sec;
    last_time_.tv_usec = tm.time_us;
    
    int16_t pkg_len;
    EndianUtils::unpack(msg, pkg_len);
    
    std::vector<uint8_t> value_buf;
    for (size_t i = 4; i < pkg_len; i++) {
        value_buf.push_back(msg[i]);
    }

    if (only_one) {
        values_.clear();
        values_.push_back(std::move(value_buf));
    } else {
        if (values_.size() > 100) {
            values_.pop_front();
        }
        values_.push_back(std::move(value_buf));
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
                result << "\n\t\t\t\t";
            }
            result << std::setfill('0') << std::setw(2) 
                   << std::hex << (int)raw_data_list[i] 
                   << " " << std::dec;
        }
        result << std::endl;
    }
    return result.str();
}


template<typename ARR_T>
static void generateArrayDataLog(const ARR_T& arr, std::stringstream& result) {
    for (const auto& i : arr) {
        result << i << " ";
    }
}

void RtsiRecipe::generateOneDataLog(const std::vector<uint8_t>& one, std::stringstream& result) {
    int msg_offset = 0;
    for (size_t i = 0; i < types_.size(); i++) {
        result << "\n\t\t\t\t";
        if (types_[i] == "UINT8") { 
            result << names_[i] << "\t" << types_[i] << "\t" << (int)*(one.begin() + msg_offset);
            msg_offset += sizeof(uint8_t);

        }  else if (types_[i] == "BOOL") {
            result << names_[i] << "\t" << types_[i] << "\t" << (int)*(one.begin() + msg_offset);
            msg_offset += sizeof(bool);

        } else if (types_[i] == "UINT16") {
            uint16_t value;
            EndianUtils::unpack(one.begin() + msg_offset, value);
            result << names_[i] << "\t" << types_[i] << "\t" << value;
            msg_offset += sizeof(value);
            
        } else if (types_[i] == "UINT32") {
            uint32_t value;
            EndianUtils::unpack(one.begin() + msg_offset, value);
            result << names_[i] << "\t" << types_[i] << "\t" << value;
            msg_offset += sizeof(value);

        }  else if (types_[i] == "INT32") {
            int32_t value;
            EndianUtils::unpack(one.begin() + msg_offset, value);
            result << names_[i] << "\t" << types_[i] << "\t" << value;
            msg_offset += sizeof(value);

        } else if (types_[i] == "UINT64") {
            uint64_t value;
            EndianUtils::unpack(one.begin() + msg_offset, value);
            result << names_[i] << "\t" << types_[i] << "\t" << value;
            msg_offset += sizeof(value);    

        } else if (types_[i] == "DOUBLE") {
            double value;
            EndianUtils::unpack(one.begin() + msg_offset, value);
            result << names_[i] << "\t" << types_[i] << "\t" << value;
            msg_offset += sizeof(value);

        } else if (types_[i] == "VECTOR3D") {
            vector3d_t value;
            EndianUtils::unpackArr<double, 3>(one.begin() + msg_offset, value);
            result << names_[i] << "\t" << types_[i] << "\t";
            generateArrayDataLog(value, result);
            msg_offset += sizeof(value);

        } else if (types_[i] == "VECTOR6D") {
            vector6d_t value;
            EndianUtils::unpackArr<double, 6>(one.begin() + msg_offset, value);
            result << names_[i] << "\t" << types_[i] << "\t";
            generateArrayDataLog(value, result);
            msg_offset += sizeof(value);

        } else if (types_[i] == "VECTOR6INT32") {
            vector6int32_t value;
            EndianUtils::unpackArr<int32_t, 6>(one.begin() + msg_offset, value);
            result << names_[i] << "\t" << types_[i] << "\t";
            generateArrayDataLog(value, result);
            msg_offset += sizeof(value);

        }
    }
    result << std::endl;
}

std::string RtsiRecipe::generateDataLog() {
    std::stringstream result;
    result << "\t\tParsered Data:\n";
    int count = 0;
    for(auto one : values_) {
        count++;
        result << "\t\t\tHistory " << count <<": ";
        generateOneDataLog(one, result);
    }
    return result.str();
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
