#include "TcpCapture.hpp"
#include <vector>

static std::vector<TcpCapture*> s_tm_list;

void registerTcpMessage(TcpCapture* tm) {
    s_tm_list.push_back(tm);
}


std::vector<TcpCapture*>& getTcpMessageList() {
    return s_tm_list;
}
