#ifndef __RTSI_RPC_HPP__
#define __RTSI_RPC_HPP__

#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>
#include <xmlrpc-c/server_abyss.hpp>
#include <thread>
#include <memory>

#include "RtsiCapture.hpp"

class RtsiRpc {
private:
    std::unique_ptr<xmlrpc_c::serverAbyss> rpc_server_;
    std::unique_ptr<xmlrpc_c::registry> rpc_registry_;
    RtsiCapture& cap_;
    std::unique_ptr<std::thread> rpc_thread_;
public:
    RtsiRpc(RtsiCapture& cap);
    ~RtsiRpc() = default;

    void init();

};




#endif
