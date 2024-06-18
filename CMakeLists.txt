cmake_minimum_required (VERSION 3.22.1)

project (rtsi_analysis)

add_executable(
    ${PROJECT_NAME}
    TcpMessage.cpp
    TcpCapture.cpp
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ./
)

target_link_libraries(
    ${PROJECT_NAME}
    pcap
)
