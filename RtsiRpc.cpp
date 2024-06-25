#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>
#include <xmlrpc-c/server_abyss.hpp>
#include "RtsiRpc.hpp"

#define RTSI_CAPTURE_PORT 7891

class RtsiSaveConnections : public xmlrpc_c::method {
private:
    RtsiCapture& cap_;
public:
    RtsiSaveConnections(RtsiCapture& cap) : cap_(cap) {}
    void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value* const retvalP) {
        cap_.saveConnectionsToFile();
        *retvalP = xmlrpc_c::value_int(0);
    }
};


RtsiRpc::RtsiRpc(RtsiCapture& cap) : cap_(cap) {

}

void RtsiRpc::init() {
    rpc_registry_ = std::make_unique<xmlrpc_c::registry>();

    rpc_registry_->addMethod("saveConnectionsToFile", new RtsiSaveConnections(cap_));

    rpc_server_ = std::make_unique<xmlrpc_c::serverAbyss>(*rpc_registry_, RTSI_CAPTURE_PORT);

    rpc_thread_.reset(new std::thread([&](){
        rpc_server_->run();
    }));
    
}