#include "TcpCapture.hpp"
#include "RtsiCapture.hpp"

int main(int argc, char** argv) {
    registerTcpMessage(new RtsiCapture(argv[1]));
    return startTcpCapture(argv[1], "tcp port 30004");
}