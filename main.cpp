#include "TcpCapture.hpp"
#include "RtsiCapture.hpp"
#include "RtsiRpc.hpp"

int main(int argc, char** argv) {
    RtsiCapture* cap = new RtsiCapture("");
    RtsiRpc* rpc = new RtsiRpc(*cap);
    
    registerTcpMessage(cap);
    
    rpc->init();

    return startTcpCapture(cap->getEthDevice(), "tcp port 30004");
}