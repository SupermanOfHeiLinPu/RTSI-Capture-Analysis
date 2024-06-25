#include "TcpCapture.hpp"
#include "RtsiCapture.hpp"
#include "RtsiRpc.hpp"

int main(int argc, char** argv) {
    RtsiCapture* cap = new RtsiCapture(argv[1]);
    RtsiRpc* rpc = new RtsiRpc(*cap);
    
    registerTcpMessage(cap);
    
    rpc->init();

    return startTcpCapture(argv[1], "tcp port 30004");
}