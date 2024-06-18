#include "TcpMessage.hpp"
#include <vector>

static std::vector<TcpMessage*> s_tm_list;

void registerTcpMessage(TcpMessage* tm) {
    s_tm_list.push_back(tm);
}


std::vector<TcpMessage*>& getTcpMessageList() {
    return s_tm_list;
}
