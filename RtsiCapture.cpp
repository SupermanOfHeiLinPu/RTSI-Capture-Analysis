#include "RtsiCapture.hpp"
#include "EndianUtils.hpp"
#include "CommUtils.hpp"
#include <string>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <regex>
#include <algorithm>


static std::unordered_map<std::string, std::string> get_eth_list() {
    struct ifaddrs *ifa = NULL, *ifList;
    std::unordered_map<std::string, std::string> result;
    if (getifaddrs(&ifList) < 0) {
        return {};
    }
    for (ifa = ifList; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if(ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *sin = (struct sockaddr_in *)ifa->ifa_addr;
            result.insert({ifa->ifa_name, inet_ntoa(sin->sin_addr)});
        }
    }
    freeifaddrs(ifList);
    return result;
}

RtsiCapture::RtsiCapture(const std::string& eth) {
    auto eth_list = get_eth_list();
    host_id_ = CommUtils::buildID(eth_list[eth], 30004);
    save_path_ = "/home/elite/EliRobot/program/rtsi_cap_log/";
    dis_save_file_name_ = "disconnect_save_analysis";
    dis_save_file_count_ = 0;
    dis_save_file_max_count_ = 5;
    DIR* sd = opendir(save_path_.c_str());
    if (sd != nullptr) {
        dirent* dirp = nullptr;
        while ((dirp = readdir(sd)) != nullptr) {
            std::string fn(dirp->d_name);
            if (fn.find(dis_save_file_name_) != std::string::npos) {
                dis_save_file_count_++;
                dis_save_file_name_list_.push_back(save_path_ + fn);
            }
        }
        if (dis_save_file_name_list_.size() > 0) {
            dis_save_file_name_list_.sort([&](std::string& f1, std::string& f2) {
                int f1_count = extractCount(f1);
                int f2_count = extractCount(f2);
                if (dis_save_file_count_ < f1_count) {
                    dis_save_file_count_ = f1_count;
                }
                if (dis_save_file_count_ < f2_count) {
                    dis_save_file_count_ = f2_count;
                }
                return f1_count < f2_count;
            });
        }
        dis_save_file_count_++;
        closedir(sd);
    }
}

int RtsiCapture::extractCount(const std::string& filename) {
    std::regex re(".*" + dis_save_file_name_ + "(\\d+)\\.txt");
    std::smatch match;
    if (std::regex_search(filename, match, re)) {
        return std::stoi(match[1].str());
    }
    return -1;  // 如果格式不匹配，返回-1
}

void RtsiCapture::analysis(const TcpMessage& tm) {
    std::string client_id = CommUtils::buildID(tm);

    if (client_id == host_id_) {
        client_id = CommUtils::buildID(tm.dst_ip, tm.dst_port);
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    if (connection_.find(client_id) == connection_.end()) {
        auto con = std::make_shared<RtsiConnection>(host_id_, client_id);
        connection_.insert({client_id, con});
    }
    connection_[client_id]->analysis(tm);
}

void RtsiCapture::established(const TcpMessage& msg) {
    std::string client_id = CommUtils::buildID(msg);
    if (host_id_ == client_id) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (connection_.find(client_id) != connection_.end()) {
        connection_.erase(client_id);
    }
    auto con = std::make_shared<RtsiConnection>(host_id_, client_id);
    connection_.insert({client_id, con});
}

void RtsiCapture::close(const TcpMessage& msg) {
    std::string client_id = CommUtils::buildID(msg);
    if (host_id_ == client_id) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (connection_.find(client_id) != connection_.end()) {
        std::string file_name = save_path_ + dis_save_file_name_ + std::to_string(dis_save_file_count_) + ".txt";
        std::ofstream fs(file_name);
        if (!fs.is_open()) {
            if (!createDirectories(save_path_)) {
                goto done;
            } else {
                fs.open(file_name);
                if (!fs.is_open()) {
                    goto done;
                }   
            }   
        }
        if (dis_save_file_name_list_.size() > dis_save_file_max_count_) {
            std::string delete_file_name;
            remove(dis_save_file_name_list_.front().c_str());
            dis_save_file_name_list_.pop_front();
        }
        fs << "Client disconnect: " << msg.time_sec << "." << msg.time_us << std::endl;
        fs << connection_[client_id]->generateLog() << std::endl;
        dis_save_file_name_list_.push_back(file_name);
        dis_save_file_count_++;
done:
        connection_.erase(client_id);
    }
}

bool RtsiCapture::createDirectories(const std::string& path) {
    size_t pre = 0, pos;
    std::string dir;
    int mdret;
    while ((pos = path.find_first_of('/', pre)) != std::string::npos) {
        dir = path.substr(0, pos++);
        pre = pos;
        if (dir.empty()) continue; // 跳过根路径（例如"/"）

        if ((mdret = mkdir(dir.c_str(), S_IRWXU)) && errno != EEXIST) {
            std::cerr << "Error creating directory " << dir << ": " << strerror(errno) << std::endl;
            return false;
        }
    }
    return true;
}

bool RtsiCapture::saveConnectionsToFile() {
    std::string file_name = save_path_ + "save_analysis.txt";
    std::ofstream fs(file_name);
    if (!fs.is_open()) {
        if (!createDirectories(save_path_)) {
            return false;
        } else {
            fs.open(file_name);
            if (!fs.is_open()) {
                return false;
            }
        }
    }

    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& con : connection_) {
        fs << con.second->generateLog() << std::endl;
    }
}

RtsiConnection::RtsiConnection(const std::string& host_id, const std::string& client_id) : 
    host_id_(host_id),
    client_id_(client_id),
    protocol_parser_(host_id),
    control_version_parser_(host_id),
    start_parser_(host_id),
    pause_parser_(host_id),
    data_parser_(host_id, orecipes_, irecipes_),
    setup_parser_(host_id) {
        orecipe_count_ = 1;
        irecipe_count_ = 1;
}

RtsiConnection::~RtsiConnection() {

}

void RtsiConnection::analysis(const TcpMessage& tm) {
    if(buffer_.size() > 0) {
        buffer_.insert(buffer_.end(), tm.data.begin(), tm.data.end());
        int parser_len = 0;
        if(!parser(tm, buffer_, parser_len)) {
            // TODO
        }
        // remove parsered message
        if (parser_len > 0) {
            buffer_.erase(buffer_.begin(), buffer_.begin() + parser_len);
        }
    } else {
        int parser_len = 0;
        if(!parser(tm, tm.data, parser_len)) {
            // TODO
        }
        // Adds the remaining unparsed packets to the buffer
        if (parser_len < tm.data.size()) {
            buffer_.insert(buffer_.end(), tm.data.begin() + parser_len, tm.data.end());
        }
    }
}

bool RtsiConnection::parser(const TcpMessage& tm, const std::vector<uint8_t>& msg, int& parsered_len) {
    uint16_t package_len = 0;
    parsered_len = 0;
    if (msg.size() < 3) {
        return true;
    }
    do {
        EndianUtils::unpack(msg.begin() + parsered_len, package_len);
        RtsiPackageType pt = (RtsiPackageType)msg[2];
        if (package_len > (msg.size() - parsered_len)) {
            break;
        }
        
        if (pt == RtsiPackageType::REQUEST_PROTOCOL_VERSION) {
            protocol_parser_.parser(tm, msg.begin() + parsered_len);
            protocol_parser_.setRecvFlag();

        } else if (pt == RtsiPackageType::GET_ELITE_CONTROL_VERSION) {
            control_version_parser_.parser(tm, msg.begin() + parsered_len);
            control_version_parser_.setRecvFlag();

        } else if (pt == RtsiPackageType::TEXT_MESSAGE) {
            // TODO:
        } else if (pt == RtsiPackageType::DATA_PACKAGE) {
            data_parser_.parser(tm, msg.begin() + parsered_len);

        } else if (pt == RtsiPackageType::CONTROL_PACKAGE_SETUP_OUTPUTS) {
            setup_parser_.parserOutSetup(tm, msg.begin() + parsered_len, orecipes_, orecipe_count_, package_len);

        } else if (pt == RtsiPackageType::CONTROL_PACKAGE_SETUP_INPUTS) {
            setup_parser_.parserInSetup(tm, msg.begin() + parsered_len, irecipes_, irecipe_count_, package_len);

        } else if (pt == RtsiPackageType::CONTROL_PACKAGE_START) {
            start_parser_.parser(tm, msg.begin() + parsered_len);
            start_parser_.setRecvFlag();

        }  else if (pt == RtsiPackageType::CONTROL_PACKAGE_PAUSE) {
            pause_parser_.parser(tm, msg.begin() + parsered_len);
            pause_parser_.setRecvFlag();

        } else {
            // TODO
        }
        parsered_len += package_len;
    } while ((parsered_len + 3) <= msg.size());
    return true;
}

std::string RtsiConnection::generateLog() {
    std::stringstream result;
    result << "Client ID: " << client_id_ << std::endl;
    result << protocol_parser_.generateLog();
    result << control_version_parser_.generateLog();
    result << start_parser_.generateLog();
    result << pause_parser_.generateLog();
    result << data_parser_.generateLog();
    return result.str();
}