cmake_minimum_required (VERSION 3.22.1)

project (rtsi_analysis)

set(CMAKE_CXX_STANDARD 14)

add_executable(
    ${PROJECT_NAME}
    RtsiRpc.cpp
    RtsiReciepSetup.cpp
    RtsiRecipe.cpp
    RtsiParserData.cpp
    RtsiParser.cpp
    RtsiCapture.cpp
    TcpMessage.cpp
    TcpCapture.cpp
    main.cpp
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ./
)

set(
    XMLRPC_C_LIBRARY
    xmlrpc++ 
    xmlrpc_server++ 
    xmlrpc_server_abyss++
)

target_link_libraries(
    ${PROJECT_NAME}
    pthread
    curl
    ssl
    crypto
    ${XMLRPC_C_LIBRARY}
    libpcap.a
)
